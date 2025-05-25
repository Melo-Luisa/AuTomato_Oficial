
// #include "PomodoroServer.h"

// PomodoroServer::PomodoroServer() {
//     ssid = "ALHN-1EB0";
//     password = "02917030";
//   server = new AsyncWebServer(80);
//   static const char index_html[] PROGMEM = R"rawliteral(
//     <!DOCTYPE html>
//     <html>
//       <head>
//         <title>Pomodoro com ESP32</title>
//       </head>
//       <body>
//         <h1>Pomodoro em andamento...</h1>
//         <p id="status">Status: Carregando...</p>
    
//         <script>
//           setInterval(() => {
//             fetch("/status")
//               .then(res => res.json())
//               .then(data => {
//                 if (data.emTrabalho) {
//                   document.getElementById("status").innerHTML = "Status: Trabalho";
//                 } else {
//                   document.getElementById("status").innerHTML = "Status: Pausa";
//                 }
//                 if (data.fim) {
//                   alert("Ciclo finalizado!");
//                 }
//               })
//               .catch(err => console.error(err));
//           }, 2000);
//         </script>
//       </body>
//     </html>
//     )rawliteral";
// }

// PomodoroServer::PomodoroServer(const char* ssid, const char* password) {
//   this->ssid = ssid;
//   this->password = password;
//   server = new AsyncWebServer(80);
//   static const char index_html[] PROGMEM = R"rawliteral(
//     <!DOCTYPE html>
//     <html>
//       <head>
//         <title>Pomodoro com ESP32</title>
//       </head>
//       <body>
//         <h1>Pomodoro em andamento...</h1>
//         <p id="status">Status: Carregando...</p>
    
//         <script>
//           setInterval(() => {
//             fetch("/status")
//               .then(res => res.json())
//               .then(data => {
//                 if (data.emTrabalho) {
//                   document.getElementById("status").innerHTML = "Status: Trabalho";
//                 } else {
//                   document.getElementById("status").innerHTML = "Status: Pausa";
//                 }
//                 if (data.fim) {
//                   alert("Ciclo finalizado!");
//                 }
//               })
//               .catch(err => console.error(err));
//           }, 2000);
//         </script>
//       </body>
//     </html>
//     )rawliteral";
// }

// PomodoroServer::~PomodoroServer() {
//   delete server;
// }

// void PomodoroServer::setSsid(const char* ssid) {
//   this->ssid = ssid;
// }
// const char* PomodoroServer::getSsid() {
//   return ssid;
// }
// void PomodoroServer::setPassword(const char* password) {
//   this->password = password;
// }
// const char* PomodoroServer::getPassword() {
//   return password;
// }
// void PomodoroServer::setup(){
//     WiFi.begin(ssid, password);
//     while (WiFi.status() != WL_CONNECTED) {
//       delay(500);
//       Serial.print(".");
//     }
//     Serial.println("\nConectado ao WiFi");
// }

// void PomodoroServer::rotaPrincipal() {
//     server->on("/", HTTP_GET, [](AsyncWebServerRequest *req){
//         req->send_P(200, "text/html", index_html);
//     });
// }

// void PomodoroServer::rotaStatus(Pomodoro &pomodoro) {
//     server->on("/status", HTTP_GET, [&pomodoro](AsyncWebServerRequest *req){
//         String json = String("{\"fim\":") + (pomodoro.getCicloFinalizado() ? "true" : "false")
//                     + String(",\"emTrabalho\":") + (pomodoro.getEmTrabalho() ? "true" : "false")
//                     + String("}");
//         req->send(200, "application/json", json);
//         pomodoro.setCicloFinalizado(false);
//     });
// }

// void PomodoroServer::serverBegin() {
//     server->begin();
//     Serial.println("Servidor iniciado");
// }
