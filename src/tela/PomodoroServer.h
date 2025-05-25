
// #include <Arduino.h>
// #include <WiFi.h>
// #include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>
// #include "Pomodoro.h"

// class PomodoroServer {
// private:
//     const char* ssid;
//     const char* password;
//     AsyncWebServer *server;
//     static const char index_html[];
    
// public:
//     PomodoroServer();
//     PomodoroServer(const char* ssid, const char* password);
//     ~PomodoroServer();
//     void setSsid(const char* ssid);
//     const char* getSsid();
//     void setPassword(const char* password);
//     const char* getPassword();
//     void setup();
//     void rotaPrincipal();
//     void rotaStatus(Pomodoro& pomodoro);
//     void serverBegin();
// };