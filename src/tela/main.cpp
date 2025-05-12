// #include <TFT_eSPI.h>
// #include <SPI.h>
// #include <Arduino.h>
// #include <WiFi.h>
// #include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>

// #define BUZZER_PIN 22

// AsyncWebServer server(80);
// const char* ssid = "Morena branca";
// const char* password = "jujuba25";
// TFT_eSPI tft = TFT_eSPI();

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
//             // Atualiza o texto conforme o estado atual
//             if (data.emTrabalho) {
//               document.getElementById("status").innerHTML = "Status: Trabalho";
//             } else {
//               document.getElementById("status").innerHTML = "Status: Pausa";
//             }
//             // Se chegou ao fim do intervalo, dispara alerta
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

// uint32_t lastSecond = 0;
// int tempoRestante   = 25;  // 25 segundos de trabalho
// bool emTrabalho     = true;
// bool somTocado      = false;
// bool cicloFinalizado = false;

// void playTone(int freq, int dur) {
//   ledcSetup(0, freq, 8);
//   ledcAttachPin(BUZZER_PIN, 0);
//   ledcWrite(0, 128);
//   delay(dur);
//   ledcWrite(0, 0);
// }

// void playWorkEndTone() {
//   playTone(1000, 200); delay(100);
//   playTone(1200, 200); delay(100);
//   playTone(1500, 300);
// }

// void playBreakEndTone() {
//   playTone(500, 500);
// }

// void atualizarTela() {
//   tft.fillRect(0, 40, 240, 160, TFT_BLACK);
//   tft.setCursor(20, 50);
//   tft.setTextColor(emTrabalho ? TFT_GREEN : TFT_CYAN, TFT_BLACK);
//   tft.setTextSize(2);
//   tft.print(emTrabalho ? "Tempo de foco:" : "Pausa:");

//   tft.setTextSize(6);
//   tft.setCursor(80, 100);
//   if (tempoRestante < 10) tft.print("0");
//   tft.print(tempoRestante);
// }

// void setup() {
//   pinMode(BUZZER_PIN, OUTPUT);
//   tft.init(); 
//   tft.setRotation(1);
//   tft.fillScreen(TFT_BLACK);
//   tft.setTextColor(TFT_YELLOW, TFT_BLACK);
//   tft.setTextSize(2);
//   tft.setCursor(10,10);
//   tft.print("Pomodoro: 25s foco / 5s pausa");

//   lastSecond = millis();
//   atualizarTela();

//   Serial.begin(115200);
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nConectado ao WiFi");
//   Serial.print("IP do ESP32: "); 
//   Serial.println(WiFi.localIP());

//   // Rota principal
//   server.on("/", HTTP_GET, [](AsyncWebServerRequest *req){
//     req->send_P(200, "text/html", index_html);
//   });

//   // Rota de status retorna JSON com fim e emTrabalho
//   server.on("/status", HTTP_GET, [](AsyncWebServerRequest *req){
//     String json = String("{\"fim\":") + (cicloFinalizado ? "true" : "false")
//                 + String(",\"emTrabalho\":") + (emTrabalho ? "true" : "false")
//                 + String("}");
//     req->send(200, "application/json", json);
//     // reseta flag de fim para não disparar alerta repetido
//     cicloFinalizado = false;
//   });

//   server.begin();
//   Serial.println("Servidor iniciado");
// }

// void loop() {
//   if (millis() - lastSecond >= 1000) {
//     lastSecond += 1000;
//     tempoRestante--;

//     if (tempoRestante >= 0) {
//       atualizarTela();
//     }

//     if (tempoRestante < 0 && !somTocado) {
//       somTocado = true;

//       if (emTrabalho) {
//         playWorkEndTone();
//         tempoRestante = 5;   // pausa de 5s
//       } else {
//         cicloFinalizado = true;
//         playBreakEndTone();
//         tempoRestante = 25;  // foco de 25s
//       }

//       emTrabalho = !emTrabalho;
//       atualizarTela();
//     }

//     if (somTocado && tempoRestante >= 0) {
//       somTocado = false;
//     }
//   }
// }