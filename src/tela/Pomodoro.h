#ifndef POMODORO_H
#define POMODORO_H

#include <TFT_eSPI.h>
#include "Buzzer.h"

class Pomodoro {
private:
    void initDisplay();                   // Initializes display

    int remainingTime;                    // Time in seconds
    bool working;
    bool tonePlayed;
    bool cycleFinished;
    unsigned long lastSecond;

    TFT_eSPI tft;
    Buzzer buzzer;
public:
    Pomodoro();                           // Default constructor
    Pomodoro(int workTimeSeconds, int buzzerPin); // Constructor with parameters

    void run();                           // Executes the Pomodoro logic
    void updateDisplay();                 // Updates the screen display

    // Getters
    int getRemainingTime();
    bool isWorking();
    bool isTonePlayed();
    bool isCycleFinished();

    // Setters
    void setRemainingTime(int time);
    void setWorking(bool state);
    void setTonePlayed(bool state);
    void setCycleFinished(bool state);

};

#endif
