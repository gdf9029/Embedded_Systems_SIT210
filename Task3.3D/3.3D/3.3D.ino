/*SIT210 - Embedded Systems Development
  Task 3.3D: MQTT Publish/Subscribe with Arduino Nano 33 IoT

  This sketch connects an Arduino Nano 33 IoT to a Wi-Fi network and an MQTT broker.
  - It uses an ultrasonic sensor to detect a "wave" (an object coming close).
  - When a wave is detected, it publishes a message with the user's name to the "SIT210/wave" topic.
  - It subscribes to the "SIT210/wave" topic.
  - When a message is received on the topic, it prints the message to the Serial Monitor.
  - If the message contains "pat", it flashes the LED in one pattern.
  - For any other message (a "wave"), it flashes the LED in another pattern.

*/


#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>

//  Pin Definitions 
const int LED_PIN = 2;       // Pin for the LED
const int TRIG_PIN = 8;      // Pin for the ultrasonic sensor's Trigger
const int ECHO_PIN = 7;      // Pin for the ultrasonic sensor's Echo

//  WiFi Credentials 

char ssid[] = "aditya-2.4";      
char pass[] = "14442021";       
int status = WL_IDLE_STATUS;

// MQTT Broker Configuration 
const char broker[] = "broker.emqx.io";
int port = 1883;
const char topic[] = "SIT210/wave";

// Global Variables
const char* Name = "Aditya"; 
const int waveThreshold = 15;     // Threshold distance for triggering a wave
long lastPublishTime = 0;
const long publishInterval = 5000; // Cooldown period of 5 seconds to prevent spamming

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

void setup() {
  // Initialize Serial communication
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Initialize pin modes
  pinMode(LED_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Check for the WiFi module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  // Attempt to connect to WiFi
  connectToWiFi();

  // Set the message handler for the MQTT client
  mqttClient.onMessage(onMqttMessage);
}

void loop() {
  // Ensure we are connected to the MQTT broker
  if (!mqttClient.connected()) {
    reconnectToMqtt();
  }

  // Poll for incoming MQTT messages
  mqttClient.poll();

  // Detect a wave and publish a message
  detectAndPublishWave();
}

// --- WiFi Connection ---
void connectToWiFi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    delay(10000); // wait 10 seconds for connection
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();
}

// --- MQTT Connection ---
void reconnectToMqtt() {
  Serial.println("Attempting to connect to MQTT broker...");
  while (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    Serial.println("Retrying in 5 seconds...");
    delay(5000);
  }
  Serial.println("Connected to MQTT broker!");

  // Subscribe to the topic
  mqttClient.subscribe(topic);
  Serial.print("Subscribed to topic: ");
  Serial.println(topic);
}

// --- Wave Detection and Publishing ---
void detectAndPublishWave() {
  // Check if enough time has passed since the last publish
  if (millis() - lastPublishTime < publishInterval) {
    return;
  }
  
  // Get distance from ultrasonic sensor
  long duration, distance;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration / 2) / 29.1; // Formula to convert duration to cm

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // If an object is within the threshold, publish a message
  if (distance > 0 && distance < waveThreshold) {
    Serial.println("Wave detected!");
    Serial.print("Publishing message to topic: ");
    Serial.println(topic);

    mqttClient.beginMessage(topic);
    mqttClient.print(Name);
    mqttClient.endMessage();

    // Update the last publish time to start the cooldown
    lastPublishTime = millis();
  }

  delay(200); // Short delay to stabilize readings
}


// --- MQTT Message Handler ---
void onMqttMessage(int messageSize) {
  // We received a message, print the topic and contents
  Serial.println();
  Serial.print("Received a message on topic: ");
  Serial.println(mqttClient.messageTopic());
  Serial.print("Message: ");
  String message = "";
  while (mqttClient.available()) {
    message += (char)mqttClient.read();
  }
  Serial.println(message);

  // Check the message content and flash the LED accordingly
  if (message.equalsIgnoreCase("pat")) {
    flashForPat();
  } else {
    flashForWave();
  }
}

// --- LED Flash Patterns ---
void flashForWave() {
  Serial.println("Reacting to a wave...");
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(300);
    digitalWrite(LED_PIN, LOW);
    delay(300);
  }
}

void flashForPat() {
  Serial.println("Reacting to a pat...");
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

// --- Utility Functions ---
void printWifiStatus() {
  Serial.println("--- WiFi Status ---");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  Serial.println("-------------------");
}