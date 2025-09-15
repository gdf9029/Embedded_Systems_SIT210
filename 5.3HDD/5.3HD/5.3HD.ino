#include <SPI.h>
#include <WiFiNINA.h>


const char* ssid = "aditya-2.4";    
const char* pass = "14442021"; 


//Firebase link
const char* firebaseHost = "traffic-light-highdistinction-default-rtdb.asia-southeast1.firebasedatabase.app";


// Pin definitions for the LEDs
const int redLedPin = 2;
const int greenLedPin = 3;
const int yellowLedPin = 4; 

const long loopDelay = 2000; // Check Firebase every 2 seconds (Server Polling)

WiFiSSLClient client;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Set LED pins to output mode
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);

  // Attempt to connect to Wi-Fi
  connectToWiFi();
}

void loop() { // Main repeating part of the program
  // Check the state from Firebase periodically
  checkFirebaseState(); // Gets updates from firebase
  delay(loopDelay); // 2 second interval
}

void connectToWiFi() {
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    delay(5000); // Wait 5 seconds for connection
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Function to get the current LED states from Firebase
void checkFirebaseState() {
  if (client.connect(firebaseHost, 443)) { // connects to the firebase server on the secure HTTPS port 443
    Serial.println("Connecting to Firebase to get data...");
    // Send HTTP GET request to the 'leds.json' path
    client.println("GET /leds.json HTTP/1.1"); // ask for the data at the /leds.json path
    client.print("Host: ");
    client.println(firebaseHost); // specifies that we are taking to this server
    client.println("Connection: close"); // Telling the server to close connection after we are done
    client.println();

    // Wait for the server's response
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) { // 5-second timeout
        Serial.println(">>> Client Timeout!");
        client.stop();
        return;
      }
    }

    // Read the response and find the JSON part
    String response = "";
    bool jsonStarted = false; // a flag to track when we findd the start of the JSON data
    while (client.available()) {
      char c = client.read(); // reads the string character-by-character
      if (c == '{') { //As json objects are starting with {} so i have used this to mark the beginning of the object
        jsonStarted = true;
      }
      if (jsonStarted) {
        response += c; //appends the character to our string
      }
    }
    client.stop();

    Serial.print("Received data: ");
    Serial.println(response);

    // Parse the JSON response and update the LEDs
    parseAndUpdateLEDs(response);

  } else {
    Serial.println("Connection to Firebase failed.");
  }
}

// Manually parse the JSON string to control LEDs
void parseAndUpdateLEDs(String json) {
  if (json.length() == 0) return; // skips the function if string is empty

  // Control Red LED
  if (json.indexOf("\"red\":true") != -1) {
    digitalWrite(redLedPin, HIGH);
    Serial.println("Red LED ON");
  } else if (json.indexOf("\"red\":false") != -1) {
    digitalWrite(redLedPin, LOW);
    Serial.println("Red LED OFF");
  }

  // Control Green LED
  if (json.indexOf("\"green\":true") != -1) {
    digitalWrite(greenLedPin, HIGH);
    Serial.println("Green LED ON");
  } else if (json.indexOf("\"green\":false") != -1) {
    digitalWrite(greenLedPin, LOW);
    Serial.println("Green LED OFF");
  }

  // Control Yellow LED
  if (json.indexOf("\"yellow\":true") != -1) {
    digitalWrite(yellowLedPin, HIGH);
    Serial.println("Yellow LED ON");
  } else if (json.indexOf("\"yellow\":false") != -1) {
    digitalWrite(yellowLedPin, LOW);
    Serial.println("Yellow LED OFF");
  }
  Serial.println("---");
}