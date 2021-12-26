#define BLYNK_TEMPLATE_ID "TMPLJKNkKkzy"
#define BLYNK_DEVICE_NAME "ESP8266"
#define BLYNK_AUTH_TOKEN "73T5gcF4txzJkT1D7TALkTMQa-lKNGaP"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial

#define APP_DEBUG

#include "BlynkEdgent.h"

#include "DHT.h"
#include <PubSubClient.h>
#define DHTTYPE DHT11

DHT dht(D1, DHTTYPE);

#define mqtt_server "broker.mqttdashboard.com"
#define mqtt_user "kahfi"
#define mqtt_password "1"

#define humidity_topic "kahfi/sensor/humidity"
#define temperature_topic "kahfi/sensor/temperature"

WiFiClient espClient;
PubSubClient client(espClient);

BLYNK_WRITE(V0)
{
  int pinValue = param.asInt();
  digitalWrite(D0, pinValue);
}
void setup()
{
  pinMode(D0, OUTPUT);
  Serial.begin(9600);
  dht.begin();
  client.setServer(mqtt_server, 1883);
  BlynkEdgent.begin();
  delay(1000);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ujicobadulu")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

bool checkBound(float newValue, float prevValue, float maxDiff) {
  return !isnan(newValue) &&
         (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}

long lastMsg = 0;
float temp = 0.0;
float hum = 0.0;
float diff = 1.0;

void loop() {
//  if (!client.connected()) {
//    reconnect();
//  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;

    float newTemp = dht.readTemperature();
    float newHum = dht.readHumidity();

    if (checkBound(newTemp, temp, diff)) {
      temp = newTemp;
      Serial.print("New temperature:");
      Serial.println(String(temp).c_str());
      client.publish(temperature_topic, String(temp).c_str(), true);
      Blynk.virtualWrite(V1, temp);
    }

    if (checkBound(newHum, hum, diff)) {
      hum = newHum;
      Serial.print("New humidity:");
      Serial.println(String(hum).c_str());
      client.publish(humidity_topic, String(hum).c_str(), true);
      Blynk.virtualWrite(V2, hum);
    }
    BlynkEdgent.run();
  }
}
