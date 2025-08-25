#include <WiFiNINA.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

char ssid[] = "aditya-2.4";
char pass[] = "14442021";

char server[] = "api.thingspeak.com";
String apiKey = "RENERXS7UBE9WEPK";

WiFiClient client;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.print("Connecting to WiFi");
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");

  dht.begin();
}

void loop() {
  float temp = dht.readTemperature();

  if (isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.println(temp);

  if (client.connect(server, 80)) {
    String url = "/update?api_key=" + apiKey + "&field1=" + String(temp);

    Serial.print("Requesting URL: ");
    Serial.println(url);

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + server + "\r\n" +
                 "Connection: close\r\n\r\n");

    delay(1000); // Wait for server to process
  } else {
    Serial.println("Connection to ThingSpeak failed!");
  }

  client.stop();
  delay(20000); // Wait at least 20s to avoid rate limit
}
