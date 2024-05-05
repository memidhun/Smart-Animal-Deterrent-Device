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
  display.setTextSize(2);  
  display.setTextColor(WHITE);
}

void loop() {
  // Check for serial data continuously
  while (Serial.available() > 0) {
    receivedChar = Serial.read();
    if (receivedChar == '1') {
      if (millis() - lastUpdateTime >= debounceDelay) {
        // Elephant detected, display alert message
        display.clearDisplay();
        display.setCursor(5, 0);
        display.print("ALERT MODE");
        display.setCursor(10, 20);
        display.print("ELEPHANT");
        display.setCursor(10, 40);
        display.print("DETECTED");
        display.display();
        delay(2000);

        // Ask user to fire after 1 second
        display.clearDisplay();
        display.setTextSize(4); 
        display.setCursor(6, 20);
        display.print("FIRE?");
        display.display();
        display.setTextSize(2); 
        // Wait for user input to fire
        unsigned long startTime = millis();
        while (millis() - startTime < 5000) {  // Wait for 5 seconds for user input
          if (Serial.available() > 0) {
            receivedChar = Serial.read();
            if (receivedChar == '2') {
              // User initiated firing, start countdown
              display.clearDisplay();
              display.setCursor(10, 20);
              display.print("FIRING IN");
              display.display();
              delay(1000);

              // Countdown loop
              for (int i = 5; i > 0; i--) {
                display.setTextSize(5); 
                display.clearDisplay();
                display.setCursor(50, 20);
                display.print(i);
                display.display();
                delay(1000); // Delay for 1 second
              }

              // Display "DONE" after countdown
              display.clearDisplay();
              display.setTextSize(2); 
              display.setCursor(40, 20);
              display.print("DONE");
              display.display();
              
              // Exit loop and continue with the rest of the code
              lastUpdateTime = millis();
              break;
            } else if (receivedChar == '3') {
              // User canceled firing, display safe mode message and exit
              display.clearDisplay();
              display.setCursor(10, 20);
              display.print("CANCELLED");
              display.display();
              delay(1000);
              display.clearDisplay();
              display.setCursor(10, 0);
              display.print("SAFE MODE");
              display.setCursor(10, 20);
              display.print("PROPERTY");
              display.setCursor(10, 40);
              display.print("PROTECTED");
              display.display();
              lastUpdateTime = millis();
              break;
            }
          }
        }

        // If no firing initiated or canceled, continue with the rest of the code
        lastUpdateTime = millis();
      }
    } 
    else if (receivedChar == '0') {
      if (millis() - lastUpdateTime >= debounceDelay) {
        // Safe mode, property protected message
        display.clearDisplay();
        display.setCursor(10, 0);
        display.print("SAFE MODE");
        display.setCursor(10, 20);
        display.print("PROPERTY");
        display.setCursor(10, 40);
        display.print("PROTECTED");
        display.display();
        lastUpdateTime = millis();
      }
    }
  }
}
