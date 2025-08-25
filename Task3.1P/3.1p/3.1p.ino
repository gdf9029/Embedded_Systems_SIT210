
#include <SPI.h>
#include <WiFiNINA.h>
#include <Wire.h>
#include <BH1750.h> // Library for the BH1750 Sensor

const char* ssid = "aditya-2.4";        
const char* pass = "14442021";    
String iftttKey = "h3eEFEdWa6QMjwftAu8G3UOW0pfh9_A6WJUa-x0A569";        
int status = WL_IDLE_STATUS;

// IFTTT Webhook Details
const char* server = "maker.ifttt.com"; // IFTTT server address
String eventSunlightOn = "sunlight_on";   // Event name for when sunlight is detected
String eventSunlightOff = "sunlight_off"; // Event name for when sunlight is gone

// Sensor Object
BH1750 lightMeter;

// Logic Variables
float lightLevel = 0; // Use float for lux readings
int lightThreshold = 150; // A low threshold for easy testing indoors.

bool isSunny = false; // State variable to track if it's currently sunny

// Shortened delay for easier testing
const long triggerDelay = 30000; // 30-second delay (30,000 ms)

// Initialize lastTriggerTime so the first trigger can fire immediately
unsigned long lastTriggerTime = -triggerDelay; 

WiFiClient client;

void setup() {
  // Initialize Serial communication for debugging
  Serial.begin(9600);
  while (!Serial);

  // Initialize I2C communication for the BH1750 sensor
  Wire.begin();
  
  // Initialize the BH1750 sensor and check for success
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Initialized Successfully"));
  } else {
    Serial.println(F("Error initialising BH1750! Check wiring."));
    while(1); // Halt execution if sensor fails
  }

  // Check for the WiFi module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // Now, attempt to connect to Wi-Fi network
  connectToWiFi();
}

void loop() {
  // Check WiFi connection status, reconnect if necessary
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    connectToWiFi();
  }

  // Read the light level from the BH1750 sensor
  lightLevel = lightMeter.readLightLevel();

  // Print the light level to the Serial Monitor for debugging/calibration
  Serial.print("Light Level: ");
  Serial.print(lightLevel);
  Serial.println(" lx");

  // Check if enough time has passed since the last trigger
  if (millis() - lastTriggerTime > triggerDelay) {
    // ---- LOGIC FOR SUNLIGHT ON ----
    // If it's not currently sunny AND the light level is above the threshold
    if (!isSunny && lightLevel > lightThreshold) {
      Serial.println("Sunlight detected! Triggering IFTTT...");
      triggerIFTTT(eventSunlightOn); // Call the function to trigger IFTTT
      isSunny = true; // Update the state
      lastTriggerTime = millis(); // Record the time of this trigger
    }
    // ---- LOGIC FOR SUNLIGHT OFF ----
    // If it IS currently sunny AND the light level is below the threshold
    else if (isSunny && lightLevel < lightThreshold) {
      Serial.println("Sunlight gone. Triggering IFTTT...");
      triggerIFTTT(eventSunlightOff); // Call the function to trigger IFTTT
      isSunny = false; // Update the state
      lastTriggerTime = millis(); // Record the time of this trigger
    }
  }

  delay(5000); // Wait 5 seconds before the next reading
}

void connectToWiFi() {
  // attempt to connect to WiFi network
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
  
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection
    delay(10000);
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();
}

void triggerIFTTT(String eventName) {
  // Add a check for the placeholder key
  if (iftttKey == "YOUR_IFTTT_KEY") {
    Serial.println("ERROR: Please replace 'YOUR_IFTTT_KEY' with your actual IFTTT key.");
    return; // Stop the function from running
  }

  // Connect to the IFTTT server
  if (client.connect(server, 80)) {
    Serial.println("Connecting to IFTTT...");
    // Make a HTTP request
    String url = "/trigger/" + eventName + "/with/key/" + iftttKey;
    client.println("GET " + url + " HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("User-Agent: Arduino/1.0");
    client.println("Connection: close");
    client.println(); 
    
    // 
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) { // 5 second timeout
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    // Read the response from the server 
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    
    client.stop(); // Disconnect from the server
    Serial.println("\nIFTTT Trigger Sent!");
  } else {
    Serial.println("Connection to IFTTT failed.");
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}