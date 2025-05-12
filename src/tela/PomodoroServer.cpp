#include "PomodoroServer.h"
// PomodoroServer.cpp
PomodoroServer::PomodoroServer(Pomodoro& p, const char* ssid, const char* password)
    : pomodoro(p), server(80), ssid(ssid), password(password) {}

void PomodoroServer::begin() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi conectado: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
        // html...
    });

    server.on("/status", HTTP_GET, [this](AsyncWebServerRequest* req) {
        String json = String("{\"fim\":") + (pomodoro.isCycleFinished() ? "true" : "false")
                    + String(",\"emTrabalho\":") + (pomodoro.isWorking() ? "true" : "false")
                    + String("}");
        req->send(200, "application/json", json);
        pomodoro.setCycleFinished(false);
    });

    server.begin();
}
