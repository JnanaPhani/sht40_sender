#ifndef I2C_H_
#define I2C_H_

#include "driver/i2c.h"

#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_SCL_IO           5     // D5
#define I2C_MASTER_SDA_IO           4     // D4
#define I2C_MASTER_FREQ_HZ          100000
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0
#define I2C_MASTER_TIMEOUT_MS       1000

void i2c_master_init(void);

#endif // I2C_H_
