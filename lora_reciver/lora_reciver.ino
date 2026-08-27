#include <SPI.h>
#include <LoRa.h>

#define SS 10
#define RST 9
#define DIO0 2
#define LED_PIN 3  // LED connected to D3

#define RELAY1_PIN 5  // Relay 1 connected to D5
#define RELAY2_PIN 6  // Relay 2 connected to D6
#define RELAY3_PIN 7  // Relay 3 connected to D7

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);     // Set LED pin as output
  digitalWrite(LED_PIN, LOW);   // Ensure LED is off initially
  
  // Set relay pins as output
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  
  // Ensure relays are off initially (active low)
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, HIGH);
  digitalWrite(RELAY3_PIN, HIGH);

  LoRa.setPins(SS, RST, DIO0);

  while (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed. Retrying...");
    delay(500);
  }

  LoRa.setSyncWord(0xF3);
  LoRa.setTxPower(20, PA_OUTPUT_PA_BOOST_PIN);
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(250E3);
  LoRa.setCodingRate4(5);

  Serial.println("LoRa Receiver Ready");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize > 0) {
    String received = "";
    while (LoRa.available()) {
      received += (char)LoRa.read();
    }

    if (received.startsWith("MYPROJ:") && received.length() == 8) {
      char command = received.charAt(7);

      // Debugging: Print the received command
      Serial.print("Received Command: ");
      Serial.println(command);

      if (command == 'W') {
        // Turn on relay1 (D5) for 500ms
        digitalWrite(RELAY1_PIN, LOW);
        Serial.println("Relay 1 ON");
        delay(500);
        digitalWrite(RELAY1_PIN, HIGH);W
        Serial.println("Relay 1 OFF");
      } else if (command == 'X') {
        // Turn on relay2 (D6) for 500ms
        digitalWrite(RELAY2_PIN, LOW);
        Serial.println("Relay 2 ON");
        delay(500);
        digitalWrite(RELAY2_PIN, HIGH);
        Serial.println("Relay 2 OFF");
      } else if (command == 'Z') {
        // Turn on relay3 (D7) for 500ms
        digitalWrite(RELAY3_PIN, LOW);
        Serial.println("Relay 3 ON");
        delay(500);
        digitalWrite(RELAY3_PIN, HIGH);
        Serial.println("Relay 3 OFF");
      } else {
        // For other characters, turn relays on sequentially for 2 seconds each
        Serial.println("Executing Default Task...");

        digitalWrite(RELAY1_PIN, LOW);
        Serial.println("Relay 1 ON");
        delay(5000); // Relay 1 on for 2 seconds
        digitalWrite(RELAY1_PIN, HIGH);
        Serial.println("Relay 1 OFF");

        digitalWrite(RELAY2_PIN, LOW);
        Serial.println("Relay 2 ON");
        delay(7000); // Relay 2 on for 2 seconds
        digitalWrite(RELAY2_PIN, HIGH);
        Serial.println("Relay 2 OFF");

        digitalWrite(RELAY3_PIN, LOW);
        Serial.println("Relay 3 ON");
        delay(2000); // Relay 3 on for 2 seconds
        digitalWrite(RELAY3_PIN, HIGH);
        Serial.println("Relay 3 OFF");
      }

      // Flash LED to indicate reception
      digitalWrite(LED_PIN, HIGH);
      delay(500);
      digitalWrite(LED_PIN, LOW);
    }
  }
}
