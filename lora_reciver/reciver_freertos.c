#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

// Pin definitions (Ensure these map correctly to your ESP32/microcontroller pins)
#define SS_PIN 10
#define RST_PIN 9
#define DIO0_PIN 2
#define LED_PIN 3  

#define RELAY1_PIN 5  
#define RELAY2_PIN 6  
#define RELAY3_PIN 7  

// FreeRTOS Queue to pass commands between tasks
QueueHandle_t commandQueue;

// Task prototypes
void loraReceiveTask(void *pvParameters);
void relayControlTask(void *pvParameters);

void setup() {
  Serial.begin(9600);
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, HIGH);
  digitalWrite(RELAY3_PIN, HIGH);

  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);

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

  // Create a queue capable of holding 5 characters
  commandQueue = xQueueCreate(5, sizeof(char));

  if (commandQueue != NULL) {
    // Core 1 (Application Core) for LoRa listening
    xTaskCreatePinnedToCore(
      loraReceiveTask,   "LoRa_RX", 
      4096,              NULL, 
      1,                 NULL, 
      1                  
    );

    // Core 0 (Pro Core) for Relay Execution
    xTaskCreatePinnedToCore(
      relayControlTask,  "Relay_CTRL", 
      4096,              NULL, 
      2,                 NULL, 
      0                  
    );
  }
}

void loraReceiveTask(void *pvParameters) {
  for (;;) {
    int packetSize = LoRa.parsePacket();
    if (packetSize > 0) {
      String received = "";
      while (LoRa.available()) {
        received += (char)LoRa.read();
      }

      if (received.startsWith("MYPROJ:") && received.length() == 8) {
        char command = received.charAt(7);
        // Send command to the queue, wait 10 ticks if full
        xQueueSend(commandQueue, &command, pdMS_TO_TICKS(10));
      }
    }
    // Yield to avoid starving the watchdog timer
    vTaskDelay(pdMS_TO_TICKS(20)); 
  }
}

void relayControlTask(void *pvParameters) {
  char cmd;
  for (;;) {
    // Block indefinitely until a command is received in the queue
    if (xQueueReceive(commandQueue, &cmd, portMAX_DELAY) == pdPASS) {
      Serial.print("Received Command: ");
      Serial.println(cmd);

      if (cmd == 'W') {
        digitalWrite(RELAY1_PIN, LOW);
        Serial.println("Relay 1 ON");
        vTaskDelay(pdMS_TO_TICKS(500)); 
        digitalWrite(RELAY1_PIN, HIGH);
        Serial.println("Relay 1 OFF");
      } 
      else if (cmd == 'X') {
        digitalWrite(RELAY2_PIN, LOW);
        Serial.println("Relay 2 ON");
        vTaskDelay(pdMS_TO_TICKS(500));
        digitalWrite(RELAY2_PIN, HIGH);
        Serial.println("Relay 2 OFF");
      } 
      else if (cmd == 'Z') {
        digitalWrite(RELAY3_PIN, LOW);
        Serial.println("Relay 3 ON");
        vTaskDelay(pdMS_TO_TICKS(500));
        digitalWrite(RELAY3_PIN, HIGH);
        Serial.println("Relay 3 OFF");
      } 
      else {
        Serial.println("Executing Default Task...");
        
        digitalWrite(RELAY1_PIN, LOW);
        Serial.println("Relay 1 ON");
        vTaskDelay(pdMS_TO_TICKS(5000)); 
        digitalWrite(RELAY1_PIN, HIGH);
        Serial.println("Relay 1 OFF");

        digitalWrite(RELAY2_PIN, LOW);
        Serial.println("Relay 2 ON");
        vTaskDelay(pdMS_TO_TICKS(7000));
        digitalWrite(RELAY2_PIN, HIGH);
        Serial.println("Relay 2 OFF");

        digitalWrite(RELAY3_PIN, LOW);
        Serial.println("Relay 3 ON");
        vTaskDelay(pdMS_TO_TICKS(2000));
        digitalWrite(RELAY3_PIN, HIGH);
        Serial.println("Relay 3 OFF");
      }

      // Flash LED to indicate completion
      digitalWrite(LED_PIN, HIGH);
      vTaskDelay(pdMS_TO_TICKS(500));
      digitalWrite(LED_PIN, LOW);
    }
  }
}

void loop() {
  // Empty loop: FreeRTOS tasks handle the logic. 
  vTaskDelete(NULL); // Deletes the default Arduino loop task to free memory
}
