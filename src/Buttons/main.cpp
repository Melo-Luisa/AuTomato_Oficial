// #include <Arduino.h>

// #define BUTTON1_PIN  36
// #define BUTTON2_PIN  39

// void setup() {
//   Serial.begin(115200);
//   pinMode(BUTTON1_PIN, INPUT);
//   pinMode(BUTTON2_PIN, INPUT);
// }

// void loop() {
// Serial.println("Lendo botões...");

//   // Lê o estado dos botões
//   int button1State = digitalRead(BUTTON1_PIN);
//   int button2State = digitalRead(BUTTON2_PIN);

//   // // Imprime os estados dos botões
//   // Serial.print("Botão 1: ");
//   // Serial.println(button1State);
//   // Serial.print("Botão 2: ");
//   // Serial.println(button2State);

//   // Verifica se os botões estão pressionados
//   if (digitalRead(BUTTON1_PIN)) {
//     Serial.println("Botão 1 pressionado!");
//   }
  
//   if (digitalRead(BUTTON2_PIN)) {
//     Serial.println("Botão 2 pressionado!");
//   }

//   // Imprime os estados digitais dos pinos
// // Serial.println(digitalRead(BUTTON1_PIN));
// // Serial.println(digitalRead(BUTTON2_PIN));

//   delay(200); // Pequeno delay para evitar repetições rápidas
// }