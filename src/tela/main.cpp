// #include <TFT_eSPI.h>
// #include <SPI.h>
// #include <Arduino.h>
// #include <WiFi.h>
// #include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>
// #include "Buzzer.h"
// #include "Pomodoro.h"

// AsyncWebServer server(80);

// const char* ssid = "Morena branca";
// const char* password = "jujuba25";

// Pomodoro pomodoro; // 25 min work / 5 min break

// const char index_html[] PROGMEM = R"rawliteral(
// <!DOCTYPE html>
// <html>
//   <head>
//     <title>Pomodoro com ESP32</title>
//   </head>
//   <body>
//     <h1>Pomodoro em andamento...</h1>
//     <p id="status">Status: Carregando...</p>

//     <script>
//       setInterval(() => {
//         fetch("/status")
//           .then(res => res.json())
//           .then(data => {
//             if (data.emTrabalho) {
//               document.getElementById("status").innerHTML = "Status: Trabalho";
//             } else {
//               document.getElementById("status").innerHTML = "Status: Pausa";
//             }
//             if (data.fim) {
//               alert("Ciclo finalizado!");
//             }
//           })
//           .catch(err => console.error(err));
//       }, 2000);
//     </script>
//   </body>
// </html>
// )rawliteral";

// void setup() {
//   pomodoro.updateDisplay();

//   Serial.begin(115200);
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }

//   Serial.println("\nConectado ao WiFi");
//   Serial.print("IP do ESP32: ");
//   Serial.println(WiFi.localIP());

//   server.on("/", HTTP_GET, [](AsyncWebServerRequest *req){
//     req->send_P(200, "text/html", index_html);
//   });

//   server.on("/status", HTTP_GET, [](AsyncWebServerRequest *req){
//     String json = String("{\"fim\":") + (pomodoro.isCycleFinished() ? "true" : "false")
//                 + String(",\"emTrabalho\":") + (pomodoro.isWorking() ? "true" : "false")
//                 + String("}");
//     req->send(200, "application/json", json);
//     pomodoro.setCycleFinished(false);
//   });

//   server.begin();
//   Serial.println("Servidor iniciado");
// }

// void loop() {
//   pomodoro.run();
// }

#include <TFT_eSPI.h>
#include <SPI.h>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define BUZZER_PIN 22

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
    <p id="status">Status: Carregando...</p>

    <form onsubmit="enviarConfiguracao(); return false;">
      <label>Tempo de trabalho: <input type="number" id="foco" value="25*60"></label><br>
      <label>Tempo de pausa: <input type="number" id="pausa" value="5*60"></label><br>
      <button type="submit">Atualizar Ciclos</button>
    </form>

    <script>
      function enviarConfiguracao() {
        const foco = document.getElementById("foco").value;
        const pausa = document.getElementById("pausa").value;

        fetch("/config", {
          method: "POST",
          headers: { "Content-Type": "application/x-www-form-urlencoded" },
          body: `foco=${foco}&pausa=${pausa}`
        }).then(res => {
          if (res.ok) {
            alert("Ciclos atualizados!");
          }
        });
      }

      setInterval(() => {
        fetch("/status")
          .then(res => res.json())
          .then(data => {
            document.getElementById("status").innerHTML = data.emTrabalho ? "Status: Trabalho" : "Status: Pausa";
            if (data.fim) {
              alert("Ciclo finalizado!");
            }
          })
          .catch(err => console.error(err));
      }, 2000);
    </script>
  </body>
</html>
)rawliteral";

// Variáveis de controle
uint32_t lastSecond = 0;
int duracaoFoco = 25*60; // 25 minutos
int duracaoPausa = 5*60; // 5 minutos
int tempoRestante = duracaoFoco;
bool emTrabalho = true;
bool somTocado = false;
bool cicloFinalizado = false;

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

void atualizarTela() {
  tft.fillRect(0, 40, 240, 160, TFT_BLACK);
  tft.setCursor(20, 50);
  tft.setTextColor(emTrabalho ? TFT_GREEN : TFT_CYAN, TFT_BLACK);
  tft.setTextSize(2);
  tft.print(emTrabalho ? "Tempo de foco:" : "Pausa:");

  // Exibir mm:ss
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

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  tft.init(); 
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("Pomodoro configuravel");
  delay(1000);

  lastSecond = millis();
  atualizarTela();

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  delay(1000);
  Serial.println("\nConectado ao WiFi");
  Serial.print("IP do ESP32: "); 
  Serial.println(WiFi.localIP());

  // Mostra IP no canto inferior direito em amarelo
  String ipStr = "IP: " + WiFi.localIP().toString();
  int16_t x1, y1;
  uint16_t w, h;
  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  w = tft.textWidth((char*)ipStr.c_str());  // Calcula a largura do texto
  tft.setCursor(240 - w - 5, 230);  // posição ajustada para canto inferior direito
  tft.print(ipStr);
  
  // Página principal
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send_P(200, "text/html", index_html);
  });

  // Status JSON
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *req){
    String json = String("{\"fim\":") + (cicloFinalizado ? "true" : "false")
                + String(",\"emTrabalho\":") + (emTrabalho ? "true" : "false")
                + String("}");
    req->send(200, "application/json", json);
    cicloFinalizado = false;
  });

  // Rota para configurar tempos
  server.on("/config", HTTP_POST, [](AsyncWebServerRequest *req){
    if (req->hasParam("foco", true) && req->hasParam("pausa", true)) {
      duracaoFoco = req->getParam("foco", true)->value().toInt()*60;
      duracaoPausa = req->getParam("pausa", true)->value().toInt()*60;
      tempoRestante = emTrabalho ? duracaoFoco : duracaoPausa;

      Serial.printf("Novo foco: %ds, nova pausa: %ds\n", duracaoFoco, duracaoPausa);
      req->send(200, "text/plain", "Ciclos atualizados");
    } else {
      req->send(400, "text/plain", "Parâmetros inválidos");
    }
  });

  server.begin();
  Serial.println("Servidor iniciado");
}

void loop() {
  if (millis() - lastSecond >= 1000) {
    lastSecond += 1000;
    tempoRestante--;

    if (tempoRestante >= 0) {
      atualizarTela();
    }

    if (tempoRestante < 0 && !somTocado) {
      somTocado = true;

      if (emTrabalho) {
        playWorkEndTone();
        tempoRestante = duracaoPausa;
      } else {
        cicloFinalizado = true;
        playBreakEndTone();
        tempoRestante = duracaoFoco;
      }

      emTrabalho = !emTrabalho;
      atualizarTela();
    }

    if (somTocado && tempoRestante >= 0) {
      somTocado = false;
    }
  }
}