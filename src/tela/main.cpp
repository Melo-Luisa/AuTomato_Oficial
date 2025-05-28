// // Projeto: Pomodoro com ESP32
// #include <TFT_eSPI.h>
// #include <SPI.h>
// #include <Arduino.h>
// #include <WiFi.h>
// #include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>
// #include <ESP32Servo.h>
// #include <FS.h>
// #include <SPIFFS.h>
// // #include "respostas.json"
// #include <ArduinoJson.h>

// #define BUZZER_PIN 13
// #define BUTTON_PIN 36
// #define BUZZER_CHANNEL 6    // Canal lógico PWM interno do ESP32
// static const int servoPin = 5;

// Servo servo1;

// AsyncWebServer server(80);
// const char* ssid = "rededoprojeto";
// const char* password = "arededoprojeto";
// TFT_eSPI tft = TFT_eSPI();

// String lerArquivo(fs::FS &fs, const char *caminho) {
//   fs::File file = fs.open(caminho, "r");
//   if (!file || file.isDirectory()) {
//     Serial.println("Erro ao abrir arquivo");
//     return String();
//   }

//   String conteudo;
//   while (file.available()) {
//     conteudo += (char)file.read();
//   }
//   file.close();
//   return conteudo;
// }


// uint32_t lastSecond = 0;
// int duracaoFoco = 5;
// int duracaoPausa = 5;
// int tempoRestante = duracaoFoco;
// bool emTrabalho = true;
// bool somTocado = false;
// bool cicloFinalizado = false;
// bool pomodoroIniciado = false;
// bool esperandoResposta = false;
// int num_ciclos = 1;
// String perguntaAtual = "";
// bool girou = false;
// //bool girouFim = false; // Variável para controlar o giro do servo no fim

// //int servoPos = 0; // posição atual do servo

// void salvarResposta(const String& pergunta, const String& resposta) {
//   JsonDocument doc;
//   doc["pergunta"] = pergunta;
//   doc["resposta"] = resposta;
//   fs::File file = SPIFFS.open("/respostas.json", FILE_APPEND);
//   if (file) {
//     serializeJson(doc, file);
//     file.println();
//     file.close();
//   }
// }

// void playTone(int freq, int dur) {
//   ledcSetup(BUZZER_CHANNEL, freq, 8);             
//   ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);      
//   ledcWrite(BUZZER_CHANNEL, 128);                
//   delay(dur);
//   ledcWrite(BUZZER_CHANNEL, 0); 
// }

// void playWorkEndTone() {
//   playTone(1000, 200); delay(100);
//   playTone(1200, 200); delay(100);
//   playTone(1500, 300);
// }

// void playBreakEndTone() {
//   playTone(500, 500);
// }

// void mostrarPerguntaTFT(const String& pergunta) {
//   tft.fillScreen(TFT_WHITE);
//   tft.setTextColor(TFT_YELLOW, TFT_WHITE);
//   tft.setTextSize(2);
//   int16_t x = (tft.width() - tft.textWidth((char*)pergunta.c_str())) / 2;
//   tft.setCursor(x, 50);
//   tft.println(pergunta);
// }

// void atualizarTela() {
//   if (esperandoResposta) return;
//   uint16_t bgColor = emTrabalho ? TFT_GREEN : TFT_SKYBLUE;

//   tft.fillScreen(TFT_WHITE);
//   tft.setTextSize(2);
//   tft.setCursor(20, 50);

//   // Centralizar o texto de status
//   String status = emTrabalho ? "Trabalhando..." : "Pausa...";
//   int16_t xStatus = (tft.width() - tft.textWidth((char*)status.c_str())) / 2;
//   int16_t xMsg = (tft.width() - tft.textWidth((char*)"Pressione o botao")) / 2;
//   int16_t xMsg2 = (tft.width() - tft.textWidth((char*)"para iniciar!")) / 2;

//   if (!pomodoroIniciado) {
//     tft.setTextColor(TFT_RED, TFT_WHITE);
//     tft.setCursor(xMsg, 50);
//     tft.print("Pressione o botao");
//     tft.setCursor(xMsg2, 80);
//     tft.print("para iniciar!");
//   } else {
//     tft.setTextColor(bgColor, TFT_WHITE);
//     tft.setCursor(xStatus, 50);
//     tft.print(status);

//     int minutos = tempoRestante / 60;
//     int segundos = tempoRestante % 60;
//     tft.setTextSize(6);
//     char tempoStr[6];
//     sprintf(tempoStr, "%02d:%02d", minutos, segundos);
//     int16_t xTempo = (tft.width() - tft.textWidth(tempoStr)) / 2;
//     tft.setCursor(xTempo, 100);
//     tft.print(tempoStr);
//   }
// }

// void girarServoInicio() {
  
//   for (int posDegrees = 0; posDegrees <= 180; posDegrees++) {
//     servo1.write(posDegrees);
//     delay(15);
//   }
//   //servoPos = 180;
  
// }

// void girarServoFim() {
//   for (int posDegrees = 180; posDegrees >= 0; posDegrees--) {
//     servo1.write(posDegrees);
//     delay(15);
//   }
//   //servoPos = 180;
// }

// void setup() {
//   Serial.begin(115200);
//   pinMode(BUZZER_PIN, OUTPUT);
//   pinMode(BUTTON_PIN, INPUT);
//   servo1.attach(servoPin);
//   SPIFFS.begin(true);

//   if (!SPIFFS.begin(true)) {
//     Serial.println("Erro ao montar SPIFFS");
//     return;
//   }

//   tft.init();
//   tft.setRotation(1);
//   tft.fillScreen(TFT_WHITE);
//   tft.setTextColor(TFT_BLACK, TFT_WHITE);
//   tft.setTextSize(2);
//   tft.setCursor(10, 10);
//   tft.print("Pomodoro configuravel");
//   delay(1000);

//   atualizarTela();

//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nWiFi conectado");
//   Serial.println(WiFi.localIP());

//   String ipStr = "IP: " + WiFi.localIP().toString();
//   tft.setTextSize(2);
//   tft.setTextColor(TFT_RED, TFT_WHITE);
//   tft.setCursor(240 - tft.textWidth((char*)ipStr.c_str()) - 5, 230);
//   tft.print(ipStr);

//   server.on("/", HTTP_GET, [](AsyncWebServerRequest *req){
//     req->send(SPIFFS, "/index.html", "text/html");
//   });

//   server.on("/status", HTTP_GET, [](AsyncWebServerRequest *req){
//     String json = String("{\"fim\":") + (cicloFinalizado ? "true" : "false") +
//                   String(",\"emTrabalho\":") + (emTrabalho ? "true" : "false") +
//                   String(",\"iniciado\":") + (pomodoroIniciado ? "true" : "false") +
//                   String(",\"pergunta\":\"") + (esperandoResposta ? perguntaAtual : "") + String("\"}");
//     req->send(200, "application/json", json);
//   });

//   server.on("/config", HTTP_POST, [](AsyncWebServerRequest *req){
//     if (req->hasParam("foco", true) && req->hasParam("pausa", true)) {
//       duracaoFoco = req->getParam("foco", true)->value().toInt()*60;
//       duracaoPausa = req->getParam("pausa", true)->value().toInt()*60;
//       tempoRestante = emTrabalho ? duracaoFoco : duracaoPausa;
//       req->send(200, "text/plain", "Ciclos atualizados");
//     } else req->send(400, "text/plain", "Parâmetros inválidos");
//   });

//   server.on("/resposta", HTTP_POST, [](AsyncWebServerRequest *req){
//     if (req->hasParam("resposta", true)) {
//       String resposta = req->getParam("resposta", true)->value();
//       salvarResposta(perguntaAtual, resposta);
//       esperandoResposta = false;
//       req->send(200, "text/plain", "Recebido");
//     }
//   });

//   server.begin();
// }

// bool teste = false;

// void loop() {
//   if (!pomodoroIniciado && digitalRead(BUTTON_PIN) == HIGH) {
//     delay(200);
//     perguntaAtual = "Como você se sente para estudar?";
//     esperandoResposta = true;
//     //mostrarPerguntaTFT(perguntaAtual);
//     teste = true;
//   }

//   if (esperandoResposta) return;

//   if (!pomodoroIniciado && teste) {
//     girarServoInicio();
//     playWorkEndTone();
//     tft.fillScreen(TFT_WHITE);
//     tft.setTextColor(TFT_BLACK, TFT_WHITE);
//     tft.setTextSize(2);
//     tft.setCursor(10, 10);
//     tft.print("Pomodoro configuravel");
//     pomodoroIniciado = true;
    
//     teste = false;
//     lastSecond = millis();
//   }

//   if (pomodoroIniciado && millis() - lastSecond >= 1000 && num_ciclos > 0) {
//     lastSecond += 1000;
//     tempoRestante--;

//     if (tempoRestante >= 0) atualizarTela();

//     if (tempoRestante < 0 && !somTocado) {
//       somTocado = true;
//       if (emTrabalho) {
//         playWorkEndTone();
//         emTrabalho = false;
//         tempoRestante = duracaoPausa;
//       } else {
//         playBreakEndTone();
//         cicloFinalizado = true;
//         if (num_ciclos > 1) {
//           emTrabalho = true;
//           tempoRestante = duracaoFoco;
//         }
//         num_ciclos--;
//       }
//       atualizarTela();
//     }

//     if (somTocado && tempoRestante >= 0)
//       somTocado = false;
//   }

//   if (num_ciclos <= 0 && cicloFinalizado) {
//     if (somTocado) {
//       somTocado = false;
//       cicloFinalizado = true;
//       playWorkEndTone();
//       perguntaAtual = "Como você se sente apos estudar?";
//       esperandoResposta = true;
//       //mostrarPerguntaTFT(perguntaAtual);
//     }

//     // Centralizar texto FIM!
//     if (!esperandoResposta) {
//       cicloFinalizado = false;
//       tft.fillScreen(TFT_WHITE);
//       tft.setTextColor(TFT_YELLOW, TFT_WHITE);
//       tft.setTextSize(6);
//       int16_t xFim = (tft.width() - tft.textWidth((char*)"FIM!")) / 2;
//       tft.setCursor(xFim, 10);
//       tft.print("FIM!");
//       girarServoFim();
//     }
//   }
// }