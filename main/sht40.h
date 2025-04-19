#ifndef SHT40_H_
#define SHT40_H_
#include "esp_err.h"

esp_err_t sht40_init(void);
esp_err_t sht40_read(float *temperature, float *humidity);

#endif // SHT40_H_
