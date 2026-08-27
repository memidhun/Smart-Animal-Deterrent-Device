#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
// #include "lora.h" // Requires an ESP-IDF native LoRa C component

// Updated LoRa Pin Definitions safe for TFT integration
#define LORA_SS_PIN    GPIO_NUM_5   //
#define LORA_RST_PIN   GPIO_NUM_27  // Moved from 14
#define LORA_DIO0_PIN  GPIO_NUM_26  // Moved from 2

// Standard ESP32 UART0 for USB Serial Monitor
#define UART_NUM       UART_NUM_0
#define BUF_SIZE       1024

static const char *TAG = "WILDLIFE_TX";

void lora_tx_task(void *pvParameters) {
    uint8_t data[BUF_SIZE];
    
    for (;;) {
        // Non-blocking FreeRTOS UART read
        int len = uart_read_bytes(UART_NUM, data, (BUF_SIZE - 1), pdMS_TO_TICKS(100));
        
        if (len > 0) {
            data[len] = '\0'; 
            
            // Replicates input.trim() and isolates the first valid character[cite: 5]
            char inputChar = '\0';
            for (int i = 0; i < len; i++) {
                if (isalpha(data[i])) {
                    inputChar = toupper(data[i]);
                    break;
                }
            }

            // Validating against accepted project targets[cite: 5]
            if (inputChar == 'H' || inputChar == 'E' || inputChar == 'M' ||
                inputChar == 'T' || inputChar == 'B' || inputChar == 'S') { 
                
                char payload[16];
                snprintf(payload, sizeof(payload), "MYPROJ:%c", inputChar); //[cite: 5]
                
                /* Native LoRa TX Execution (Depends on specific ESP-IDF LoRa component)
                   lora_send_packet((uint8_t*)payload, strlen(payload)); 
                */
                
                ESP_LOGI(TAG, "Sent: %c", inputChar); //[cite: 5]
                vTaskDelay(pdMS_TO_TICKS(50)); // Allow TX completion[cite: 5]
            } else if (inputChar != '\0') {
                ESP_LOGW(TAG, "Invalid character ignored: %c", inputChar); //[cite: 5]
            }
        }
        // Yield to the FreeRTOS scheduler
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Initializing LoRa Transmitter...");

    // 1. Hardware UART Configuration (Replaces Serial.begin)[cite: 5]
    uart_config_t uart_config = {
        .baud_rate = 115200, 
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM, &uart_config);

    // 2. SPI & LoRa Initialization 
    // (This block requires a native ESP-IDF C library for the SX1278 to replace LoRa.h)
    /*
        spi_bus_config_t buscfg = {
            .miso_io_num = GPIO_NUM_19, // SPI MISO[cite: 5]
            .mosi_io_num = GPIO_NUM_23, // SPI MOSI[cite: 5]
            .sclk_io_num = GPIO_NUM_18, // SPI Clock[cite: 5]
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 0
        };
        spi_bus_initialize(VSPI_HOST, &buscfg, 1);
        
        lora_init();
        lora_set_pins(LORA_SS_PIN, LORA_RST_PIN, LORA_DIO0_PIN); //[cite: 5]
        lora_set_frequency(433e6); //[cite: 5]
        lora_set_sync_word(0xF3);  //[cite: 5]
        lora_set_tx_power(20);     // PA_OUTPUT_PA_BOOST_PIN equivalent[cite: 5]
        lora_set_spreading_factor(7); //[cite: 5]
        lora_set_bandwidth(250e3); //[cite: 5]
        lora_set_coding_rate(5);   //[cite: 5]
    */

    ESP_LOGI(TAG, "LoRa Transmitter Ready. Type H, E, M, T, B, or S."); //[cite: 5]

    // 3. Launch the dedicated UART/TX task on Core 1
    xTaskCreatePinnedToCore(
        lora_tx_task, 
        "LoRa_TX", 
        4096, 
        NULL, 
        5, 
        NULL, 
        1
    );
}
