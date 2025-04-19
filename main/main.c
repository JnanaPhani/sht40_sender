#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sht40.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_spp_api.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include <string.h>
#define SPP_TAG "SPP_TEMP"
#define DEVICE_NAME "ESP32_BT_TEMP"
#include "driver/gpio.h"

#define LED_GPIO GPIO_NUM_2
static bool bt_connected = false;

// static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;
static uint32_t spp_handle = 0;

static const char *TAG = "MAIN";

void blink_led(int times)
{
    for (int i = 0; i < times; i++) {
        gpio_set_level(LED_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_set_level(LED_GPIO, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void led_disconnected_pattern() {
    // Long pulse
    gpio_set_level(LED_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_set_level(LED_GPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(200));

    // 4 quick flickers
    for (int i = 0; i < 4; i++) {
        gpio_set_level(LED_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_set_level(LED_GPIO, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // Long pulse
    gpio_set_level(LED_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_set_level(LED_GPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(200));
    
}

void spp_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    switch (event) {
        case ESP_SPP_INIT_EVT:
            ESP_LOGI(SPP_TAG, "SPP initialized");
            esp_bt_gap_set_device_name(DEVICE_NAME);

            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);

            esp_spp_start_srv(ESP_SPP_SEC_NONE, ESP_SPP_ROLE_SLAVE, 0, "SPP_SERVER");
            break;

        case ESP_SPP_START_EVT:
            ESP_LOGI(SPP_TAG, "SPP server started");
            break;

        case ESP_SPP_SRV_OPEN_EVT:
            ESP_LOGI(SPP_TAG, "Client connected");
            bt_connected = true;
            blink_led(5);
            spp_handle = param->srv_open.handle;
            break;
        case ESP_SPP_CLOSE_EVT:
            ESP_LOGI(TAG, "Bluetooth device disconnected");
            bt_connected = false;
            led_disconnected_pattern();
            break;
        default:
            break;
    }
}

void bt_init() {
    esp_err_t ret;

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(SPP_TAG, "Bluetooth controller init failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BTDM);

    if (ret) {
        ESP_LOGE(SPP_TAG, "Bluetooth controller enable failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(SPP_TAG, "Bluedroid init failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(SPP_TAG, "Bluedroid enable failed: %s", esp_err_to_name(ret));
        return;
    }

    esp_spp_register_callback(spp_callback);

    esp_spp_cfg_t bt_spp_cfg = {
        .mode = ESP_SPP_MODE_CB,
        .enable_l2cap_ertm = false,
        .tx_buffer_size = 0
    };

    ret = esp_spp_enhanced_init(&bt_spp_cfg);
    if (ret) {
        ESP_LOGE(SPP_TAG, "SPP init failed: %s", esp_err_to_name(ret));
        return;
    }

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    
    gpio_set_level(LED_GPIO, 0); // LED off initially

    ESP_LOGI(TAG, "Bluetooth initialized");
    blink_led(10);

    
}

void app_main(void) {
    sht40_init();

    // Initialize NVS (for Wi-Fi)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize Bluetooth
    bt_init();

    vTaskDelay(pdMS_TO_TICKS(2000)); // Optional wait

    float temp, hum;
    char bt_data[64];

    while (1) {
        if (sht40_read(&temp, &hum) == ESP_OK) {
            printf("Temperature: %.2f °C, Humidity: %.2f %%\n", temp, hum);
            snprintf(bt_data, sizeof(bt_data), "Temp: %.2f C, Hum: %.2f %%\r\n", temp, hum);

            // Send over Bluetooth if connected
            if (spp_handle != 0) {
                // Blink twice
                if (bt_connected) {
                    blink_led(2);  // short 2 blinks when data sent
                }
                esp_spp_write(spp_handle, strlen(bt_data), (uint8_t *)bt_data);
            }
        } else {
            printf("Failed to read from SHT40\n");
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
