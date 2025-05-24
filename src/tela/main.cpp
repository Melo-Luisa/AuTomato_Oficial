#include <TFT_eSPI.h>
#include <SPI.h>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define BUZZER_PIN 22
#define BUTTON_PIN 36

AsyncWebServer server(80);
const char* ssid = "Redmi Note 14";
const char* password = "giugiu24";
TFT_eSPI tft = TFT_eSPI();

const int tempoTrabalho = 25;  // pode ajustar para 1500 (25min) se quiser
const int tempoPausa = 5;

uint32_t lastSecond = 0;
int tempoRestante = tempoTrabalho;
bool emTrabalho = true;
bool somTocado = false;
bool cicloFinalizado = false;
bool pomodoroIniciado = false;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <title>Pomodoro com ESP32</title>
  </head>
  <body>
    <h1>Pomodoro em andamento...</h1>
    <p id="status">Status: Aguardando início...</p>

    <script>
      setInterval(() => {
        fetch("/status")
          .then(res => res.json())
          .then(data => {
            if (data.iniciado) {
              if (data.emTrabalho) {
                document.getElementById("status").innerHTML = "Status: Trabalho";
              } else {
                document.getElementById("status").innerHTML = "Status: Pausa";
              }
              if (data.fim) {
                alert("Ciclo finalizado!");
              }
            } else {
              document.getElementById("status").innerHTML = "Status: Aguardando início...";
            }
          })
          .catch(err => console.error(err));
      }, 2000);
    </script>
  </body>
</html>
)rawliteral";

// ----------------- Funções de som ---------------------
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

// ------------- Funções de fase ------------------------
void iniciarFaseTrabalho() {
  emTrabalho = true;
  tempoRestante = tempoTrabalho;
  Serial.println("Iniciando trabalho");
}

void encerrarFaseTrabalho() {
  playWorkEndTone();
  iniciarFasePausa();
}

void iniciarFasePausa() {
  emTrabalho = false;
  tempoRestante = tempoPausa;
  Serial.println("Iniciando pausa");
}

void encerrarFasePausa() {
  playBreakEndTone();
  cicloFinalizado = true;
  iniciarFaseTrabalho();
}

// ---------------- Tela -------------------------------
void atualizarTela() {
  uint16_t bgColor = emTrabalho ? TFT_DARKGREEN : TFT_NAVY;
  tft.fillRect(0, 40, 240, 160, bgColor);

  tft.setTextSize(2);
  tft.setCursor(20, 50);

  if (!pomodoroIniciado) {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.print("Pressione o botao");
    tft.setCursor(20, 80);
    tft.print("para iniciar!");
  } else {
    if (emTrabalho) {
      tft.setTextColor(TFT_WHITE, bgColor);
      tft.print("🔨 Trabalhando!");
    } else {
      tft.setTextColor(TFT_CYAN, bgColor);
      tft.print("☕ Hora da pausa!");
    }

    tft.setTextSize(6);
    tft.setCursor(80, 100);
    if (tempoRestante < 10) tft.print("0");
    tft.print(tempoRestante);
  }
}

// ---------------- Setup ------------------------------
void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);  // GPIO36 não tem INPUT_PULLUP

  Serial.begin(115200);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("Pomodoro: 25s foco / 5s pausa");

  atualizarTela();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado ao WiFi");
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send_P(200, "text/html", index_html);
  });

  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *req){
    String json = String("{\"fim\":") + (cicloFinalizado ? "true" : "false")
                + String(",\"emTrabalho\":") + (emTrabalho ? "true" : "false")
                + String(",\"iniciado\":") + (pomodoroIniciado ? "true" : "false")
                + String("}");
    req->send(200, "application/json", json);
    cicloFinalizado = false;
  });

  server.begin();
  Serial.println("Servidor iniciado");
}

// ---------------- Loop Principal ---------------------
void loop() {
  if (!pomodoroIniciado && digitalRead(BUTTON_PIN) == HIGH) {
    delay(200);  // debounce
    Serial.println("Pomodoro iniciado!");
    pomodoroIniciado = true;
    iniciarFaseTrabalho();
    atualizarTela();
    lastSecond = millis();
  }

  if (pomodoroIniciado && millis() - lastSecond >= 1000) {
    lastSecond += 1000;
    tempoRestante--;

    if (tempoRestante >= 0) {
      atualizarTela();
    }

    if (tempoRestante < 0 && !somTocado) {
      somTocado = true;

      if (emTrabalho) {
        encerrarFaseTrabalho();
      } else {
        encerrarFasePausa();
      }

      atualizarTela();
    }

    if (somTocado && tempoRestante >= 0) {
      somTocado = false;
    }
  }
}
