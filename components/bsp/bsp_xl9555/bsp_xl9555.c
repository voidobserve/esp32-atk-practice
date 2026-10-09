#include "bsp_xl9555.h"
#include "driver/i2c.h"
#include "i2c.h"

static esp_err_t bsp_xl9555_read(uint8_t reg_addr, uint8_t *data, size_t len);
static esp_err_t bsp_xl9555_write(uint8_t reg_addr, uint8_t *data, size_t len);

void bsp_xl9555_init(void)
{
    gpio_config_t gpio_config_param = {0};

    i2c_bus_init(XL9555_I2C_BUS_IDX);

    gpio_config_param.intr_type = GPIO_INTR_DISABLE;
    gpio_config_param.mode = GPIO_MODE_INPUT;
    gpio_config_param.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config_param.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config_param.pin_bit_mask = (uint64_t)0x01 << XL9555_INT_PIN;
    gpio_config(&gpio_config_param);
}

/**
 * @brief
 *
 * @param reg_addr
 * @param data
 * @param len
 * @return esp_err_t
 */
static esp_err_t bsp_xl9555_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    // 传入要读取的寄存器地址，再读取数据
    return i2c_master_write_read_device(
        XL9555_I2C_BUS_IDX, XL9555_ADDR, &reg_addr, 1, data, len,
        XL9555_I2C_BUS_TIMEOUT_MS / portTICK_PERIOD_MS);
}

/**
 * @brief
 *
 * @param reg_addr
 * @param data
 * @param len
 */
static esp_err_t bsp_xl9555_write(uint8_t reg_addr, uint8_t *data, size_t len)
{
    esp_err_t err = ESP_OK;
    i2c_cmd_handle_t handle = NULL;
    handle = i2c_cmd_link_create();
    if (handle == NULL) {
        return ESP_FAIL;
    }

    err = i2c_master_start(handle);
    if (err != ESP_OK) {
        goto end;
    }

    // 写操作，写入器件地址
    err = i2c_master_write_byte(handle, XL9555_ADDR << 1 | I2C_MASTER_WRITE,
                                true);
    if (err != ESP_OK) {
        goto end;
    }

    // 写操作，写入寄存器地址
    err = i2c_master_write_byte(handle, reg_addr, true);
    if (err != ESP_OK) {
        goto end;
    }

    // 写操作，写入数据
    err = i2c_master_write(handle, data, len, true);
    if (err != ESP_OK) {
        goto end;
    }

    err = i2c_master_stop(handle);
    if (err != ESP_OK) {
        goto end;
    }

    err = i2c_master_cmd_begin(XL9555_I2C_BUS_IDX, handle,
                               XL9555_I2C_BUS_TIMEOUT_MS / portTICK_PERIOD_MS);

end:
    i2c_cmd_link_delete(handle);
    return err;
}

// USER_TO_DO 未确定是否使用
int bsp_xl9555_get_int(void)
{
    return gpio_get_level(GPIO_NUM_40);
}

/**
 * @brief 设置某个io的输入输出方向
 *
 * @param pin 引脚掩码，取值见 XL9555_IO_xxx
 * @param dir 1: 输入 0: 输出
 */
void bsp_xl9555_set_pin_dir(uint16_t pin, uint8_t dir)
{
    uint8_t reg_addr = 0; // 寄存器地址
    uint8_t bit_mask = 0; // 引脚在该端口寄存器中的位掩码
    uint8_t reg_val = 0;  // 寄存器原值

    if (pin <= ((uint16_t)0x01 << 7)) {
        reg_addr = XL9555_CMD_REG_CONFIG_PORT_0;
        bit_mask = (uint8_t)pin;
    } else {
        reg_addr = XL9555_CMD_REG_CONFIG_PORT_1;
        bit_mask = (uint8_t)(pin >> 8);
    }

    // 先读出原值，避免改写其他引脚的方向
    if (bsp_xl9555_read(reg_addr, &reg_val, 1) != ESP_OK) {
        return;
    }

    // XL9555配置寄存器：对应位为1是输入，为0是输出
    if (dir) {
        reg_val |= bit_mask;
    } else {
        reg_val &= (uint8_t)~bit_mask;
    }

    bsp_xl9555_write(reg_addr, &reg_val, 1);
}

/**
 * @brief 控制某个io的电平
 *
 * @param pin 引脚掩码，取值见 XL9555_IO_xxx
 * @param lev 1: 高电平 0: 低电平
 */
void bsp_xl9555_set_pin_lev(uint16_t pin, uint8_t lev)
{
    uint8_t reg_addr = 0; // 寄存器地址
    uint8_t bit_mask = 0; // 引脚在该端口寄存器中的位掩码
    uint8_t reg_val = 0;  // 寄存器原值

    if (pin <= ((uint16_t)0x01 << 7)) {
        reg_addr = XL9555_CMD_REG_OUTPUT_PORT_0;
        bit_mask = (uint8_t)pin;
    } else {
        reg_addr = XL9555_CMD_REG_OUTPUT_PORT_1;
        bit_mask = (uint8_t)(pin >> 8);
    }

    // 先读出输出寄存器原值，避免改写其他引脚的电平
    if (bsp_xl9555_read(reg_addr, &reg_val, 1) != ESP_OK) {
        return;
    }

    // XL9555输出寄存器：对应位为1输出高电平，为0输出低电平
    if (lev) {
        reg_val |= bit_mask;
    } else {
        reg_val &= (uint8_t)~bit_mask;
    }

    bsp_xl9555_write(reg_addr, &reg_val, 1);
}

/**
 * @brief 获取某个引脚的电平
 *
 * @param pin 引脚掩码，取值见 XL9555_IO_xxx
 * @param lev 读取到的电平，1: 高电平 0: 低电平
 */
void bsp_xl9555_get_pin_lev(uint16_t pin, uint8_t *lev)
{
    uint8_t reg_addr = 0; // 寄存器地址
    uint8_t bit_mask = 0; // 引脚在该端口寄存器中的位掩码
    uint8_t reg_val = 0;  // 寄存器原值

    if (lev == NULL) {
        return;
    }

    if (pin <= ((uint16_t)0x01 << 7)) {
        reg_addr = XL9555_CMD_REG_INPUT_PORT_0;
        bit_mask = (uint8_t)pin;
    } else {
        reg_addr = XL9555_CMD_REG_INPUT_PORT_1;
        bit_mask = (uint8_t)(pin >> 8);
    }

    // 读取失败时返回确定值，避免调用方使用未初始化数据
    if (bsp_xl9555_read(reg_addr, &reg_val, 1) != ESP_OK) {
        *lev = 0;
        return;
    }

    *lev = (reg_val & bit_mask) ? 1 : 0;
}
