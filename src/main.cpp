#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char ssid[] = "shelly1-12BA14";
const char password[] = "shellyAP";
const char schalter_on[] = "http://192.168.33.1/relay/0?turn=on";
const char schalter_off[] = "http://192.168.33.1/relay/0?turn=off";

// put function declarations here:
void begin_wlan_client();
String http_Post_Request(const char* url);
void heizung_ein();
void heizung_aus();

void setup() {
  begin_wlan_client(); // WLAN-Verbindung starten
}

void loop() {
  // Beispiel für das Einschalten und Ausschalten der Heizung
  heizung_ein();
  delay(10000); // Warte 5 Sekunden
  heizung_aus();
  delay(10000); // Warte 5 Sekunden
}

// put function definitions here:
void begin_wlan_client() {
  WiFi.begin(ssid, password);
  Serial.begin(115200); // Zum Debuggen in der Konsole

  Serial.println("Verbindung zum WLAN herstellen...");

  // Wartet, bis der ESP32 mit dem Shelly-WLAN verbunden ist
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Verbunden mit IP: ");
  Serial.println(WiFi.localIP());
}

String http_Post_Request(const char* url) {
  HTTPClient http;
  http.begin(url); // Starte die HTTP-Verbindung

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
