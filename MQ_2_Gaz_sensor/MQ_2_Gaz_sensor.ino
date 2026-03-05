#define MQ2_PIN 34   // AO → P34

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("MQ2 Gas Sensor Test...");
}

void loop() {

  int gasValue = analogRead(MQ2_PIN);

  Serial.print("Gas Level: ");
  Serial.println(gasValue);

  delay(1000);
}