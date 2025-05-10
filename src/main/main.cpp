
//******************************* FUNCIONA TELA TFT ************************************ */
// #include <Arduino.h>
// #include <TFT_eSPI.h>

// //Inicialização Display
// TFT_eSPI d = TFT_eSPI();
// TFT_eSprite ball = TFT_eSprite(&d);

// void setup(){
//   d.init();
//   d.setRotation(1);
//   d.fillScreen(TFT_BLACK);

//   ball.setColorDepth(8);
//   //Cria a Sprite um pouco maior do que o objeto que será desenhado dentro da Sprite
//   ball.createSprite(40, 40); 
// }

// int x = 0;
// int y = 0;

// void loop(){
//   //img.fillCircle(20, 20, 15,TFT_BLACK); //Também funcionou. Não vi diferença do fillSprite abaixo.
//   //Apaga o conteúdo da sprite para não gerar o "rastro" e ficar só o movimento do objeto
//   ball.fillSprite(TFT_BLACK);
//   x=x+1;
//   if(x>480){
//     x=0;
//   }
//   ball.fillCircle(20, 20, 15, TFT_RED); //estatico, não mexe mais

//   //Movimenta a sprite toda, e não o objeto dentro da sprite
//   ball.pushSprite(x, y);
  

// }
/************************************************************************************************* */
/********************************************* MOTOR - FUNCIONA MEIO XÔXÔ******************************************** */
// #include <ESP32Servo.h>
// int APin = 5;
// ESP32PWM pwm;
// int freq = 1000;
// void setup() {
// 	// Allow allocation of all timers
// 	ESP32PWM::allocateTimer(0);
// 	ESP32PWM::allocateTimer(1);
// 	ESP32PWM::allocateTimer(2);
// 	ESP32PWM::allocateTimer(3);
// 	Serial.begin(115200);
// 	pwm.attachPin(APin, freq, 10); // 1KHz 10 bits

// }
// void loop() {

// 	// fade the LED on thisPin from off to brightest:
// 	for (float brightness = 0; brightness <= 0.5; brightness += 0.001) {
// 		// Write a unit vector value from 0.0 to 1.0
// 		pwm.writeScaled(brightness);
// 		delay(2);
// 	}
// 	//delay(1000);
// 	// fade the LED on thisPin from brithstest to off:
// 	for (float brightness = 0.5; brightness >= 0; brightness -= 0.001) {
// 		freq += 10;
// 		// Adjust the frequency on the fly with a specific brightness
// 		// Frequency is in herts and duty cycle is a unit vector 0.0 to 1.0
// 		pwm.adjustFrequency(freq, brightness); // update the time base of the PWM
// 		delay(2);
// 	}
// 	// pause between LEDs:
// 	delay(1000);
// 	freq = 1000;
// 	pwm.adjustFrequency(freq, 0.0);    // reset the time base
// }
/***************************** BUZZER FUNCIONA ***************************************** */
// #include <Arduino.h>

// // Define the GPIO pin connected to the buzzer
// #define BUZZER_PIN 22

// void playTone(int frequency, int duration) {
//   ledcSetup(0, frequency, 8); // Set the frequency
//   ledcAttachPin(BUZZER_PIN, 0);
//   ledcWrite(0, 128); // 50% duty cycle
//   delay(duration);
//   ledcWrite(0, 0); // Stop the sound
// }


// void playRightAnswerTone() {
//   // Resposta correta: tocar uma sequência melódica
//   playTone(1000, 200); // 1 kHz tom por 200 ms
//   delay(100);          // Pausa curta
//   playTone(1200, 200); // 1.2 kHz tom por 200 ms
//   delay(100);          // Pausa curta
//   playTone(1500, 300); // 1.5 kHz tom por 300 ms
// }


// void playWrongAnswerTone() {
//   // Wrong answer: play a low-pitched longer beep
//   playTone(500, 500); // 500 Hz tone for 500 ms
// }



// void setup() {
//   pinMode(BUZZER_PIN, OUTPUT);
  
//   // Example: Play right and wrong answer tones
//   playRightAnswerTone();
//   delay(1000); // Wait for 1 second
//   playWrongAnswerTone();
// }

// void loop() {
//   // Nothing to do here
// }
/***************************************************************************** */
/*
 An example digital clock using a TFT LCD screen to show the time.
 Demonstrates use of the font printing routines. (Time updates but date does not.)

 It uses the time of compile/upload to set the time
 For a more accurate clock, it would be better to use the RTClib library.
 But this is just a demo...

 Make sure all the display driver and pin connections are correct by
 editing the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################

 Based on clock sketch by Gilchrist 6/2/2014 1.0

A few colour codes:

code	color
0x0000	Black
0xFFFF	White
0xBDF7	Light Gray
0x7BEF	Dark Gray
0xF800	Red
0xFFE0	Yellow
0xFBE0	Orange
0x79E0	Brown
0x7E0	Green
0x7FF	Cyan
0x1F	Blue
0xF81F	Pink

 */

// #include <TFT_eSPI.h> // Hardware-specific library
// #include <SPI.h>

// #define TFT_GREY 0x5AEB

// TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

// uint32_t targetTime = 0;                    // for next 1 second timeout

// static uint8_t conv2d(const char* p); // Forward declaration needed for IDE 1.6.x

// uint8_t hh = conv2d(__TIME__), mm = conv2d(__TIME__ + 3), ss = conv2d(__TIME__ + 6); // Get H, M, S from compile time

// byte omm = 99, oss = 99;
// byte xcolon = 0, xsecs = 0;
// unsigned int colour = 0;

// void setup(void) {
//   //Serial.begin(115200);
//   tft.init();
//   tft.setRotation(1);
//   tft.fillScreen(TFT_BLACK);

//   tft.setTextSize(1);
//   tft.setTextColor(TFT_YELLOW, TFT_BLACK);

//   targetTime = millis() + 1000;
// }

// void loop() {
//   if (targetTime < millis()) {
//     // Set next update for 1 second later
//     targetTime = millis() + 1000;

//     // Adjust the time values by adding 1 second
//     ss++;              // Advance second
//     if (ss == 60) {    // Check for roll-over
//       ss = 0;          // Reset seconds to zero
//       omm = mm;        // Save last minute time for display update
//       mm++;            // Advance minute
//       if (mm > 59) {   // Check for roll-over
//         mm = 0;
//         hh++;          // Advance hour
//         if (hh > 23) { // Check for 24hr roll-over (could roll-over on 13)
//           hh = 0;      // 0 for 24 hour clock, set to 1 for 12 hour clock
//         }
//       }
//     }


//     // Update digital time
//     int xpos = 0;
//     int ypos = 85; // Top left corner ot clock text, about half way down
//     int ysecs = ypos + 24;

//     if (omm != mm) { // Redraw hours and minutes time every minute
//       omm = mm;
//       // Draw hours and minutes
//       if (hh < 10) xpos += tft.drawChar('0', xpos, ypos, 8); // Add hours leading zero for 24 hr clock
//       xpos += tft.drawNumber(hh, xpos, ypos, 8);             // Draw hours
//       xcolon = xpos; // Save colon coord for later to flash on/off later
//       xpos += tft.drawChar(':', xpos, ypos - 8, 8);
//       if (mm < 10) xpos += tft.drawChar('0', xpos, ypos, 8); // Add minutes leading zero
//       xpos += tft.drawNumber(mm, xpos, ypos, 8);             // Draw minutes
//       xsecs = xpos; // Sae seconds 'x' position for later display updates
//     }
//     if (oss != ss) { // Redraw seconds time every second
//       oss = ss;
//       xpos = xsecs;

//       if (ss % 2) { // Flash the colons on/off
//         tft.setTextColor(0x39C4, TFT_BLACK);        // Set colour to grey to dim colon
//         tft.drawChar(':', xcolon, ypos - 8, 8);     // Hour:minute colon
//         xpos += tft.drawChar(':', xsecs, ysecs, 6); // Seconds colon
//         tft.setTextColor(TFT_YELLOW, TFT_BLACK);    // Set colour back to yellow
//       }
//       else {
//         tft.drawChar(':', xcolon, ypos - 8, 8);     // Hour:minute colon
//         xpos += tft.drawChar(':', xsecs, ysecs, 6); // Seconds colon
//       }

//       //Draw seconds
//       if (ss < 10) xpos += tft.drawChar('0', xpos, ysecs, 6); // Add leading zero
//       tft.drawNumber(ss, xpos, ysecs, 6);                     // Draw seconds
//     }
//   }
// }


// // Function to extract numbers from compile time string
// static uint8_t conv2d(const char* p) {
//   uint8_t v = 0;
//   if ('0' <= *p && *p <= '9')
//     v = *p - '0';
//   return 10 * v + *++p - '0';
// }



