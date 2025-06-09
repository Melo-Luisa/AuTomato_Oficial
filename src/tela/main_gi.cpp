// #include <WiFi.h>
// #include <ESPAsyncWebServer.h>
// #include <DNSServer.h>
// #include <SPIFFS.h>
// #include <Preferences.h>
// #include <ArduinoJson.h> // Usado para parsear JSON recebido do frontend

// // Instâncias de objetos globais
// Preferences preferences;           // Para armazenar SSID e senha de Wi-Fi de forma persistente
// AsyncWebServer server(80);         // Servidor web assíncrono na porta 80
// DNSServer dnsServer;               // Servidor DNS para redirecionar qualquer domínio ao ESP32 no modoAP
//   // Problema DNS: mais funcional para IOS e LINUX
// bool isConfigured = false;         // Flag: indica se já existe configuração de Wi-Fi salva
// const byte DNS_PORT = 53;          // Porta padrão do DNS

// // Função para configurar rotas de portal cativo
// // Todos esses acessos redirecionam para "/"
// void setupCaptivePortalRoutes() {
//   const char * captiveRoutes[] = {
//     "/generate_204", "/hotspot-detect.html", "/ncsi.txt",
//     "/redirect", "/connecttest.txt", "/fwlink"
//   };
//   for (auto route : captiveRoutes) {
//     server.on(route, HTTP_GET, [](AsyncWebServerRequest *request){
//       request->redirect("/");
//     });
//   }
// }

// // Função para iniciar o ESP32 como Access Point (AP), criando a rede "AuTomato"
// void startAccessPoint() {
//   WiFi.softAP("AuTomato", "estudante");                    // Cria rede Wi-Fi com nome e senha fixos
//   dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());         // Redireciona todo DNS para o próprio ESP32

//   // Rota principal: serve a página de configuração de Wi-Fi
//   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
//     request->send(SPIFFS, "/index.html", "text/html"); // Problems on SPIFFS still on;
//   });
//   // JS da página de configuração
//   server.on("/js/wifi.js", HTTP_GET, [](AsyncWebServerRequest *request){
//     request->send(SPIFFS, "/js/wifi.js", "text/javascript");
//   });

//   // Endpoint: recebe POST em /connect com JSON contendo ssid/senha (JSON por ser conectado com JS)
//   server.on("/connect", HTTP_POST, [](AsyncWebServerRequest *request){
//     String body;
//     if (request->hasParam("plain", true)) {
//       body = request->getParam("plain", true)->value();
//     }
//     // Faz o parse do JSON recebido
//     DynamicJsonDocument doc(256);
//     DeserializationError error = deserializeJson(doc, body);
//     if (!error && doc.containsKey("ssid") && doc.containsKey("password")) {
//       String ssid = doc["ssid"].as<String>();
//       String password = doc["password"].as<String>();

//       // Salva as credenciais em memória flash (even without energy)
//       preferences.begin("wifi", false);
//       preferences.putString("ssid", ssid);
//       preferences.putString("password", password);
//       preferences.end();

//       // Responde ao frontend confirmando sucesso
//       AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{\"success\":true}");
//       response->addHeader("Access-Control-Allow-Origin", "*");
//       request->send(response);

//       delay(1000); // Garante envio da resposta antes de reiniciar
//       ESP.restart(); // Reinicia para conectar à rede informada
//     } else {
//       AsyncWebServerResponse *response = request->beginResponse(400, "application/json", "{\"success\":false}");
//       response->addHeader("Access-Control-Allow-Origin", "*");
//       request->send(response);
//     }
//   });

//   // Endpoint legado: aceita POST de formulário para /save-wifi
//   server.on("/save-wifi", HTTP_POST, [](AsyncWebServerRequest *request){
//     if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
//       String ssid = request->getParam("ssid", true)->value();
//       String password = request->getParam("password", true)->value();

//       preferences.begin("wifi", false);
//       preferences.putString("ssid", ssid);
//       preferences.putString("password", password);
//       preferences.end();

//       request->send(200, "text/plain", "Salvo");
//       delay(1000);
//       ESP.restart();
//     } else {
//       request->send(400, "text/plain", "Erro");
//     }
//   });

//   // Arquivos estáticos (css, js, html) do SPIFFS, padrão = index.html
//   server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

//   // Qualquer rota não encontrada redireciona para index.html (útil para SPA)
//   server.onNotFound([](AsyncWebServerRequest *request){
//     request->send(SPIFFS, "/index.html", "text/html");
//   });

//   // Configura as rotas do portal cativo
//   setupCaptivePortalRoutes();

//   server.begin();
// }

// // Função para conectar à rede Wi-Fi previamente salva e servir o site principal
// void startStationMode() {
//   preferences.begin("wifi", true);
//   String ssid = preferences.getString("ssid", "");
//   String password = preferences.getString("password", "");
//   preferences.end();

//   WiFi.begin(ssid.c_str(), password.c_str());

//   Serial.print("Conectando a ");
//   Serial.println(ssid);

//   unsigned long startAttemptTime = millis();
//   // Tenta conectar por até 15 segundos
//   while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
//     delay(500);
//     Serial.print(".");
//   }

//   if (WiFi.status() == WL_CONNECTED) {
//     Serial.println("\nConectado com sucesso!");
//     // Serve o site principal (ex: pre.html) para quem acessa o ESP32 pela rede
//     server.serveStatic("/", SPIFFS, "/").setDefaultFile("pre.html");
//     server.onNotFound([](AsyncWebServerRequest *request){
//       request->send(SPIFFS, "/pre.html", "text/html");
//     });
//     server.begin();
//   } else {
//     Serial.println("Falha na conexão. Voltando para AP.");
//     // Se não conseguiu conectar, volta para modo AP para nova configuração
//     startAccessPoint();
//   }
// }

// void setup() {
//   Serial.begin(115200);
//   if (!SPIFFS.begin(true)) {
//     Serial.println("Erro ao montar SPIFFS");
//     return;
//   }

//   // Verifica se já existe configuração Wi-Fi salva
//   preferences.begin("wifi", true);
//   isConfigured = preferences.isKey("ssid") && preferences.isKey("password");
//   preferences.end();

//   // decide: vai para modo estação (Wi-Fi da casa) ou AP (portais cativos geralmente são usados por redes de Wi-Fi públicas para gerenciar o acesso e a autenticação do usuário)
//   if (isConfigured) {
//     startStationMode();
//   } else {
//     startAccessPoint();
//   }
// }

// void loop() {
//   // Se estiver em modo AP, processa as requisições DNS para garantir o redirecionamento do portal cativo
//   if (!isConfigured) {
//     dnsServer.processNextRequest();
//   }
// }