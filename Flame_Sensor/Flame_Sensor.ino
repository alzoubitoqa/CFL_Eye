#define FLAME_DO 26   // DO -> P26

void setup() {
  Serial.begin(115200);
  pinMode(FLAME_DO, INPUT);
  delay(500);
  Serial.println("Flame sensor test...");
}

void loop() {

  int flame = digitalRead(FLAME_DO);

  if (flame == 0) {
    Serial.println(" FIRE DETECTED !");
  } else {
    Serial.println("Safe");
  }

  delay(300);
}