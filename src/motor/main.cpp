// #include <ESP32Servo.h>

// static const int servoPin = 5;

// Servo servo1;

// void girar1(){
//     for(int posDegrees = 0; posDegrees <= 90; posDegrees++) {
//         servo1.write(posDegrees);
//         Serial.println(posDegrees);
//         delay(20);
//     }
//     /*0 é o topo*/
// }

// void girar2(){
//     for(int posDegrees = 90; posDegrees >= 150; posDegrees--) {
//         servo1.write(posDegrees);
//         Serial.println(posDegrees);
//         delay(20);
//     }
// }
// void setup() {

//   Serial.begin(115200);
//   servo1.attach(servoPin); 
// }

// void loop() {
//     //girar2();
//     // delay(1000); // Wait for 1 second before the next iteration
//     girar1();
// }