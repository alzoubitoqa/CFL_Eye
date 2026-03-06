#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "DESKTOP-QM4P8AF 3638";
const char* password = "55^87Bk4";

const char* URL = "http://192.168.137.1:3000/ping";

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Hotspot");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi connected");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  HTTPClient http;
  http.begin(URL);
  int code = http.GET();
  Serial.print("GET code: ");
  Serial.println(code);

  if (code > 0) {
    Serial.println("Response: " + http.getString());  // لازم تكون pong
  }
  http.end();
}

void loop() {}