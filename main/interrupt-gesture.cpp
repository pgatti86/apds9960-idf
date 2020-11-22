#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "APDS9960.h"

#define ESP_INTR_FLAG_DEFAULT 0

#define INTERRUPT_GPIO 18

static const char *TAG = "Interrupt Sample";

static xQueueHandle gpio_evt_queue = NULL;

SparkFun_APDS9960 apds = SparkFun_APDS9960();

void IRAM_ATTR gesture_isr_handler(void* arg) {
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void log_task(void *arg) {

    uint32_t io_num;

    while(1) {
         
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            
            if (apds.isGestureAvailable() ) {
                switch ( apds.readGesture() ) {
                    case DIR_UP:
                        ESP_LOGI(TAG, "UP");
                        break;
                    case DIR_DOWN:
                        ESP_LOGI(TAG, "DOWN");
                        break;
                    case DIR_LEFT:
                        ESP_LOGI(TAG, "LEFT");
                        break;
                    case DIR_RIGHT:
                        ESP_LOGI(TAG, "RIGHT");
                        break;
                    case DIR_NEAR:
                        ESP_LOGI(TAG, "NEAR");
                        break;
                    case DIR_FAR:
                        ESP_LOGI(TAG, "FAR");
                        break;
                    default:
                        ESP_LOGI(TAG, "NONE");
                }
            }
        }
    }
}

extern "C" void app_main() {
    
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    gpio_num_t interrupt_pin = (gpio_num_t)INTERRUPT_GPIO;
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.pin_bit_mask = (1ULL<<INTERRUPT_GPIO);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = (gpio_pullup_t)0;
    io_conf.pull_down_en = (gpio_pulldown_t)0;
    gpio_config(&io_conf);
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(interrupt_pin, gesture_isr_handler, NULL);  

    ESP_LOGI(TAG, "Configuring event group");  
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(log_task, "log_task", 2048, NULL, 5, NULL);

    if (apds.init() ) {
        ESP_LOGI(TAG, "APDS-9960 initialization complete");
    } else {
        ESP_LOGI(TAG, "Something went wrong during APDS-9960 init!");
    }

    // Start running the APDS-9960 gesture sensor engine
    ESP_LOGI(TAG, "---ENABLING GESTURE---"); 
    if (apds.enableGestureSensor(true) ) {
        ESP_LOGI(TAG, "Gesture sensor is now running");
    } else {
        ESP_LOGI(TAG, "Something went wrong during gesture sensor init!");
    }

    while(1) {
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}



