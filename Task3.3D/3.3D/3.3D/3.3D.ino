//The necessary libraries that i have included for the implementation of this task
#include <WiFiNINA.h>          // For Wi-Fi connectivity on the Nano 33 IoT
#include <ArduinoMqttClient.h> // For MQTT communication

//Wifi Credentials
#define WiFi_SSID "Mi 11X Pro"
#define WiFi_PASS "12345678"

const char* Name = "Aditya"; 



//  MQTT Broker Configuration as given in the Tasksheet
const char broker[] = "broker.emqx.io";
const int  port     = 1883;
const char topic[]  = "SIT210/wave";

//Defining Pins for connection
const int ledPin  = 6;  // Digital pin for the LED
const int trigPin = 9;  // Ultrasonic sensor Trigger Pin
const int echoPin = 10; // Ultrasonic sensor Echo pin

//Set conditions for trigger and assigning intervals to avoid message flooding
const float waveThreshold   = 15.0;   // Distance in cm to trigger a "wave"
const long  publishInterval = 5000;   // Cooldown in ms to prevent message spam
unsigned long lastPublishTime = 0;    // Stores the last time a message was published

//Creating instances of objects 
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// functions for connecting to Wifi and MQTT 
void connectWiFi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(WiFi_SSID);

  // Loop until a Wi-Fi connection is established
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(WiFi_SSID, WiFi_PASS);
    Serial.print(".");
    delay(5000); // Wait 5 seconds for connection
  }

  Serial.println("\n✅ WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}


void connectMQTT() {
  Serial.print("Connecting to MQTT broker: ");
  Serial.println(broker);

  // Set the function that will be called when a message is received
  mqttClient.onMessage(onMqttMessage);

  // Attempt to connect to the broker
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    // If connection fails, stop the program
    while (1);
  }

  Serial.println("✅ You're connected to the MQTT broker!");

  // Subscribe to the topic to receive messages
  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  mqttClient.subscribe(topic);
}


//setup function
void setup() {

  Serial.begin(9600);
  while (!Serial); // waiting till it gets connected

  // Now i configured the pins
  pinMode(ledPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  digitalWrite(ledPin, LOW); // We should make sure that LED is turned off first before execution

  Serial.println("SIT210 Task 3.3D - MQTT Wave & Pat System");
  
  // Connect to Wi-Fi and then to the MQTT Broker
  connectWiFi();
  connectMQTT();
}

void loop() {
  // This is crucial! It keeps the MQTT client alive, processing incoming
  // messages and maintaining the connection to the broker.
  mqttClient.poll();

  // Continuously check for a wave and publish a message if detected
  detectAndPublishWave();
}

//Now lets write the required functions for the implementation
/**
 * step 1: Callback function triggered when an MQTT message is received.
 * Also, the messageSize i.e. size of the incoming message should be in bytes
 */
void onMqttMessage(int messageSize) {
  Serial.println("---");
  Serial.print("Received a message on topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("': ");

  String message = "";
  // Read the message character by character
  while (mqttClient.available()) {
    message += (char)mqttClient.read();
  }
  Serial.println(message);

  // Check the message content and react by flashing the LED
  if (message.equalsIgnoreCase("pat")) {
    Serial.println("-> It's a 'pat'! Flashing LED quickly. 🐾");
    flashForPat();
  } else {
    Serial.println("-> It's a 'wave'! Flashing LED slowly. 👋");
    flashForWave();
  }
  Serial.println("---");
}

/**
 * step2:  Detects a "wave" using the ultrasonic sensor and publishes a message.
 */
void detectAndPublishWave() {
  // Check if enough time has passed since the last publication which is required for avoiding spam
  if (millis() - lastPublishTime < publishInterval) {
    return; // Exit if not enough time has already elapsed
  }

  // Ultrasonic sensing principle
  // Ensure the trigger pin is clear before sending a pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Send a 10-microsecond high pulse to trigger the sensor
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

 // Checks the duration of the travel (to be set for threshold further)
  long duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in centimeters.
  // Speed of sound is approx. 343 m/s or 
  // we will also divide by 2 because first it reaches to object and then back to the ultrasonic sensor
  float distance = duration * 0.0343 / 2.0;

  // Now since i am using MQTT we have to code 2 logics, 1: Publishing (Pub) / 2: Subscribe (Sub)
  // If the distance is valid and within the wave threshold...
  if (distance > 0 && distance < waveThreshold) {
    Serial.print("Wave detected! Distance: ");
    Serial.print(distance);
    Serial.println(" cm. Publishing message...");

    // publish name to the MQTT topic
    mqttClient.beginMessage(topic);
    mqttClient.print(Name);
    mqttClient.endMessage();

    // Reset the cooldown timer
    lastPublishTime = millis();
  }
}

/**
 * step3: As per the tasksheet, the number of times LED blinks is based on wave and pat
 */
void flashForWave() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
  }
}


void flashForPat() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }
}




