#define LED_FLAME 32
#define LED_IR    12
#define LED_GAS   13
#define LED_DHT   33

void setup() {
  pinMode(LED_FLAME, OUTPUT);
  pinMode(LED_IR, OUTPUT);
  pinMode(LED_GAS, OUTPUT);
  pinMode(LED_DHT, OUTPUT);
}

void loop() {
  digitalWrite(LED_FLAME, HIGH); delay(250); digitalWrite(LED_FLAME, LOW); delay(150);
  digitalWrite(LED_IR,    HIGH); delay(250); digitalWrite(LED_IR,    LOW); delay(150);
  digitalWrite(LED_GAS,   HIGH); delay(250); digitalWrite(LED_GAS,   LOW); delay(150);
  digitalWrite(LED_DHT,   HIGH); delay(250); digitalWrite(LED_DHT,   LOW); delay(600);
}