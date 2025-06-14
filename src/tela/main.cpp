// Projeto: Pomodoro com ESP32
#include <TFT_eSPI.h>
#include <SPI.h>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h>
#include <FS.h>
#include <SPIFFS.h>
#include "respostas.json"
#include <ArduinoJson.h>

#define BUZZER_PIN 22
#define BUTTON_PIN 36
static const int servoPin = 5;

Servo servo1;

AsyncWebServer server(80);
const char* ssid = "Redmi Note 14";
const char* password = "giugiu24";
TFT_eSPI tft = TFT_eSPI();

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <title>Pomodoro com ESP32</title>
  </head>
  <body>
    <h1>Pomodoro em andamento...</h1>
    <p id="status">Status: Aguardando início...</p>

    <form onsubmit="enviarConfiguracao(); return false;">
      <label>Tempo de trabalho: <input type="number" id="foco" value="1500"></label><br>
      <label>Tempo de pausa: <input type="number" id="pausa" value="300"></label><br>
      <button type="submit">Atualizar Ciclos</button>
    </form>

    <div id="pesquisa" style="display:none;">
      <p id="pergunta"></p>
      <input type="text" id="resposta">
      <button onclick="enviarResposta()">Enviar</button>
    </div>

    <script>
      function enviarConfiguracao() {
        const foco = document.getElementById("foco").value;
        const pausa = document.getElementById("pausa").value;

        fetch("/config", {
          method: "POST",
          headers: { "Content-Type": "application/x-www-form-urlencoded" },
          body: `foco=${foco}&pausa=${pausa}`
        }).then(res => {
          if (res.ok) alert("Ciclos atualizados!");
        });
      }

      function exibirPergunta(pergunta) {
        document.getElementById("pergunta").innerText = pergunta;
        document.getElementById("pesquisa").style.display = "block";
      }

      function enviarResposta() {
        const resp = document.getElementById("resposta").value;
        fetch("/resposta", {
          method: "POST",
          headers: { "Content-Type": "application/x-www-form-urlencoded" },
          body: `resposta=${resp}`
        }).then(res => {
          if (res.ok) {
            document.getElementById("pesquisa").style.display = "none";
          }
        });
      }

      setInterval(() => {
        fetch("/status")
          .then(res => res.json())
          .then(data => {
            document.getElementById("status").innerHTML = data.emTrabalho ? "Status: Trabalho" : "Status: Pausa";
            if (data.pergunta) {
              exibirPergunta(data.pergunta);
            }
          });
      }, 2000);
    </script>
  </body>
</html>
)rawliteral";

uint32_t lastSecond = 0;
int duracaoFoco = 5;
int duracaoPausa = 5;
int tempoRestante = duracaoFoco;
bool emTrabalho = true;
bool somTocado = false;
bool cicloFinalizado = false;
bool pomodoroIniciado = false;
bool esperandoResposta = false;
int num_ciclos = 1;
String perguntaAtual = "";
bool girou = false;
//bool girouFim = false; // Variável para controlar o giro do servo no fim

int servoPos = 0; // posição atual do servo

void salvarResposta(const String& pergunta, const String& resposta) {
  JsonDocument doc;
  doc["pergunta"] = pergunta;
  doc["resposta"] = resposta;
  fs::File file = SPIFFS.open("/respostas.json", FILE_APPEND);
  if (file) {
    serializeJson(doc, file);
    file.println();
    file.close();
  }
}

void playTone(int freq, int dur) {
  ledcSetup(0, freq, 8);
  ledcAttachPin(BUZZER_PIN, 0);
  ledcWrite(0, 128);
  delay(dur);
  ledcWrite(0, 0);
}

void playWorkEndTone() {
  playTone(1000, 200); delay(100);
  playTone(1200, 200); delay(100);
  playTone(1500, 300);
}

void playBreakEndTone() {
  playTone(500, 500);
}

void mostrarPerguntaTFT(const String& pergunta) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(2);
  tft.setCursor(10, 50);
  tft.println(pergunta);
}

void atualizarTela() {
  if (esperandoResposta) return;
  uint16_t bgColor = emTrabalho ? TFT_GREEN : TFT_SKYBLUE;

  tft.setTextSize(2);
  tft.setCursor(20, 50);

  if (!pomodoroIniciado) {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.print("Pressione o botao");
    tft.setCursor(20, 80);
    tft.print("para iniciar!");
  } else {
    tft.setTextColor(bgColor, TFT_BLACK);
    tft.setCursor(20, 50);
    tft.print(emTrabalho ? "Trabalhando..." : "Pausa...");

    int minutos = tempoRestante / 60;
    int segundos = tempoRestante % 60;
    tft.setTextSize(6);
    tft.setCursor(60, 100);
    if (minutos < 10) tft.print("0");
    tft.print(minutos);
    tft.print(":");
    if (segundos < 10) tft.print("0");
    tft.print(segundos);
  }
}

// void girarServoInicio() {
//   if (servoPos == 0) {
//     for (int posDegrees = 0; posDegrees <= 10; posDegrees++) {
//       servo1.(1);
//       delay(15);
//     }
//     servoPos = 180;
//   }
// }

// void girarServoFim() {
//   if (servoPos == 180) {
//     for (int posDegrees = 10; posDegrees >= 0; posDegrees--) {
//       servo1.write(-1);
//       delay(15);
//     }
//     servoPos = 0;
//   }
// }

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  servo1.attach(servoPin);
  SPIFFS.begin(true);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("Pomodoro configuravel");
  delay(1000);

  atualizarTela();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");
  Serial.println(WiFi.localIP());

  String ipStr = "IP: " + WiFi.localIP().toString();
  tft.setTextSize(2);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setCursor(240 - tft.textWidth((char*)ipStr.c_str()) - 5, 230);
  tft.print(ipStr);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send_P(200, "text/html", index_html);
  });

  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *req){
    String json = String("{\"fim\":") + (cicloFinalizado ? "true" : "false") +
                  String(",\"emTrabalho\":") + (emTrabalho ? "true" : "false") +
                  String(",\"iniciado\":") + (pomodoroIniciado ? "true" : "false") +
                  String(",\"pergunta\":\"") + (esperandoResposta ? perguntaAtual : "") + String("\"}");
    req->send(200, "application/json", json);
  });

  server.on("/config", HTTP_POST, [](AsyncWebServerRequest *req){
    if (req->hasParam("foco", true) && req->hasParam("pausa", true)) {
      duracaoFoco = req->getParam("foco", true)->value().toInt()*60;
      duracaoPausa = req->getParam("pausa", true)->value().toInt()*60;
      tempoRestante = emTrabalho ? duracaoFoco : duracaoPausa;
      req->send(200, "text/plain", "Ciclos atualizados");
    } else req->send(400, "text/plain", "Parâmetros inválidos");
  });

  server.on("/resposta", HTTP_POST, [](AsyncWebServerRequest *req){
    if (req->hasParam("resposta", true)) {
      String resposta = req->getParam("resposta", true)->value();
      salvarResposta(perguntaAtual, resposta);
      esperandoResposta = false;
      req->send(200, "text/plain", "Recebido");
    }
  });

  server.begin();
}

bool teste = false;

void loop() {
  if (!pomodoroIniciado && digitalRead(BUTTON_PIN) == HIGH) {
    delay(200);
    perguntaAtual = "Como você se sente para estudar?";
    esperandoResposta = true;
    //mostrarPerguntaTFT(perguntaAtual);
    teste = true;
  }

  if (esperandoResposta) return;

  if (!pomodoroIniciado && teste) {
    playWorkEndTone();
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.print("Pomodoro configuravel");
    //if(girouFim==false){
      //Serial.println("Servo girado para 180 graus");
      //girouFim = true;
    //}
    pomodoroIniciado = true;
    
    teste = false;
    lastSecond = millis();
  }

  if (pomodoroIniciado && millis() - lastSecond >= 1000 && num_ciclos > 0) {
    lastSecond += 1000;
    tempoRestante--;

    if (tempoRestante >= 0) atualizarTela();

    if (tempoRestante < 0 && !somTocado) {
      somTocado = true;
      if (emTrabalho) {
        playWorkEndTone();
        emTrabalho = false;
        tempoRestante = duracaoPausa;
      } else {
        playBreakEndTone();
        cicloFinalizado = true;
        if (num_ciclos > 1) {
          emTrabalho = true;
          tempoRestante = duracaoFoco;
        }
        num_ciclos--;
      }
      atualizarTela();
    }

    if (somTocado && tempoRestante >= 0)
      somTocado = false;
  }

  if (num_ciclos <= 0 && cicloFinalizado) {
    if (somTocado) {
      somTocado = false;
      cicloFinalizado = true;
      playWorkEndTone();
      perguntaAtual = "Como você se sente apos estudar?";
      esperandoResposta = true;
      //mostrarPerguntaTFT(perguntaAtual);
    }

    if (!esperandoResposta) {
      cicloFinalizado = false;
      tft.fillScreen(TFT_BLACK);
      tft.setTextColor(TFT_YELLOW, TFT_BLACK);
      tft.setTextSize(6);
      tft.setCursor(10, 10);
      tft.print("FIM!");
      if(girou==false){
        // servo1.write(0);
        // delay(200);
        //servo1.write(180);
        //delay(1000); 
        //girou = true;
      }
    }
  }
}
