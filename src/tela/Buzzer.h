#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>
#define BUZZER_PIN 22

class Buzzer {
    private:
    int buzzerPin;

    public:
    Buzzer();
    Buzzer(int pin);
    ~Buzzer();	
    void playTone(int freq, int dur);
    void playWorkEndTone();
    void playBreakEndTone();
};

#endif // BUZZER_H