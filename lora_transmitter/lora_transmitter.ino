// | **LoRa Pin** | **New ESP32 GPIO** | Notes          |
// | ------------ | ------------------ | -------------- |
// | SS (NSS)     | GPIO5              | stays the same |
// | RST          | **GPIO27**         | was GPIO14     |
// | DIO0         | **GPIO26**         | was GPIO2      |
// | SCK          | GPIO18             | SPI Clock      |
// | MISO         | GPIO19             | SPI MISO       |
// | MOSI         | GPIO23             | SPI MOSI       |

#include <LoRa.h>

// Updated LoRa Pin Definitions (safe with TFT)
#define LORA_SS   5
#define LORA_RST  27  // moved from 14
#define LORA_DIO0 26  // moved from 2

void setup() {
  Serial.begin(115200);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  while (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed. Retrying...");
    delay(500);
  }

  LoRa.setSyncWord(0xF3); // Custom sync word
  LoRa.setTxPower(20, PA_OUTPUT_PA_BOOST_PIN);
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(250E3);
  LoRa.setCodingRate4(5);

  Serial.println("LoRa Transmitter Ready. Type H, E, M, T, B, or S then press ENTER.");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');  // Read line until newline
    input.trim();  // Remove whitespace, carriage return, etc.

    if (input.length() == 1) {
      char inputChar = toupper(input.charAt(0));

      if (inputChar == 'H' || inputChar == 'E' || inputChar == 'M' ||
          inputChar == 'T' || inputChar == 'B' || inputChar == 'S') {
        LoRa.beginPacket();
        LoRa.print("MYPROJ:");
        LoRa.print(inputChar);
        LoRa.endPacket();
        Serial.print("Sent: ");
        Serial.println(inputChar);
        delay(50); // Give LoRa time to transmit before next input
      } else {
        Serial.print("Invalid character ignored: ");
        Serial.println(inputChar);
      }
    } else {
      Serial.println("Invalid input: Only one character at a time allowed.");
    }
  }
}
