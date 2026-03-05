#include <DHT.h>

#define DHTPIN 14      // P14
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("DHT11 test...");
  dht.begin();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT11 (check wiring)!");
  } else {
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print("%  Temp: ");
    Serial.print(t);
    Serial.println("C");
  }

  delay(2000);
}