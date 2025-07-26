// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

const int DOT = 400; // Shorter delay for dot
const int LINE = 1000; // Longer delay for line 
const int between = 100; // delay between dots and lines

//Creating methods for dots and lines
void dot()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(DOT);
  digitalWrite(LED_BUILTIN, LOW);
  delay(between);
}

void line()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(LINE);
  digitalWrite(LED_BUILTIN, LOW);
  delay(between);
}
// the loop function runs over and over again forever

//Blinking LED code for morse code (Name: Aditya) .- -.. .. - -.-- .-
void loop() {
dot();
line();
delay(1200 - between);
line();
dot();
dot();
delay(1200 - between);
dot();
dot();
delay(1200 - between);
line();
delay(1200 - between);
line();
dot();
line();
line();
delay(1200 - between);
dot();
line();
delay(5000);                         
}
