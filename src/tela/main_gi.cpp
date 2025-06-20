// // Projeto: Pomodoro com ESP32
// #include <TFT_eSPI.h>
// #include <SPI.h>
// #include <Arduino.h>
// #include <WiFi.h>
// #include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>
// #include <ESP32Servo.h>
// #include <FS.h>
// #include <SPIFFS.h>
// #include <DNSServer.h>
// #include <Preferences.h>
// #include <ArduinoJson.h> // Restaurado

// #define BUZZER_PIN 13
// #define BUTTON_PIN 36
// #define BUTTON_EMERGENCY 36
// #define BUZZER_CHANNEL 6
// static const int servoPin = 5;

// Servo servo1;
// Preferences preferences; // Para armazenar SSID e senha de Wi-Fi de forma persistente
// DNSServer dnsServer;  

// AsyncWebServer server(80);
// // const char* ssid = "Redmi Note 14";
// // const char* password = "giugiu24";
// TFT_eSPI tft = TFT_eSPI();

// uint32_t lastSecond = 0;
// int duracaoFoco = 25;
// int duracaoPausa = 5;
// int tempoRestante = duracaoFoco;
// int num_ciclos = 1;
// int contEmergency = 0;

// bool isConfigured = false;         // Flag: indica se já existe configuração de Wi-Fi salva
// const byte DNS_PORT = 53; 

// bool emTrabalho = true;
// bool somTocado = false;
// bool cicloFinalizado = false;
// bool pomodoroIniciado = false;
// bool esperandoResposta = false;
// bool iniciarPomodoro_aux = false;
// bool girou = false;

// static bool lastEmergencyState = HIGH;
// unsigned long lastEmergencyClick = 0;

// String perguntaAtual = "";

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
//   WiFi.softAP("AuTomato", "estudante",6);                    // Cria rede Wi-Fi com nome e senha fixos
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
//   server.serveStatic("/", SPIFFS, "/").setDefaultFile("/index.html");

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
//     server.serveStatic("/", SPIFFS, "/").setDefaultFile("/index.html");
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

// void salvarResposta(const String& pergunta, const String& resposta) {
//   JsonDocument doc;
//   doc["pergunta"] = pergunta;
//   doc["resposta"] = resposta;
//   fs::File file = SPIFFS.open("/respostas.json", FILE_APPEND); // Restaurado para .json
//   if (file) {
//     serializeJson(doc, file); // Lógica JSON restaurada
//     file.println();
//     file.close();
//     Serial.println("Resposta salva em respostas.json"); // Mensagem atualizada
//   } else {
//     Serial.println("Erro ao abrir o arquivo respostas.json para escrita");
//   }
// }

// void playTone(int freq, int dur) {
//   ledcSetup(BUZZER_CHANNEL, freq, 8);
//   ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
//   ledcWrite(BUZZER_CHANNEL, 128);
//   delay(dur);
//   ledcWrite(BUZZER_CHANNEL, 0);
// }

// void playWorkEndTone() {
//   playTone(1000, 200); delay(100);
//   playTone(1200, 200); delay(100);
//   playTone(1500, 300);
// }

// void playBreakEndTone() {
//   playTone(500, 500);
// }

// void playEmergencyTone() {
//   playTone(2000, 500); delay(100);
//   playTone(2000, 500); delay(100);
//   playTone(2000, 500);
// }

// void mostrarPerguntaTFT(const String& pergunta) {
//   tft.fillScreen(TFT_WHITE);
//   tft.setTextColor(TFT_YELLOW, TFT_WHITE);
//   tft.setTextSize(2);
//   int16_t x = (tft.width() - tft.textWidth((char*)pergunta.c_str())) / 2;
//   tft.setCursor(x, 50);
//   tft.println(pergunta);
// }

// void atualizarTela() {
//   if (esperandoResposta) return;
//   uint16_t bgColor = emTrabalho ? TFT_GREEN : TFT_SKYBLUE;
//   tft.fillScreen(TFT_WHITE);
//   tft.setTextSize(2);

//   if (!pomodoroIniciado) {
//     tft.setTextColor(TFT_RED, TFT_WHITE);
//     int16_t x1 = (tft.width() - tft.textWidth("Pressione o botao")) / 2;
//     int16_t x2 = (tft.width() - tft.textWidth("para iniciar!")) / 2;
//     tft.setCursor(x1, 50); tft.print("Pressione o botao");
//     tft.setCursor(x2, 80); tft.print("para iniciar!");
//   } else {
//     tft.setTextColor(bgColor, TFT_WHITE);
//     String status = emTrabalho ? "Trabalhando..." : "Pausa...";
//     int16_t xStatus = (tft.width() - tft.textWidth((char*)status.c_str())) / 2;
//     tft.setCursor(xStatus, 50);
//     tft.print(status);

//     int minutos = tempoRestante / 60;
//     int segundos = tempoRestante % 60;
//     tft.setTextSize(6);
//     char tempoStr[6];
//     sprintf(tempoStr, "%02d:%02d", minutos, segundos);
//     int16_t xTempo = (tft.width() - tft.textWidth(tempoStr)) / 2;
//     tft.setCursor(xTempo, 100);
//     tft.print(tempoStr);
//   }
// }

// void girarServoInicio() {
//   for (int pos = 0; pos <= 180; pos++) {
//     servo1.write(pos);
//     Serial.println(pos);
//     delay(15);
//   }
// }

// void girarServoFim() {
//   for (int pos = 180; pos >= 0; pos--) {
//     servo1.write(pos);
//     Serial.println(pos);
//     delay(15);
//   }
// }

// void emergencyStop() {
//   Serial.println("Parando o pomodoro de emergencia!");
//   pomodoroIniciado = false;
//   emTrabalho = true;
//   tempoRestante = duracaoFoco;
//   somTocado = false;
//   cicloFinalizado = false;
//   esperandoResposta = false;
//   girarServoFim();
//   atualizarTela();
// }

// void screenEmergency() {
//   tft.fillScreen(TFT_RED);
//   tft.setTextColor(TFT_WHITE, TFT_RED);
//   tft.setTextSize(4);
//   tft.setCursor(20, 100);
//   tft.print("BOTAO DE EMERGENCIA ACIONADO!");
//   playEmergencyTone();
//   delay(200);
//   emergencyStop();
// }

// void pomodoroBotaoIniciar() {
//   if (!pomodoroIniciado && digitalRead(BUTTON_PIN) == LOW) {
//     delay(200);
//     perguntaAtual = "Como voce se sente para estudar?";
//     esperandoResposta = true;
//     iniciarPomodoro_aux = true;
//   }
// }

// void pomodoroIniciar() {
//   if (!pomodoroIniciado && iniciarPomodoro_aux) {
    
//     playWorkEndTone();
//     tft.fillScreen(TFT_WHITE);
//     tft.setTextColor(TFT_BLACK, TFT_WHITE);
//     tft.setTextSize(2);
//     tft.setCursor(10, 10);
//     tft.print("Pomodoro configuravel");
//     pomodoroIniciado = true;
//     iniciarPomodoro_aux = false;
//     Serial.println("Entrou em pomodoroIniciar");
//     girarServoInicio();
//     lastSecond = millis();
//   }
// }

// void pomodoroLogica() {
//   if (pomodoroIniciado && millis() - lastSecond >= 1000 && num_ciclos > 0) {
//     lastSecond += 1000;
//     tempoRestante--;

//     if (tempoRestante >= 0) atualizarTela();

//     if (tempoRestante < 0 && !somTocado) {
//       somTocado = true;
//       if (emTrabalho) {
//         playWorkEndTone();
//         emTrabalho = false;
//         tempoRestante = duracaoPausa;
//       } else {
//         playBreakEndTone();
//         cicloFinalizado = true;
//         if (num_ciclos > 1) {
//           emTrabalho = true;
//           tempoRestante = duracaoFoco;
//         }
//         num_ciclos--;
//       }
//       atualizarTela();
//     }

//     if (somTocado && tempoRestante >= 0)
//       somTocado = false;
//   }
// }

// void pomodoroFinalizar() {
//   if (num_ciclos <= 0 && cicloFinalizado) {
//     if (somTocado) {
//       somTocado = false;
//       cicloFinalizado = true;
//       playWorkEndTone();
//       perguntaAtual = "Como voce se sente apos estudar?";
//       esperandoResposta = true;
//     }

//     if (!esperandoResposta) {
//       cicloFinalizado = false;
//       tft.fillScreen(TFT_WHITE);
//       tft.setTextColor(TFT_YELLOW, TFT_WHITE);
//       tft.setTextSize(6);
//       int16_t xFim = (tft.width() - tft.textWidth("FIM!")) / 2;
//       tft.setCursor(xFim, 10);
//       tft.print("FIM!");
//       girarServoFim();
//     }
//   }
// }

// void pomodoroEmergencia() {
//   bool currentState = digitalRead(BUTTON_EMERGENCY);
//   unsigned long now = millis();

//   if (lastEmergencyState == LOW && currentState == HIGH) {
//     if (now - lastEmergencyClick > 5000) {
//       contEmergency = 0; // Reset se passou mais de 5 segundos entre cliques
//     }

//     if (now - lastEmergencyClick > 300) { // Debounce de 300ms
//       contEmergency++;
//       Serial.print("Emergencia cliques: "); Serial.println(contEmergency);
//       lastEmergencyClick = now;

//       if (contEmergency >= 10) {
//         screenEmergency();
//         contEmergency = 0;
//       }
//     }
//   }

//   lastEmergencyState = currentState;
// }

// void setup() {
//   Serial.begin(115200);
//   pinMode(BUZZER_PIN, OUTPUT);
//   pinMode(BUTTON_PIN, INPUT_PULLUP);
//   pinMode(BUTTON_EMERGENCY, INPUT_PULLUP);
//   servo1.attach(servoPin);

//   if (!SPIFFS.begin(true)) {
//     Serial.println("Erro ao montar SPIFFS");
//     return;
//   }
//   Serial.println(WiFi.softAPIP());          // Mostra o IP local do AP (geralmente 192.168.4.1)
//   Serial.println(WiFi.softAPmacAddress());
//   Serial.println("Modo Access Point iniciado. Conecte-se à rede AuTomato.");
//   Serial.println("Acesse http://" + WiFi.softAPIP().toString() + " para configurar o Wi-Fi.");

//   tft.init();
//   tft.setRotation(1);
//   tft.fillScreen(TFT_WHITE);
//   tft.setTextColor(TFT_BLACK, TFT_WHITE);
//   tft.setTextSize(2);
//   tft.setCursor(10, 10);
//   tft.print("Pomodoro configuravel");
//   delay(1000);
//   atualizarTela();



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

//   Serial.println(WiFi.localIP());

//   String ipStr = "IP: " + WiFi.localIP().toString();
//   tft.setTextSize(2);
//   tft.setTextColor(TFT_RED, TFT_WHITE);
//   tft.setCursor(240 - tft.textWidth((char*)ipStr.c_str()) - 5, 230);
//   tft.print(ipStr);

//   server.on("/", HTTP_GET, [](AsyncWebServerRequest *req){
//     req->send(SPIFFS, "/index.html", "text/html");
//   });

//   server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *req){
//     req->send(SPIFFS, "/style.css", "text/css");
//   });

//   server.on("/status", HTTP_GET, [](AsyncWebServerRequest *req){
//     String json = String("{\"fim\":") + (cicloFinalizado ? "true" : "false") +
//                   String(",\"emTrabalho\":") + (emTrabalho ? "true" : "false") +
//                   String(",\"iniciado\":") + (pomodoroIniciado ? "true" : "false") +
//                   String(",\"pergunta\":\"") + (esperandoResposta ? perguntaAtual : "") + String("\"}");
//     req->send(200, "application/json", json);
//   });

//   server.on("/config", HTTP_POST, [](AsyncWebServerRequest *req){
//     if (req->hasParam("foco", true) && req->hasParam("pausa", true)) {
//       duracaoFoco = req->getParam("foco", true)->value().toInt();
//       duracaoPausa = req->getParam("pausa", true)->value().toInt();
//       tempoRestante = emTrabalho ? duracaoFoco : duracaoPausa;
//       req->send(200, "text/plain", "Ciclos atualizados");
//     } else req->send(400, "text/plain", "Parâmetros inválidos");
//   });

//   server.on("/resposta", HTTP_POST, [](AsyncWebServerRequest *req){
//     if (req->hasParam("resposta", true)) {
//       String resposta = req->getParam("resposta", true)->value();
//       salvarResposta(perguntaAtual, resposta);
//       esperandoResposta = false;
//       req->send(200, "text/plain", "Recebido");
//     }
//   });

//   // Nova rota para visualizar o arquivo respostas.json
//   server.on("/getrespostas", HTTP_GET, [](AsyncWebServerRequest *req){
//     fs::File file = SPIFFS.open("/respostas.json", FILE_READ);
//     if (!file || file.isDirectory()) {
//       Serial.println("Falha ao abrir respostas.json para leitura");
//       req->send(404, "text/plain", "Arquivo nao encontrado");
//       return;
//     }
//     String fileContent = "";
//     while(file.available()){
//       fileContent += (char)file.read();
//     }
//     file.close();
//     req->send(200, "application/json", fileContent);
//   });



// }

// void loop() {
//     if (!isConfigured) {
//         dnsServer.processNextRequest();
//     }
//   pomodoroBotaoIniciar();
//   if (esperandoResposta) return;
//   pomodoroIniciar();
//   pomodoroLogica();
//   pomodoroFinalizar();
//   pomodoroEmergencia();
  
// }