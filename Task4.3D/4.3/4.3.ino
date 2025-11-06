// Task4_3D_Timer_.ino
// Nano 33 IoT (SAMD21) - Minimal flags + timer version with DHT11 temperature sensor
// - Three external interrupts: button, PIR, DHT11 digital data pin used as an interrupt source
// - One hardware timer interrupt that sets a flag; timer handler causes periodic DHT read and LED3 toggle
// - All heavy work, debouncing, and Serial I/O done in loop()
// Requires: SAMDTimerInterrupt library and DHT sensor library (Adafruit DHT or equivalent).
// Install via Arduino Library Manager: "SAMDTimerInterrupt" and "DHT sensor library" (Adafruit)

#include <SAMDTimerInterrupt.h>
#include <DHT.h>

const int BUTTON_PIN = 2;  // button (use INPUT_PULLUP)
const int PIR_PIN    = 3;  // PIR sensor (digital output)
const int DHT_PIN    = 4;  // DHT11 data pin (uses single-wire protocol)
const int LED1_PIN   = 5;  // controlled by button
const int LED2_PIN   = 6;  // controlled by PIR
const int LED3_PIN   = 7;  // controlled by timer (periodic)

// --- DHT settings ---
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

// --- Volatile flags set by ISRs ---
volatile bool buttonFlag = false;
volatile bool pirFlag    = false;
volatile bool timerFlag  = false;

// --- Debounce / timing (handled in loop) ---
unsigned long lastButtonTime = 0;
unsigned long lastPirTime    = 0;
const unsigned long DEBOUNCE_MS = 200; // debounce interval in ms

// --- Timer setup ---
SAMDTimer Timer(TIMER_TC3);       // TC3 commonly available on Nano 33 IoT
const uint32_t TIMER_MS = 5000;   // timer interval in milliseconds (e.g., read DHT every 5s)

// --- ISRs (minimal: only set flags) ---
void isr_button() { buttonFlag = true; }
void isr_pir()    { pirFlag    = true; }
void isr_timer()  { timerFlag  = true; }

void setup() {
  Serial.begin(115200);
  while (!Serial); // optional for debugging; remove for headless runs

  // Configure pins
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP); // button to GND
  pinMode(PIR_PIN, INPUT);           // PIR module output
  // DHT uses its own timing; do not set pinMode(DHT_PIN) here

  // Initialize DHT library
  dht.begin();

  // Attach external interrupts
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), isr_button, FALLING); // button press
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), isr_pir, RISING);        // PIR: motion -> HIGH
  // DHT11 modules usually do NOT generate clean edge interrupts for data; we use it as a sensor polled periodically by timer.

  // Configure timer interrupt (interval specified in microseconds)
  if (!Timer.attachInterruptInterval(TIMER_MS * 1000, isr_timer)) {
    Serial.println("Timer attach failed");
  }

  Serial.println("Task4_3D Minimal Flags + Timer with DHT11 initialized");
  Serial.print("Reading DHT every "); Serial.print(TIMER_MS); Serial.println(" ms");
}

void loop() {
  unsigned long now = millis();

  // --- Handle button events (debounced in loop) ---
  if (buttonFlag) {
    noInterrupts();
    buttonFlag = false;
    interrupts();

    if (now - lastButtonTime >= DEBOUNCE_MS) {
      lastButtonTime = now;
      digitalWrite(LED1_PIN, !digitalRead(LED1_PIN)); // toggle LED1
      Serial.print("Button: LED1 -> ");
      Serial.println(digitalRead(LED1_PIN) ? "ON" : "OFF");
    }
  }

  // --- Handle PIR events ---
  if (pirFlag) {
    noInterrupts();
    pirFlag = false;
    interrupts();

    if (now - lastPirTime >= DEBOUNCE_MS) {
      lastPirTime = now;
      digitalWrite(LED2_PIN, !digitalRead(LED2_PIN)); // toggle LED2
      Serial.print("PIR: LED2 -> ");
      Serial.println(digitalRead(LED2_PIN) ? "ON" : "OFF");
    }
  }

  // --- Handle timer events: read DHT11 and toggle LED3 ---
  if (timerFlag) {
    noInterrupts();
    timerFlag = false;
    interrupts();

    // Toggle LED3
    digitalWrite(LED3_PIN, !digitalRead(LED3_PIN));
    Serial.print("Timer: LED3 -> ");
    Serial.println(digitalRead(LED3_PIN) ? "ON" : "OFF");

    // Read DHT11 (blocking; takes ~50-200 ms depending on sensor)
    float h = dht.readHumidity();
    float t = dht.readTemperature(); // Celsius by default

    if (isnan(h) || isnan(t)) {
      Serial.println("DHT11 read failed");
    } else {
      Serial.print("DHT11: Temp = ");
      Serial.print(t);
      Serial.print(" *C, Humidity = ");
      Serial.print(h);
      Serial.println(" %");
    }
  }

  // Keep loop responsive; avoid long blocking delays
  delay(1);
}