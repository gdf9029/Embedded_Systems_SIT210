// Task 4.2C: Multiple Interrupts
// This program uses two interrupts to control two separate LEDs.
// - A push button toggles LED 1.
// - A PIR motion sensor toggles LED 2.
// The main loop does not poll sensors; it only acts on flags set by the ISRs.

// --- Pin Definitions ---
const int BUTTON_PIN = 2; // Interrupt pin for the button
const int PIR_PIN = 3;    // Interrupt pin for the PIR sensor
const int LED1_PIN = 4;   // LED controlled by the button
const int LED2_PIN = 5;   // LED controlled by the PIR sensor

// --- Volatile variables to be modified by ISRs ---
// 'volatile' is crucial as these are shared between ISRs and the main loop.
volatile bool buttonFlag = false;
volatile bool motionFlag = false;

// --- Variables for Debouncing ---
volatile unsigned long lastButtonInterruptTime = 0;
volatile unsigned long lastPirInterruptTime = 0;
const long debounceDelay = 500; // Debounce delay in milliseconds

void setup() {
  // Start serial communication for monitoring
  Serial.begin(9600);
  while (!Serial); // Wait for Serial Monitor to open

  Serial.println("Task 4.2C: Multiple Interrupt System Initialized");
  Serial.println("----------------------------------------------");

  // Set up pin modes
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Use internal pull-up for the button
  pinMode(PIR_PIN, INPUT);          // PIR sensor pin is an input

  // Attach Interrupt 1 for the Button
  // Triggers buttonISR() when the pin value goes from HIGH to LOW (FALLING edge with PULLUP)
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

  // Attach Interrupt 2 for the PIR Motion Sensor
  // Triggers pirISR() when the sensor output goes from LOW to HIGH (RISING edge)
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), pirISR, RISING);
}

void loop() {
  // The main loop is now purely event-driven.
  // It performs no polling and only acts when an ISR sets a flag to true.

  // 1. Check if the button interrupt has occurred
  if (buttonFlag) {
    // Toggle the state of LED 1
    digitalWrite(LED1_PIN, !digitalRead(LED1_PIN));

    // Log the event to the Serial Monitor
    Serial.print("Event Detected: Button Press! Toggling LED 1 to ");
    Serial.println(digitalRead(LED1_PIN) ? "ON" : "OFF");

    // Reset the flag to wait for the next interrupt
    buttonFlag = false;
  }

  // 2. Check if the motion sensor interrupt has occurred
  if (motionFlag) {
    // Toggle the state of LED 2
    digitalWrite(LED2_PIN, !digitalRead(LED2_PIN));

    // Log the event
    Serial.print("Event Detected: Motion! Toggling LED 2 to ");
    Serial.println(digitalRead(LED2_PIN) ? "ON" : "OFF");

    // Reset the flag
    motionFlag = false;
  }
}

// --- Interrupt Service Routines (ISRs) ---
// These functions are called automatically when an interrupt is triggered.
// They should be kept as short and fast as possible.

void buttonISR() {
  // Debounce the button: check if enough time has passed since the last press
  if ((millis() - lastButtonInterruptTime) > debounceDelay) {
    buttonFlag = true; // Set the flag for the main loop to handle
    lastButtonInterruptTime = millis(); // Update the time of the last interrupt
  }
}

void pirISR() {
  // Debounce the PIR sensor to prevent rapid, successive triggers
  if ((millis() - lastPirInterruptTime) > debounceDelay) {
    motionFlag = true; // Set the flag for the main loop to handle
    lastPirInterruptTime = millis(); // Update the time of the last interrupt
  }
}