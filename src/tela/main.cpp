#include <TFT_eSPI.h>
#include <SPI.h>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "Buzzer.h"
#include "Pomodoro.h"

AsyncWebServer server(80);

const char* ssid = "Morena branca";
const char* password = "jujuba25";

Pomodoro pomodoro; // 25 min work / 5 min break

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <title>Pomodoro com ESP32</title>
  </head>
  <body>
    <h1>Pomodoro em andamento...</h1>
    <p id="status">Status: Carregando...</p>

    <script>
      setInterval(() => {
        fetch("/status")
          .then(res => res.json())
          .then(data => {
            if (data.emTrabalho) {
              document.getElementById("status").innerHTML = "Status: Trabalho";
            } else {
              document.getElementById("status").innerHTML = "Status: Pausa";
            }
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

void setup() {
  pomodoro.updateDisplay();

  Serial.begin(115200);
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
    String json = String("{\"fim\":") + (pomodoro.isCycleFinished() ? "true" : "false")
                + String(",\"emTrabalho\":") + (pomodoro.isWorking() ? "true" : "false")
                + String("}");
    req->send(200, "application/json", json);
    pomodoro.setCycleFinished(false);
  });

  server.begin();
  Serial.println("Servidor iniciado");
}

void loop() {
  pomodoro.run();
}
