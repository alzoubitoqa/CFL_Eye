void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Blink test running...");
  pinMode(2, OUTPUT);
}

void loop() {
  digitalWrite(2, !digitalRead(2));
  Serial.println("Toggled GPIO2");
  delay(1000);
}