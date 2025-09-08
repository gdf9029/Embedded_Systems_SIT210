#include "DHT.h"

#define DHTPIN 6
#define DHTTYPE DHT22

const int buttonPin = 2;
const int led1Pin = 4;
const int led2Pin = 5;
const int led3Pin = 6;

DHT dht(DHTPIN, DHTTYPE);

volatile bool buttonFlag = false;

long previousMillis = 0;
const long interval = 2000;

void buttonISR();

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  dht.begin();
  
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonISR, CHANGE);
  
  Serial.println("System Initialized!");
}

void buttonISR() {
  buttonFlag = true;
}

void loop() {
  if (buttonFlag) {
    digitalWrite(led1Pin, !digitalRead(led1Pin));
    Serial.println("Button interrupt triggered: Toggling LED1");
    buttonFlag = false;
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      digitalWrite(led2Pin, LOW);
    } else {
      Serial.print("Sensor reading: Humidity: ");
      Serial.print(humidity);
      Serial.print(" %, Temperature: ");
      Serial.print(temperature);
      Serial.println(" C");
      
      digitalWrite(led2Pin, !digitalRead(led2Pin));
    }
    
    digitalWrite(led3Pin, !digitalRead(led3Pin));
  }
}

