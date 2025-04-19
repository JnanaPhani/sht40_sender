#include "sht40.h"
#include "i2c.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

#define SHT40_ADDR                  0x44
#define SHT40_MEASURE_HIGH_PRECISION 0xFD

esp_err_t sht40_init(void) {
    i2c_master_init();
    return ESP_OK;
}

esp_err_t sht40_read(float *temperature, float *humidity) {
    uint8_t cmd = SHT40_MEASURE_HIGH_PRECISION;
    uint8_t data[6];

    esp_err_t ret = i2c_master_write_to_device(I2C_MASTER_NUM, SHT40_ADDR,
                                               &cmd, 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    if (ret != ESP_OK) return ret;

    vTaskDelay(pdMS_TO_TICKS(10)); // Wait for measurement

    ret = i2c_master_read_from_device(I2C_MASTER_NUM, SHT40_ADDR,
                                      data, sizeof(data), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    if (ret != ESP_OK) return ret;

    uint16_t raw_temp = (data[0] << 8) | data[1];
    uint16_t raw_hum = (data[3] << 8) | data[4];

    *temperature = -45 + 175 * ((float)raw_temp / 65535);
    *humidity = 100 * ((float)raw_hum / 65535);

    return ESP_OK;
}
