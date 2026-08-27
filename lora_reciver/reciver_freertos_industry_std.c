#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
// #include "lora.h" // Requires a native ESP-IDF LoRa component, not the Arduino one

#define RELAY1_PIN GPIO_NUM_5
#define RELAY2_PIN GPIO_NUM_6
#define RELAY3_PIN GPIO_NUM_7

static const char *TAG = "WILDLIFE_RX";
QueueHandle_t commandQueue;

void lora_receive_task(void *pvParameters) {
    uint8_t buf[256];
    for (;;) {
        // Native LoRa receive logic (component dependent)
        // int x = lora_receive_packet(buf, sizeof(buf));
        
        // Simulated parsing for example
        char received_cmd = 'W'; 
        
        if (received_cmd) {
            xQueueSend(commandQueue, &received_cmd, pdMS_TO_TICKS(10));
        }
        
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void relay_control_task(void *pvParameters) {
    char cmd;
    for (;;) {
        if (xQueueReceive(commandQueue, &cmd, portMAX_DELAY) == pdPASS) {
            ESP_LOGI(TAG, "Received Command: %c", cmd);

            if (cmd == 'W') {
                gpio_set_level(RELAY1_PIN, 0); // Active Low
                ESP_LOGI(TAG, "Relay 1 ON");
                vTaskDelay(pdMS_TO_TICKS(500));
                
                gpio_set_level(RELAY1_PIN, 1);
                ESP_LOGI(TAG, "Relay 1 OFF");
            }
            // ... (Additional relay logic follows same pattern)
        }
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Initializing LoRa Receiver...");

    // Configure GPIOs without Arduino wrappers
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << RELAY1_PIN) | (1ULL << RELAY2_PIN) | (1ULL << RELAY3_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    // Set initial states (Active Low relays)
    gpio_set_level(RELAY1_PIN, 1);
    gpio_set_level(RELAY2_PIN, 1);
    gpio_set_level(RELAY3_PIN, 1);

    commandQueue = xQueueCreate(5, sizeof(char));

    if (commandQueue != NULL) {
        xTaskCreatePinnedToCore(lora_receive_task, "LoRa_RX", 4096, NULL, 5, NULL, 1);
        xTaskCreatePinnedToCore(relay_control_task, "Relay_CTRL", 4096, NULL, 4, NULL, 0);
    }
}
