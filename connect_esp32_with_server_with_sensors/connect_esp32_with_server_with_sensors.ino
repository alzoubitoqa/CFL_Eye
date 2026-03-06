#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// ===== WiFi (Hotspot) =====
const char* ssid = "DESKTOP-QM4P8AF 3638";
const char* password = "55^87Bk4";

// ===== Server =====
const char* SERVER_URL = "http://192.168.137.1:3000/api/ingest";

// ===== Pins (حسب توصيلاتك) =====
#define DHTPIN 14
#define DHTTYPE DHT11

#define MQ_PIN 34       // ✅ مهم: AO -> GPIO34 (ADC1)
#define IR_PIN 27       // DO -> GPIO12
#define FLAME_PIN 26    // DO -> GPIO32

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);

  pinMode(IR_PIN, INPUT);
  pinMode(FLAME_PIN, INPUT);

  dht.begin();

  // ADC setup
  analogReadResolution(12); // 0..4095

  // WiFi
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Hotspot");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    delay(300);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✅ WiFi connected");
    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("❌ WiFi failed");
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ WiFi disconnected");
    delay(2000);
    return;
  }

  // ===== Read sensors =====
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // DHT sometimes returns NaN
  if (isnan(h) || isnan(t)) {
    Serial.println("⚠️ DHT read failed");
    // نخلي آخر قراءة أو صفر، بس ما نوقف النظام
    h = 0;
    t = 0;
  }

  int gas = analogRead(MQ_PIN);        // 0..4095
  int ir = digitalRead(IR_PIN);        // 0/1
  int flameDO = digitalRead(FLAME_PIN);// 0/1 (غالبًا 0 = detected)

  // حسب كود السيرفر عندك: fire=1 safe, fire=0 fire
  int fire = flameDO; // إذا عندك العكس قوليلي وبنعكسها

  // Door/Locker حالياً خليهم ثابتين (إلا إذا عندك حساسات لهم)
  const char* door = "Closed";
  const char* locker = "Stored";

  // ===== Build JSON =====
  String payload = "{";
  payload += "\"gas\":" + String(gas) + ",";
  payload += "\"humidity\":" + String(h, 1) + ",";
  payload += "\"temperature\":" + String(t, 1) + ",";
  payload += "\"fire\":" + String(fire) + ",";
  payload += "\"door\":\"" + String(door) + "\",";
  payload += "\"locker\":\"" + String(locker) + "\",";
  payload += "\"ir\":" + String(ir);
  payload += "}";

  Serial.println("📤 Sending: " + payload);

  // ===== POST =====
  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "application/json");

  int code = http.POST(payload);
  Serial.print("POST code: ");
  Serial.println(code);

  if (code > 0) {
    Serial.println("Server: " + http.getString());
  }

  http.end();

  delay(3000); // كل 3 ثواني
}