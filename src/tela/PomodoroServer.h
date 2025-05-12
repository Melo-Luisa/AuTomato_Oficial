// PomodoroServer.h
#ifndef POMODOROSERVER_H
#define POMODOROSERVER_H

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "Pomodoro.h"

class PomodoroServer {
private:
    Pomodoro& pomodoro;
    AsyncWebServer server;
    const char* ssid;
    const char* password;

    const char* index_html = R"rawliteral(
    <!DOCTYPE html>
    <html>
      <head>
        <title>Pomodoro com ESP32</title>
      </head>
      <body>
        <h1>Pomodoro em andamento...</h1>
        <p id="status">Status: Carregando...</p>

        <script>
          setInterval(() => {
            fetch("/status")
              .then(res => res.json())
              .then(data => {
                if (data.working) {
                  document.getElementById("status").innerHTML = "Status: Trabalho";
                } else {
                  document.getElementById("status").innerHTML = "Status: Pausa";
                }
                if (data.finished) {
                  alert("Ciclo finalizado!");
                }
              })
              .catch(err => console.error(err));
          }, 2000);
        </script>
      </body>
    </html>
    )rawliteral";

public:
    PomodoroServer(Pomodoro& p, const char* ssid, const char* password);
    void begin();
};

#endif
