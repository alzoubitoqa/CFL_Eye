#include <DHT.h>

// ---------- Pins ----------
#define DHTPIN     14      // P14
#define DHTTYPE    DHT11
#define MQ2_PIN    34      // P34 (AO)
#define IR_DO      27      // P27 (DO)
#define FLAME_DO   26      // P26 (DO)

// ---------- Objects ----------
DHT dht(DHTPIN, DHTTYPE);

// ---------- Thresholds (عدليهم بعد التجربة) ----------
int MQ2_THRESHOLD = 800;   // إذا قراءاتك بالهواء كانت 250-400 فـ 800 مناسب كبداية

// ---------- Helpers ----------
unsigned long lastDHT = 0;
unsigned long lastPrint = 0;

void setup() {
  Serial.begin(115200);
  delay(800);

  pinMode(IR_DO, INPUT);
  pinMode(FLAME_DO, INPUT);

  dht.begin();

  Serial.println("=== Digital Forensic Lab - All Sensors Online ===");
}

void loop() {
  // ---- Read fast digital sensors ----
  int ir = digitalRead(IR_DO);          // غالبًا: 0 = جسم موجود, 1 = لا يوجد
  int flame = digitalRead(FLAME_DO);    // غالبًا: 0 = لهب, 1 = آمن

  // ---- Read MQ2 analog ----
  int mq2 = analogRead(MQ2_PIN);

  // ---- Read DHT every 2 seconds (لأنه بطيء شوي) ----
  static float t = NAN, h = NAN;
  if (millis() - lastDHT >= 2000) {
    lastDHT = millis();
    h = dht.readHumidity();
    t = dht.readTemperature();
  }

  // ---- Build status flags ----
  bool intrusion = (ir == 0);
  bool fireAlert = (flame == 0);
  bool gasAlert  = (mq2 >= MQ2_THRESHOLD);

  // ---- Print dashboard every 1 second ----
  if (millis() - lastPrint >= 1000) {
    lastPrint = millis();

    Serial.println("--------------------------------------------------");

    // DHT
    if (isnan(t) || isnan(h)) {
      Serial.println("DHT11: ERROR (check wiring)");
    } else {
      Serial.print("DHT11: Temp=");
      Serial.print(t);
      Serial.print(" C | Humidity=");
      Serial.print(h);
      Serial.println(" %");
    }

    // MQ2
    Serial.print("MQ-2:  GasLevel=");
    Serial.print(mq2);
    Serial.print(" | Threshold=");
    Serial.println(MQ2_THRESHOLD);

    // IR
    Serial.print("IR:    ");
    Serial.println(intrusion ? "⚠ INTRUSION DETECTED" : "Area Clear");

    // Flame
    Serial.print("Flame: ");
    Serial.println(fireAlert ? "🔥 FIRE DETECTED" : "Safe");

    // Summary
    Serial.print("SYSTEM STATUS: ");
    if (fireAlert || gasAlert || intrusion) {
      Serial.print("ALERT -> ");
      if (fireAlert) Serial.print("[FIRE] ");
      if (gasAlert)  Serial.print("[GAS] ");
      if (intrusion) Serial.print("[INTRUSION] ");
      Serial.println();
    } else {
      Serial.println("OK ✅");
    }
  }
}