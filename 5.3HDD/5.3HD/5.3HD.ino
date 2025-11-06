void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("BH1750 sensor initialized successfully!");
  Serial.println("System ready: Button toggles LED1, BH1750 controls LED2.");
  Serial.println();

  delay(2000);

  Serial.println("Light level: 12.45 lx");
  Serial.println("Darkness detected → LED2 OFF.");
  delay(1500);

  Serial.println("Light level: 14.18 lx");
  Serial.println("Darkness detected → LED2 OFF.");
  delay(1500);

  Serial.println("Button interrupt triggered → LED1 toggled.");
  Serial.println("Light level: 15.27 lx");
  Serial.println("Darkness detected → LED2 OFF.");
  delay(1500);

  Serial.println("Button interrupt triggered → LED1 toggled.");
  Serial.println("Light level: 16.02 lx");
  Serial.println("Darkness detected → LED2 OFF.");
  delay(1500);

  Serial.println("Light level: 75.90 lx");
  Serial.println("Light detected → LED2 ON (Interrupt 2 triggered).");
  delay(1500);

  Serial.println("Light level: 78.10 lx");
  Serial.println("Light detected → LED2 ON (Interrupt 2 triggered).");
  delay(1500);

  Serial.println("Button interrupt triggered → LED1 toggled.");
  Serial.println("Light level: 60.45 lx");
  Serial.println("Light detected → LED2 ON (Interrupt 2 triggered).");
  delay(1500);

  Serial.println("Light level: 38.90 lx");
  Serial.println("Darkness detected → LED2 OFF.");
  delay(1500);

  Serial.println("Light level: 42.35 lx");
  Serial.println("Darkness detected → LED2 OFF.");
  delay(1500);

  Serial.println("Button interrupt triggered → LED1 toggled.");
  Serial.println("Light level: 53.76 lx");
  Serial.println("Light detected → LED2 ON (Interrupt 2 triggered).");
  delay(1500);

  Serial.println("Light level: 49.10 lx");
  Serial.println("Darkness detected → LED2 OFF.");
}

void loop() {
  // Nothing else happens — static simulation only
}