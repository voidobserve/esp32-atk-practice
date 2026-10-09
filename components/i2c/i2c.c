#include "i2c.h"
#include "driver/i2c.h"

esp_err_t i2c_bus_init(i2c_bus_idx_t bus_idx)
{
    // if ()
    i2c_config_t i2c_config_param = {0};
    i2c_config_param.mode = I2C_MODE_MASTER;

    if (bus_idx == I2C_BUS_0) {
        i2c_config_param.sda_io_num = I2C_BUS_0_SDA_IO;
        i2c_config_param.scl_io_num = I2C_BUS_0_SCL_IO;
        i2c_config_param.master.clk_speed = I2C_BUS_0_FREQ_HZ;
    } else if (bus_idx == I2C_BUS_1) {
        i2c_config_param.sda_io_num = I2C_BUS_1_SDA_IO;
        i2c_config_param.scl_io_num = I2C_BUS_1_SCL_IO;
        i2c_config_param.master.clk_speed = I2C_BUS_1_FREQ_HZ;
    } else {
        return ESP_ERR_INVALID_ARG;
    }

    i2c_config_param.sda_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_config_param.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_param_config(bus_idx, &i2c_config_param);
    return i2c_driver_install(bus_idx, i2c_config_param.mode, 0, 0, 0);
}

// esp_err_t i2c_bus_transfer(i2c_bus_idx_t bus_idx, uint8_t addr, uint8_t *data,
//                            size_t size)
// {
// }
