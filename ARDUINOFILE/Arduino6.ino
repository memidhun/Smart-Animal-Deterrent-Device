// THIS CODE USES BUTTON THAN SERIAL INPUT 

#include <Adafruit_GFX.h>
#include "Adafruit_SH1106.h"

#define OLED_RESET 4
Adafruit_SH1106 display(OLED_RESET);

char receivedChar;
unsigned long lastUpdateTime = 0;
const unsigned long debounceDelay = 200; // Adjust debounce delay as needed

// Define the pin numbers for the buttons
const int buttonPin1 = 2; // Button 1 connected to pin 2
const int buttonPin2 = 3; // Button 2 connected to pin 3

void setup() {
  Serial.begin(9600);
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.display();
  display.clearDisplay();
  display.setTextSize(2);  
  display.setTextColor(WHITE);

  // Set the button pins as inputs
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
}

void displayAlert() {
  display.clearDisplay();
  display.setCursor(5, 0);
  display.println("ALERT MODE");
  display.setCursor(10, 20);
  display.println("ELEPHANT");
  display.setCursor(10, 40);
  display.println("DETECTED");
  display.display();
  delay(2000);
}

void askToFire() {
  display.clearDisplay();
  display.setTextSize(4); 
  display.setCursor(6, 20);
  display.println("FIRE?");
  display.display();
  display.setTextSize(2); 
}

void countdownAndDone() {
  display.clearDisplay();
  display.setCursor(10, 20);
  display.println("FIRING IN");
  display.display();
  delay(1000);

  // Countdown loop
  for (int i = 5; i > 0; i--) {
    display.setTextSize(5); 
    display.clearDisplay();
    display.setCursor(50, 20);
    display.println(i);
    display.display();
    delay(1000); // Delay for 1 second
  }

  // Display "DONE" after countdown
  display.clearDisplay();
  display.setTextSize(2); 
  display.setCursor(40, 20);
  display.println("DONE");
  display.display();
  delay(1000);
}

void displayCancelled() {
  display.clearDisplay();
  display.setCursor(10, 20);
  display.println("CANCELLED");
  display.display();
  delay(1000);
}

void displaySafeMode() {
  display.clearDisplay();
  display.setCursor(10, 0);
  display.println("SAFE MODE");
  display.setCursor(10, 20);
  display.println("PROPERTY");
  display.setCursor(10, 40);
  display.println("PROTECTED");
  display.display();
}

void loop() {
  // Check for serial data continuously
  while (Serial.available() > 0) {
    receivedChar = Serial.read();
    if (receivedChar == '1') {
      if (millis() - lastUpdateTime >= debounceDelay) {
        displayAlert();
        askToFire();

        // Wait for button press to fire
        unsigned long startTime = millis();
        while (millis() - startTime < 5000) {  // Wait for 5 seconds for button press
          if (digitalRead(buttonPin1) == LOW) {
            // Button 1 pressed, start countdown
            countdownAndDone();
            lastUpdateTime = millis();
            break;
          } else if (digitalRead(buttonPin2) == LOW) {
            // Button 2 pressed, display cancelled message and exit
            displayCancelled();
            displaySafeMode();
            lastUpdateTime = millis();
            break;
          }
        }

        // If no button press, continue with the rest of the code
        displaySafeMode();
        lastUpdateTime = millis();
      }
    } 
    else if (receivedChar == '0') {
      if (millis() - lastUpdateTime >= debounceDelay) {
        // Safe mode, property protected message
        displaySafeMode();
        lastUpdateTime = millis();
      }
    }
  }
}
