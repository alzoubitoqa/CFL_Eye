#define IR_DO 27   // DO -> P27

void setup() {
  Serial.begin(115200);
  pinMode(IR_DO, INPUT);
  delay(500);
  Serial.println("IR sensor test (DO)...");
}

void loop() {
  int v = digitalRead(IR_DO);
  Serial.print("IR DO = ");
  Serial.println(v);
  delay(200);
}