#include <Adafruit_GFX.h>
#include "Adafruit_SH1106.h"
#define OLED_RESET 4
Adafruit_SH1106 display(OLED_RESET);
char receivedChar;
unsigned long lastUpdateTime = 0;
const unsigned long debounceDelay = 200; // Adjust debounce delay as needed

void setup() {
  Serial.begin(9600);
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.display();
  display.clearDisplay();
  display.setTextSize(2);  // Set text size here
  display.setTextColor(WHITE);
}

void loop() {
  // Check for serial data continuously
  while (Serial.available() > 0) {
    receivedChar = Serial.read();
    if (receivedChar == '1') {
      if (millis() - lastUpdateTime >= debounceDelay) {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Elephant");
        display.display();
        lastUpdateTime = millis();
      }
    } 
    else if (receivedChar == '0') {
      if (millis() - lastUpdateTime >= debounceDelay) {
        display.clearDisplay();
        display.setCursor(0, 20);
        display.print("No Elephant");
        display.display();
        lastUpdateTime = millis();
      }
    }
  }
}
