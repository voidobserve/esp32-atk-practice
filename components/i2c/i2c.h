#ifndef __I2C_H__
#define __I2C_H__

#include "driver/gpio.h"

// i2c总线接口编号
enum
{
    I2C_BUS_0 = 0,
    I2C_BUS_1,
    I2C_BUS_MAX, // 2c总线接口数量
};
typedef uint8_t i2c_bus_idx_t;

/*
    由于
*/
typedef struct
{
    uint8_t *buf;
    size_t len;
} i2c_buf_t;

#define I2C_BUS_0_SCL_IO     GPIO_NUM_42
#define I2C_BUS_0_SDA_IO     GPIO_NUM_41
#define I2C_BUS_0_FREQ_HZ    ((uint32_t)400 * 1000) // 400K
#define I2C_BUS_0_TIMEOUT_MS ((uint32_t)1000)       //

#define I2C_BUS_1_SCL_IO     -1
#define I2C_BUS_1_SDA_IO     -1
#define I2C_BUS_1_FREQ_HZ    ((uint32_t)400 * 1000) // 400K
#define I2C_BUS_1_TIMEOUT_MS ((uint32_t)1000)       //

esp_err_t i2c_bus_init(uint8_t bus_idx);

#endif
