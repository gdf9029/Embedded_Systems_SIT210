// Task4_3D_from_4_2C.ino
// Nano 33 IoT (SAMD21) - Multiple external interrupts + timer interrupt
// Requires: SAMDTimerInterrupt library 

#include <SAMDTimerInterrupt.h>


const int BUTTON_PIN = 2; // interrupt pin for button (with INPUT_PULLUP)
const int PIR_PIN    = 3; // sensor 1 (e.g., PIR)
const int SND_PIN    = 4; // sensor 2 (e.g., sound or another digital sensor)
const int LED1_PIN   = 5; // LED toggled by button
const int LED2_PIN   = 6; // LED toggled by PIR
const int LED3_PIN   = 7; // LED toggled by timer

// --- Event types for ring buffer ---
enum Event : uint8_t { EVT_NONE = 0, EVT_BUTTON = 1, EVT_PIR = 2, EVT_SND = 3, EVT_TIMER = 4 };

// --- Simple lock-free ring buffer (size 8) ---
volatile Event evBuf[8];
volatile uint8_t evHead = 0; // consumer index
volatile uint8_t evTail = 0; // producer index
#define EV_NEXT(i) (((i) + 1) & 7)

// --- Debounce/state variables (main loop context) ---
unsigned long lastButtonTime = 0;
unsigned long lastPirTime = 0;
unsigned long lastSndTime = 0;
const unsigned long DEBOUNCE_MS = 200; // debounce interval used in loop()

// --- Timer setup (SAMDTimerInterrupt) ---
SAMDTimer Timer(TIMER_TC3); // TC3 usually available on Nano 33 IoT
const uint32_t TIMER_MS = 1000; // timer interval in milliseconds

// --- Utility: push event into buffer (called from ISRs) ---
inline void pushEvent(Event e) {
  uint8_t next = EV_NEXT(evTail);
  if (next == evHead) {
    // buffer full: drop oldest event (advance head)
    evHead = EV_NEXT(evHead);
  }
  evBuf[evTail] = e;
  evTail = next;
}

// --- ISRs (minimal) ---
void isr_button() { pushEvent(EVT_BUTTON); }
void isr_pir()    { pushEvent(EVT_PIR); }
void isr_snd()    { pushEvent(EVT_SND); }
void isr_timer()  { pushEvent(EVT_TIMER); }

void setup() {
  Serial.begin(115200);
  while (!Serial); // optional for debugging

  // pins
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP); // button to GND
  pinMode(PIR_PIN, INPUT);           // PIR module output
  pinMode(SND_PIN, INPUT);           // second digital sensor

  // attach external interrupts
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), isr_button, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), isr_pir, RISING);
  attachInterrupt(digitalPinToInterrupt(SND_PIN), isr_snd, RISING);

  // configure timer interrupt (interval in microseconds)
  if (!Timer.attachInterruptInterval(TIMER_MS * 1000, isr_timer)) {
    Serial.println("Timer attach failed");
  }

  Serial.println("Task 4.3D initialized - interrupts + timer running");
}

void loop() {
  // consume events until buffer empty
  while (evHead != evTail) {
    // read one event atomically (briefly disable interrupts for indices)
    noInterrupts();
    uint8_t idx = evHead;
    Event e = evBuf[idx];
    evHead = EV_NEXT(idx);
    interrupts();

    unsigned long now = millis();

    switch (e) {
      case EVT_BUTTON:
        if (now - lastButtonTime >= DEBOUNCE_MS) {
          lastButtonTime = now;
          // toggle LED1
          digitalWrite(LED1_PIN, !digitalRead(LED1_PIN));
          Serial.print("Button event: LED1 -> ");
          Serial.println(digitalRead(LED1_PIN) ? "ON" : "OFF");
        }
        break;

      case EVT_PIR:
        if (now - lastPirTime >= DEBOUNCE_MS) {
          lastPirTime = now;
          digitalWrite(LED2_PIN, !digitalRead(LED2_PIN));
          Serial.print("PIR event: LED2 -> ");
          Serial.println(digitalRead(LED2_PIN) ? "ON" : "OFF");
        }
        break;

      case EVT_SND:
        if (now - lastSndTime >= DEBOUNCE_MS) {
          lastSndTime = now;
          // example action: log only
          Serial.println("Sensor2 event: detected (logged)");
        }
        break;

      case EVT_TIMER:
        // periodic task: toggle LED3 and print sensor summary
        digitalWrite(LED3_PIN, !digitalRead(LED3_PIN));
        Serial.print("Timer tick: LED3 -> ");
        Serial.println(digitalRead(LED3_PIN) ? "ON" : "OFF");
        // Could also sample analog sensors here if needed
        break;

      default:
        break;
    }
  }

  // small non-blocking idle; keep loop responsive
  delay(1);
}