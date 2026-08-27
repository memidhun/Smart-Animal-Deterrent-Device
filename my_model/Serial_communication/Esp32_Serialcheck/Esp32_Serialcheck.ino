#define LED_PIN 2  // Onboard LED pin on ESP32 boards

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("ESP32 Serial Communication Started");
}

void loop() {
  if (Serial.available() > 0) {
    char receivedChar = Serial.read();

    // Blink onboard LED
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);

    // Print animal name
    switch (receivedChar) {
      case 'E':
        Serial.println("Elephant");
        break;
      case 'T':
        Serial.println("Tiger");
        break;
      case 'S':
        Serial.println("Squirrel");
        break;
      case 'B':
        Serial.println("Bat");
        break;
      case 'M':
        Serial.println("Monkey");
        break;
      case 'H':
        Serial.println("Human");
        break;
    }

    Serial.println("------------------"); // Visual separator
  }

  delay(10); // Small delay to avoid overwhelming the loop
}
