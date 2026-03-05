#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

// ========= WiFi =========
const char* ssid     = "";
const char* password = "";

// ✅ IP اللابتوب اللي عليه السيرفر
const char* SERVER_URL = "";

// ========= Pins (حسب توصيلك) =========
#define FLAME_DO   26
#define IR_DO      27
#define MQ_AO      34     // ✅ لازم ADC1 عشان يشتغل مع WiFi
#define DHT_PIN    14
#define DHT_TYPE   DHT11

#define BUZZER     25     // Passive buzzer
#define LED_FIRE   32
#define LED_GAS    13
#define LED_DHT    33
#define LED_IR     12

DHT dht(DHT_PIN, DHT_TYPE);

// ========= Settings =========
int MQ_THRESHOLD = 800;
float TEMP_HIGH = 40.0;
float HUM_HIGH  = 80.0;

// -------- Passive buzzer helper (بدون LEDC) --------
void beep(int freq, int ms) {
  tone(BUZZER, freq, ms);
  delay(ms);
  noTone(BUZZER);
}

// (اختياري) قلب إشارة IR لو كان عندك بالعكس
bool IR_ACTIVE_HIGH = true;

void setup() {
  Serial.begin(115200);

  pinMode(FLAME_DO, INPUT);
  pinMode(IR_DO, INPUT);

  pinMode(BUZZER, OUTPUT);

  pinMode(LED_FIRE, OUTPUT);
  pinMode(LED_GAS, OUTPUT);
  pinMode(LED_DHT, OUTPUT);
  pinMode(LED_IR, OUTPUT);

  digitalWrite(LED_FIRE, LOW);
  digitalWrite(LED_GAS, LOW);
  digitalWrite(LED_DHT, LOW);
  digitalWrite(LED_IR, LOW);

  dht.begin();

  // WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  delay(300);

  Serial.print("Connecting WiFi");
  WiFi.begin(ssid, password);

  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 40) {
    delay(500);
    Serial.print(".");
    tries++;
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
  // ====== Read sensors ======
  int flame = digitalRead(FLAME_DO); // 0 = نار غالبًا
  int irRaw = digitalRead(IR_DO);
  int gas   = analogRead(MQ_AO);

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  bool dhtOk = !(isnan(h) || isnan(t));

  // ====== Determine statuses ======
  String st_fire = (flame == 0) ? "Alert" : "Normal";
  String st_gas  = (gas >= MQ_THRESHOLD) ? "Warning" : "Normal";

  int ir = IR_ACTIVE_HIGH ? irRaw : (irRaw == 1 ? 0 : 1);
  String st_ir = (ir == 1) ? "Alert" : "Normal";

  String st_dht = "Normal";
  if (!dhtOk) st_dht = "Error";
  else if (t >= TEMP_HIGH || h >= HUM_HIGH) st_dht = "Warning";

  // ====== LEDs ======
  digitalWrite(LED_FIRE, (st_fire != "Normal"));
  digitalWrite(LED_GAS,  (st_gas  != "Normal"));
  digitalWrite(LED_IR,   (st_ir   != "Normal"));
  digitalWrite(LED_DHT,  (st_dht  != "Normal"));

  // ====== Buzzer ======
  bool anyDanger =
    (st_fire == "Alert") ||
    (st_ir == "Alert")   ||
    (st_gas == "Warning")||
    (st_dht == "Warning");

  if (anyDanger) {
    beep(2000, 120);
    delay(80);
    beep(2000, 120);
  }

  // ====== Print ======
  Serial.println("---- Sensors ----");
  Serial.printf("Flame=%d (%s)\n", flame, st_fire.c_str());
  Serial.printf("IR=%d (%s)\n", ir, st_ir.c_str());
  Serial.printf("Gas=%d (%s)\n", gas, st_gas.c_str());
  if (dhtOk) {
    Serial.printf("Temp=%.1fC Hum=%.1f%% (%s)\n", t, h, st_dht.c_str());
  } else {
    Serial.println("DHT: ERROR");
  }

  // ====== Send to server ======
  if (WiFi.status() == WL_CONNECTED) {
    // JSON payload
    String payload = "{";
    payload += "\"fire\":" + String(flame) + ",";
    payload += "\"gas\":" + String(gas) + ",";
    payload += "\"humidity\":" + String(dhtOk ? h : -1) + ",";
    payload += "\"temperature\":" + String(dhtOk ? t : -1) + ",";
    payload += "\"ir\":" + String(ir);
    payload += "}";

    WiFiClient client;
    HTTPClient http;

    http.setTimeout(6000);
    http.setReuse(false);

    Serial.print("Sending: ");
    Serial.println(payload);

    if (!http.begin(client, SERVER_URL)) {
      Serial.println("❌ http.begin failed");
      delay(2000);
      return;
    }

    http.addHeader("Content-Type", "application/json");
    int code = http.POST(payload);

    Serial.print("POST -> ");
    Serial.println(code);

    if (code > 0) {
      String resp = http.getString();
      Serial.print("Response: ");
      Serial.println(resp);
    } else {
      Serial.printf("HTTP error: %s\n", http.errorToString(code).c_str());
    }

    http.end();
  } else {
    Serial.println("WiFi not connected, skip send.");
  }

  delay(2000);
}
