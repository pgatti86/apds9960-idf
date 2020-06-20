/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"

#include "SparkFun_APDS9960.h"

static const char * TAG = "MAIN";

void gestureTest(SparkFun_APDS9960 &apds);

extern "C" void app_main()
{
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


    SparkFun_APDS9960 apds = SparkFun_APDS9960();
    if (apds.init() ) {
        ESP_LOGI(TAG, "APDS-9960 initialization complete");
    } else {
        ESP_LOGI(TAG, "Something went wrong during APDS-9960 init!");
    }

    gestureTest(apds);
}

void gestureTest(SparkFun_APDS9960 &apds) {
    
  // Start running the APDS-9960 gesture sensor engine
  if (apds.enableGestureSensor(false) ) {
    ESP_LOGI(TAG, "Gesture sensor is now running");
  } else {
    ESP_LOGI(TAG, "Something went wrong during gesture sensor init!");
  }

  while(1) {
      if ( apds.isGestureAvailable() ) {
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
      vTaskDelay(250 / portTICK_PERIOD_MS);
    }
}

void proximitySensing(SparkFun_APDS9960 &apds) {
    
    // Adjust the Proximity sensor gain
    if ( !apds.setProximityGain(PGAIN_2X) ) {
        ESP_LOGI(TAG, "Something went wrong trying to set PGAIN");
    }
  
    // Start running the APDS-9960 proximity sensor (no interrupts)
    if ( apds.enableProximitySensor(false) ) {
        ESP_LOGI(TAG, "Proximity sensor is now running");
    } else {
        ESP_LOGI(TAG, "Something went wrong during sensor init!");
    }

    uint8_t proximity_data = 0;
    while(1) {
        if ( !apds.readProximity(proximity_data) ) {
             ESP_LOGI(TAG, "Error reading proximity value");
        } else {
             ESP_LOGI(TAG, "Proximity: %d", proximity_data);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void colorSensing(SparkFun_APDS9960 &apds) {
    
    uint16_t ambient_light = 0;
    uint16_t red_light = 0;
    uint16_t green_light = 0;
    uint16_t blue_light = 0;

    if (apds.init() ) {
        ESP_LOGI(TAG, "APDS-9960 initialization complete");
    } else {
        ESP_LOGI(TAG, "Something went wrong during APDS-9960 init!");
    }

    // Start running the APDS-9960 light sensor (no interrupts)
    if ( apds.enableLightSensor(false) ) {
        ESP_LOGI(TAG, "Light sensor is now running");
    } else {
        ESP_LOGI(TAG, "Something went wrong during light sensor init!");
    }

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    while(1) {
         // Read the light levels (ambient, red, green, blue)
        if (  !apds.readAmbientLight(ambient_light) ||
                !apds.readRedLight(red_light) ||
                !apds.readGreenLight(green_light) ||
                !apds.readBlueLight(blue_light) ) {
            ESP_LOGI(TAG, "Error reading light values");
        } else {
            ESP_LOGI(TAG, "Ambient: %i, Red; %i, Green: %i, Blue: %i", ambient_light, red_light, green_light, blue_light);
        }

        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
