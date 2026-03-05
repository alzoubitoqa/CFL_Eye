#define BUZZ 25

void setup() {
  ledcSetup(0, 2000, 8);     // تردد 2000Hz
  ledcAttachPin(BUZZ, 0);
}

void loop() {
  ledcWrite(0, 128);         // تشغيل
  delay(500);
  ledcWrite(0, 0);           // إيقاف
  delay(500);
}