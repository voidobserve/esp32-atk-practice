#include "bsp_xl9555.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "i2c.h"

static const char *TAG = "bsp_xl9555";

static esp_err_t bsp_xl9555_read(uint8_t reg_addr, uint8_t *data, size_t len);
static esp_err_t bsp_xl9555_write(uint8_t reg_addr, uint8_t *data, size_t len);
static esp_err_t bsp_xl9555_read_u16(uint8_t reg_addr, uint16_t pin_mask,
                                     uint16_t *val_mask);

/**
 * @brief 初始化 XL9555
 *
 * @return 初始化结果，任一步骤失败都会返回对应的错误码
 */
esp_err_t bsp_xl9555_init(void)
{
    // XL9555 的输入寄存器 0、1 连续，用于上电后读取两个端口并清除中断状态。
    uint8_t buf[2];
    gpio_config_t gpio_config_param = {0};
    esp_err_t err;

    // 初始化连接 XL9555 的 I2C 总线。
    err = i2c_bus_init(XL9555_I2C_BUS_IDX);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "i2c bus init failed: %s", esp_err_to_name(err));
        return err;
    }

    // 配置 XL9555 的中断输出引脚为输入上拉，暂不启用 ESP32 GPIO 中断。
    gpio_config_param.intr_type = GPIO_INTR_DISABLE;
    gpio_config_param.mode = GPIO_MODE_INPUT;
    gpio_config_param.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config_param.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config_param.pin_bit_mask = (uint64_t)0x01 << XL9555_INT_PIN;
    err = gpio_config(&gpio_config_param);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "int pin gpio config failed: %s", esp_err_to_name(err));
        return err;
    }

    // 读取两个输入端口，清除 XL9555 上电后可能保留的中断状态。
    // 同时这也是第一次 I2C 访问，可用于确认 XL9555 是否应答。
    err = bsp_xl9555_read(XL9555_CMD_REG_INPUT_PORT_0, buf, sizeof(buf));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "xl9555 not responding at addr 0x%02x: %s",
                 (unsigned)XL9555_ADDR, esp_err_to_name(err));
        return err;
    }

    // 方向掩码按 bit15..bit0 对应 P1.7..P1.0、P0.7..P0.0：
    // 1 表示输入，0 表示输出。0xF003 将 P0.0、P0.1 和 P1.4..P1.7
    // 配置为输入，其余引脚配置为输出。
    err = bsp_xl9555_set_pin_dir(XL9555_IO_MASK_ALL, 0xF003);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "set pin dir failed: %s", esp_err_to_name(err));
        return err;
    }

    // 蜂鸣器脚给高电平，不鸣叫
    err = bsp_xl9555_set_pin_lev_single(XL9555_IO_MASK_BEEP, 1);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "set beep level failed: %s", esp_err_to_name(err));
        return err;
    }

    err = bsp_xl9555_set_pin_lev_single(XL9555_IO_MASK_SPK_EN, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "set spk_en level failed: %s", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}

/**
 * @brief 从 XL9555 的指定寄存器连续读取数据
 *
 * @param reg_addr 起始寄存器地址
 * @param data 接收数据的缓冲区
 * @param len 要读取的字节数
 * @return I2C 读取操作的执行结果
 */
static esp_err_t bsp_xl9555_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    // 先发送寄存器地址，再从同一设备连续读取指定长度的数据。
    return i2c_master_write_read_device(
        XL9555_I2C_BUS_IDX, XL9555_ADDR, &reg_addr, 1, data, len,
        XL9555_I2C_BUS_TIMEOUT_MS / portTICK_PERIOD_MS);
}

/**
 * @brief 从指定寄存器开始，向 XL9555 连续写入数据
 *
 * @param reg_addr 起始寄存器地址
 * @param data 要写入的数据缓冲区
 * @param len 要写入的字节数
 * @return I2C 写入操作的执行结果
 */
static esp_err_t bsp_xl9555_write(uint8_t reg_addr, uint8_t *data, size_t len)
{
    esp_err_t err = ESP_OK;
    i2c_cmd_handle_t handle = NULL;

    // 创建本次 I2C 事务；失败时无法继续构造命令。
    handle = i2c_cmd_link_create();
    if (handle == NULL) {
        return ESP_FAIL;
    }

    err = i2c_master_start(handle);
    if (err != ESP_OK) {
        goto end;
    }

    // START 后发送器件地址和写标志，等待从设备应答。
    err = i2c_master_write_byte(handle, XL9555_ADDR << 1 | I2C_MASTER_WRITE,
                                true);
    if (err != ESP_OK) {
        goto end;
    }

    // 指定要写入的 XL9555 起始寄存器。
    err = i2c_master_write_byte(handle, reg_addr, true);
    if (err != ESP_OK) {
        goto end;
    }

    // 写入一个或多个数据字节。
    err = i2c_master_write(handle, data, len, true);
    if (err != ESP_OK) {
        goto end;
    }

    // 结束 I2C 事务，并提交命令链执行。
    err = i2c_master_stop(handle);
    if (err != ESP_OK) {
        goto end;
    }

    err = i2c_master_cmd_begin(XL9555_I2C_BUS_IDX, handle,
                               XL9555_I2C_BUS_TIMEOUT_MS / portTICK_PERIOD_MS);

end:
    // 无论事务成功与否都释放命令链资源。
    i2c_cmd_link_delete(handle);
    return err;
}

// 获取xl9555的中断引脚电平
int bsp_xl9555_get_int_lev(void)
{
    return gpio_get_level(XL9555_INT_PIN);
}

/**
 * @brief 设置一个或多个io的输入输出方向
 *
 * @param pin_mask 引脚掩码，取值见 XL9555_IO_xxx
 * @param dir_mask 对应位为1: 输入，为0: 输出
 * @return I2C 读写操作的执行结果
 */
esp_err_t bsp_xl9555_set_pin_dir(uint16_t pin_mask, uint16_t dir_mask)
{
    // 配置寄存器低字节对应 P0，高字节对应 P1。
    uint8_t reg_val[2];
    uint16_t current_dir;
    uint16_t new_dir;
    esp_err_t err;

    // 空掩码不选中任何引脚，无需访问 I2C。
    if (pin_mask == 0) {
        return ESP_OK;
    }

    // 连续读取两个配置寄存器，保留未选中引脚当前的方向配置。
    err = bsp_xl9555_read(XL9555_CMD_REG_CONFIG_PORT_0, reg_val,
                          sizeof(reg_val));
    if (err != ESP_OK) {
        return err;
    }

    // 将 P0、P1 两个字节合并成 16 位值；pin_mask 指定需要修改的位，
    // dir_mask 对应位为 1 配置为输入，为 0 配置为输出。
    current_dir = (uint16_t)reg_val[0] | ((uint16_t)reg_val[1] << 8);
    new_dir = (current_dir & (uint16_t)~pin_mask) | (dir_mask & pin_mask);

    // 按 XL9555 寄存器顺序拆成 P0、P1 两个字节并连续写回。
    reg_val[0] = (uint8_t)new_dir;
    reg_val[1] = (uint8_t)(new_dir >> 8);
    return bsp_xl9555_write(XL9555_CMD_REG_CONFIG_PORT_0, reg_val,
                            sizeof(reg_val));
}

/**
 * @brief 控制一个或多个io的输出电平
 *
 * @param pin_mask 引脚掩码，取值见 XL9555_IO_xxx
 * @param lev_mask 对应位为1: 高电平，为0: 低电平
 * @return I2C 读写操作的执行结果
 */
esp_err_t bsp_xl9555_set_pin_lev(uint16_t pin_mask, uint16_t lev_mask)
{
    // 输出寄存器低字节对应 P0，高字节对应 P1。
    uint8_t reg_val[2];
    uint16_t current_lev;
    uint16_t new_lev;
    esp_err_t err;

    // 空掩码不选中任何引脚，无需访问 I2C。
    if (pin_mask == 0) {
        return ESP_OK;
    }

    // 先读取当前输出锁存值，避免改变未选中引脚的输出电平。
    err = bsp_xl9555_read(XL9555_CMD_REG_OUTPUT_PORT_0, reg_val,
                          sizeof(reg_val));
    if (err != ESP_OK) {
        return err;
    }

    // 只更新 pin_mask 选中的位：lev_mask 对应位为 1 输出高电平，为 0 输出低电平。
    current_lev = (uint16_t)reg_val[0] | ((uint16_t)reg_val[1] << 8);
    new_lev = (current_lev & (uint16_t)~pin_mask) | (lev_mask & pin_mask);

    // 将更新后的 16 位电平值拆成 P0、P1 两字节并连续写回。
    reg_val[0] = (uint8_t)new_lev;
    reg_val[1] = (uint8_t)(new_lev >> 8);
    return bsp_xl9555_write(XL9555_CMD_REG_OUTPUT_PORT_0, reg_val,
                            sizeof(reg_val));
}

/**
 * @brief 获取一个或多个io的输入电平
 *
 * @param pin_mask 引脚掩码，取值见 XL9555_IO_xxx
 * @param lev_mask 读取到的电平掩码，对应位为1表示高电平，读取失败时为0
 * @return I2C 读取操作的执行结果
 */
esp_err_t bsp_xl9555_get_pin_lev(uint16_t pin_mask, uint16_t *lev_mask)
{
    return bsp_xl9555_read_u16(XL9555_CMD_REG_INPUT_PORT_0, pin_mask, lev_mask);
}

/**
 * @brief 读回一个或多个io的输入输出方向配置
 *
 * @param pin_mask 引脚掩码，取值见 XL9555_IO_xxx
 * @param dir_mask 读取到的方向掩码，对应位为1表示输入，0表示输出
 * @return I2C 读取操作的执行结果
 */
esp_err_t bsp_xl9555_get_pin_dir(uint16_t pin_mask, uint16_t *dir_mask)
{
    return bsp_xl9555_read_u16(XL9555_CMD_REG_CONFIG_PORT_0, pin_mask,
                               dir_mask);
}

/**
 * @brief 读回一个或多个io的输出锁存电平
 *
 * @param pin_mask 引脚掩码，取值见 XL9555_IO_xxx
 * @param lev_mask 读取到的输出电平掩码，对应位为1表示高电平
 * @return I2C 读取操作的执行结果
 */
esp_err_t bsp_xl9555_get_pin_out_lev(uint16_t pin_mask, uint16_t *lev_mask)
{
    return bsp_xl9555_read_u16(XL9555_CMD_REG_OUTPUT_PORT_0, pin_mask,
                               lev_mask);
}

/**
 * @brief 从相邻的两个端口寄存器读取 16 位值，并屏蔽未请求的引脚
 *
 * @param reg_addr 低字节对应的寄存器地址（P0）
 * @param pin_mask 需要保留的位
 * @param val_mask 读取结果，读取失败时为0
 * @return I2C 读取操作的执行结果
 */
static esp_err_t bsp_xl9555_read_u16(uint8_t reg_addr, uint16_t pin_mask,
                                     uint16_t *val_mask)
{
    // 端口寄存器低字节对应 P0，高字节对应 P1，且地址连续。
    uint8_t reg_val[2];
    esp_err_t err;

    // 输出指针无效时无法返回读取结果。
    if (val_mask == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // 空掩码没有待读取的引脚，返回空结果。
    if (pin_mask == 0) {
        *val_mask = 0;
        return ESP_OK;
    }

    // 连续读取 P0、P1；通信失败时返回确定的空掩码。
    err = bsp_xl9555_read(reg_addr, reg_val, sizeof(reg_val));
    if (err != ESP_OK) {
        *val_mask = 0;
        return err;
    }

    // 合并两个端口的值，并屏蔽调用者未请求的引脚。
    *val_mask = ((uint16_t)reg_val[0] | ((uint16_t)reg_val[1] << 8)) & pin_mask;
    return ESP_OK;
}

/**
 * @brief 设置单个io的输入输出方向
 *
 * @param pin 引脚，取值见 XL9555_IO_xxx，只能指定一个引脚
 * @param dir 1: 输入，0: 输出
 * @return I2C 读写操作的执行结果
 */
esp_err_t bsp_xl9555_set_pin_dir_single(uint16_t pin, uint8_t dir)
{
    return bsp_xl9555_set_pin_dir(pin, dir ? pin : 0);
}

/**
 * @brief 控制单个io的输出电平
 *
 * @param pin 引脚，取值见 XL9555_IO_xxx，只能指定一个引脚
 * @param lev 1: 高电平，0: 低电平
 * @return I2C 读写操作的执行结果
 */
esp_err_t bsp_xl9555_set_pin_lev_single(uint16_t pin, uint8_t lev)
{
    return bsp_xl9555_set_pin_lev(pin, lev ? pin : 0);
}

/**
 * @brief 获取单个io的输入电平
 *
 * @param pin 引脚，取值见 XL9555_IO_xxx，只能指定一个引脚
 * @return 1: 高电平，0: 低电平，-1: I2C 通信失败
 */
int bsp_xl9555_get_pin_lev_single(uint16_t pin)
{
    uint16_t lev = 0;

    if (bsp_xl9555_read_u16(XL9555_CMD_REG_INPUT_PORT_0, pin, &lev) != ESP_OK) {
        return -1;
    }

    return (lev & pin) ? 1 : 0;
}
