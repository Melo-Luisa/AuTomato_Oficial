// #include <TFT_eSPI.h>
// #include <SPI.h>
// #include <Arduino.h>

// #define BUZZER_PIN 22

// TFT_eSPI tft = TFT_eSPI();

// uint32_t lastSecond = 0;
// int tempoRestante = 25 * 60;  // Tempo em segundos (25 minutos)
// bool emTrabalho = true;
// bool somTocado = false;

// void playTone(int frequency, int duration) {
//   ledcSetup(0, frequency, 8);
//   ledcAttachPin(BUZZER_PIN, 0);
//   ledcWrite(0, 128);
//   delay(duration);
//   ledcWrite(0, 0);
// }

// void playWorkEndTone() {
//   playTone(1000, 200);
//   delay(100);
//   playTone(1200, 200);
//   delay(100);
//   playTone(1500, 300);
// }

// void playBreakEndTone() {
//   playTone(500, 500);
// }

// void atualizarTela() {
//   tft.fillRect(0, 40, 240, 160, TFT_BLACK); // Limpa a área principal
//   tft.setCursor(20, 50);
//   tft.setTextColor(emTrabalho ? TFT_GREEN : TFT_CYAN, TFT_BLACK);
//   tft.setTextSize(2);
//   tft.print(emTrabalho ? "Tempo de foco:" : "Pausa:");

//   int minutos = tempoRestante / 60;
//   int segundos = tempoRestante % 60;

//   tft.setTextSize(6);
//   tft.setCursor(50, 100);
//   if (minutos < 10) tft.print("0");
//   tft.print(minutos);
//   tft.print(":");
//   if (segundos < 10) tft.print("0");
//   tft.print(segundos);
// }

// void setup() {
//   pinMode(BUZZER_PIN, OUTPUT);

//   tft.init();
//   tft.setRotation(1);
//   tft.fillScreen(TFT_BLACK);
//   tft.setTextColor(TFT_YELLOW, TFT_BLACK);
//   tft.setTextSize(2);
//   tft.setCursor(10, 10);
//   tft.print("Pomodoro - 25min foco / 5min pausa");

//   lastSecond = millis();
//   atualizarTela();
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
//         tempoRestante = 5 * 60; // 5 minutos de pausa
//       } else {
//         playBreakEndTone();
//         tempoRestante = 25 * 60; // 25 minutos de trabalho
//       }

//       emTrabalho = !emTrabalho;
//       atualizarTela();
//     }

//     if (somTocado && tempoRestante >= 0) {
//       somTocado = false;
//     }
//   }
// }
