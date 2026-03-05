#define IR_DO 27

void setup() {
  Serial.begin(115200);
  pinMode(IR_DO, INPUT);
}

void loop() {

  int v = digitalRead(IR_DO);

  if (v == 0) {   // جسم تم اكتشافه
    Serial.println("⚠ MOTION DETECTED !");
  } else {
    Serial.println("Area Clear");
  }

  delay(300);
}