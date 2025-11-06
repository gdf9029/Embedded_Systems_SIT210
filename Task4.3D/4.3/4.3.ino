// Task4_3D_MinimalFlags_Timer.ino
// Nano 33 IoT (SAMD21) - Minimal change from 4.2C to meet 4.3D
// - Three external interrupts: button, PIR, sensor2
// - One hardware timer interrupt that sets a flag
// - All heavy work, debouncing, and Serial I/O done in loop()
// Requires: SAMDTimerInterrupt library (install via Library Manager)

#include <SAMDTimerInterrupt.h>


const int BUTTON_PIN = 2; // button (use INPUT_PULLUP)
const int PIR_PIN    = 3; // PIR sensor
const int SND_PIN    = 4; // second digital sensor
const int LED1_PIN   = 5; // controlled by button
const int LED2_PIN   = 6; // controlled by PIR
const int LED3_PIN   = 7; // controlled by timer

// --- Volatile flags set by ISRs ---
volatile bool buttonFlag = false;
volatile bool pirFlag    = false;
volatile bool sndFlag    = false;
volatile bool timerFlag  = false;

// --- Debounce / timing (handled in loop) ---
unsigned long lastButtonTime = 0;
unsigned long lastPirTime    = 0;
unsigned long lastSndTime    = 0;
const unsigned long DEBOUNCE_MS = 200; // debounce interval in ms

// --- Timer setup ---
SAMDTimer Timer(TIMER_TC3);       // TC3 commonly available on Nano 33 IoT
const uint32_t TIMER_MS = 1000;   // timer interval in milliseconds

// --- ISRs (minimal: only set flags) ---
void isr_button() { buttonFlag = true; }
void isr_pir()    { pirFlag = true;    }
void isr_snd()    { sndFlag = true;    }
void isr_timer()  { timerFlag = true;  }

void setup() {
  Serial.begin(115200);
  while (!Serial); // optional for debugging; remove for headless runs

  // Configure pins
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP); // button to GND
  pinMode(PIR_PIN, INPUT);           // PIR module output
  pinMode(SND_PIN, INPUT);           // second digital sensor

  // Attach external interrupts
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), isr_button, FALLING); // button press
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), isr_pir, RISING);        // PIR: motion -> HIGH
  attachInterrupt(digitalPinToInterrupt(SND_PIN), isr_snd, RISING);        // sensor2: RISING or CHANGE

  // Configure timer interrupt (interval specified in microseconds)
  if (!Timer.attachInterruptInterval(TIMER_MS * 1000, isr_timer)) {
    Serial.println("Timer attach failed");
  }

  Serial.println("Task4_3D Minimal Flags + Timer initialized");
}

void loop() {
  unsigned long now = millis();

  // --- Handle button events (debounced in loop) ---
  if (buttonFlag) {
    // clear flag early to avoid missing a new press while processing
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

  // --- Handle sensor2 events ---
  if (sndFlag) {
    noInterrupts();
    sndFlag = false;
    interrupts();

    if (now - lastSndTime >= DEBOUNCE_MS) {
      lastSndTime = now;
      // Example action: log only (or toggle another LED if you prefer)
      Serial.println("Sensor2: event detected (logged)");
    }
  }

  // --- Handle timer events ---
  if (timerFlag) {
    noInterrupts();
    timerFlag = false;
    interrupts();

    // Periodic action: toggle LED3 and log
    digitalWrite(LED3_PIN, !digitalRead(LED3_PIN));
    Serial.print("Timer: LED3 -> ");
    Serial.println(digitalRead(LED3_PIN) ? "ON" : "OFF");


  }

  // Keep loop responsive; avoid long blocking delays
  delay(1);
}