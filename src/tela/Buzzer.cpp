// #include "Buzzer.h"

// Buzzer::Buzzer() {
//     buzzerPin = 22;
//     pinMode(buzzerPin, OUTPUT);
// }
// Buzzer::Buzzer(int pin) {
//     buzzerPin = pin;
//     pinMode(buzzerPin, OUTPUT);
// }
// Buzzer::~Buzzer() {
// }
// void Buzzer::playTone(int freq, int dur) {
//     ledcSetup(0, freq, 8);
//     ledcAttachPin(buzzerPin, 0);
//     ledcWrite(0, 128);
//     delay(dur);
//     ledcWrite(0, 0);
// }
// void Buzzer::playWorkEndTone() {
//     playTone(1000, 200); delay(100);
//     playTone(1200, 200); delay(100);
//     playTone(1500, 300);
// }
// void Buzzer::playBreakEndTone() {
//     playTone(500, 500);
// }
