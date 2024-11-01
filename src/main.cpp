// Erforderliche Bibliotheken einbinden
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>

const char ssid[] = "xxxxx"; //dein WLAN Name
const char password[] = "xxxxxxxxxxxxx"; // dein WLAN Passwort
const char shelly_ip[] = "192.xxx.x.xxx"; // Beispiel IP-Adresse des Shelly im Heimnetzwerk

String schalter_on = "/relay/0?turn=on";
String schalter_off = "/relay/0?turn=off";

WebServer server(80);

// put function declarations here:
void begin_wlan_client();
String http_Post_Request(const String& url);
void heizung_ein();
void heizung_aus();
void handle_root();
void handle_heizung_ein();
void handle_heizung_aus();

void setup() {
  begin_wlan_client(); // WLAN-Verbindung starten

  // Webserver-Endpunkte definieren
  server.on("/", handle_root);
  server.on("/ein", handle_heizung_ein);
  server.on("/aus", handle_heizung_aus);
  server.begin();
  Serial.println("Webserver gestartet.");
}

void loop() {
  server.handleClient();
}

// put function definitions here:
void begin_wlan_client() {
  WiFi.begin(ssid, password);
  Serial.begin(115200); // Zum Debuggen in der Konsole

  Serial.println("Verbindung zum WLAN herstellen...");

  // Wartet, bis der ESP32 mit dem Heim-WLAN verbunden ist
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Verbunden mit IP: ");
  Serial.println(WiFi.localIP());
}

String http_Post_Request(const String& url) {
  HTTPClient http;
  String fullUrl = "http://" + String(shelly_ip) + url;
  http.begin(fullUrl); // Starte die HTTP-Verbindung

  int httpResponseCode = http.GET(); // Sende HTTP GET-Anfrage

  String payload = ""; // Leeres Payload für die Antwort
  if (httpResponseCode > 0) {
    payload = http.getString(); // Erhalte Antwort als String
    Serial.print("Antwort: ");
    Serial.println(payload);
  } else {
    Serial.print("Fehler bei der Anfrage, Fehlercode: ");
    Serial.println(httpResponseCode);
  }
  http.end(); // Beende die HTTP-Verbindung
  return payload;
}

void heizung_ein() { // Funktion zum Einschalten der Heizung
  Serial.println("Schalte Heizung ein...");
  String payload = http_Post_Request(schalter_on);

  int pos = payload.indexOf(':'); // Erstes ':' suchen, z.B. {"ison":true,
  int pos2 = payload.indexOf(',');

  String result = payload.substring(pos + 1, pos2);
  Serial.print("Antwort vom Shelly: ");
  Serial.println(result);

  if (result == "true") {
    Serial.println("Heizung erfolgreich eingeschaltet!");
    Serial.println();
  } else {
    Serial.println("Fehler beim Einschalten der Heizung.");
    Serial.println();
  }
}

void heizung_aus() { // Funktion zum Ausschalten der Heizung
  Serial.println("Schalte Heizung aus...");
  String payload = http_Post_Request(schalter_off);

  int pos = payload.indexOf(':');
  int pos2 = payload.indexOf(',');

  String result = payload.substring(pos + 1, pos2);
  Serial.print("Antwort vom Shelly: ");
  Serial.println(result);

  if (result == "false") {
    Serial.println("Heizung erfolgreich ausgeschaltet!");
    Serial.println();
  } else {
    Serial.println("Fehler beim Ausschalten der Heizung.");
    Serial.println();
  }
}

void handle_root() {
  String html = "<html><body style='text-align: center; display: flex; flex-direction: column; justify-content: center; height: 100vh;'>";
  html += "<div style='margin-top: 10%;'>";
  html += "<h1 style='text-align: center;'>ESP schaltet Shelly</h1>";
  html += "<button style='font-size: 24px; width: 200px; height: 100px; background-color: green; color: white; margin: 20px;' onclick=\"location.href='/ein'\">Ein</button>";
  html += "<button style='font-size: 24px; width: 200px; height: 100px; background-color: red; color: white; margin: 20px;' onclick=\"location.href='/aus'\">Aus</button>";
  html += "<p id='status' style='font-size: 24px; margin-top: 40px;'></p>";
  html += "</div>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handle_heizung_ein() {
  heizung_ein();
  server.send(200, "text/html", "<html><body style='text-align: center; display: flex; flex-direction: column; justify-content: center; height: 100vh;'><div style='margin-top: 10%;'><h1>ESP schaltet Shelly</h1><p style='font-size: 24px; margin-top: 40px;'>Shelly Relais eingeschaltet</p><a href='/'>Zur&uuml;ck</a></div></body></html>");
}

void handle_heizung_aus() {
  heizung_aus();
  server.send(200, "text/html", "<html><body style='text-align: center; display: flex; flex-direction: column; justify-content: center; height: 100vh;'><div style='margin-top: 10%;'><h1>ESP schaltet Shelly</h1><p style='font-size: 24px; margin-top: 40px;'>Shelly Relais ist ausgeschaltet</p><a href='/'>Zur&uuml;ck</a></div></body></html>");
}