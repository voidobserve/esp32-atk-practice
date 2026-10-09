#ifndef __BSP_XL9555_H__
#define __BSP_XL9555_H__

#include "driver/gpio.h"
#include "i2c.h"
#include <stdint.h>

/*
    XL9555 地址 0100 000x，
    由于esp32官方的函数内会将地址左移1位，
    这里定义的是7bit地址： (0b 0100 000x >> 1)
*/
#define XL9555_ADDR    (0x40 >> 1)
#define XL9555_INT_PIN GPIO_NUM_40

// XL9555所使用的i2c总线接口编号
#define XL9555_I2C_BUS_IDX I2C_BUS_0
// XL9555所使用的i2c总线等待超时时间
#define XL9555_I2C_BUS_TIMEOUT_MS I2C_BUS_0_TIMEOUT_MS

// i2c通信中，XL9555控制寄存器对应的控制字节
enum
{
    XL9555_CMD_REG_INPUT_PORT_0 = 0x00,
    XL9555_CMD_REG_INPUT_PORT_1 = 0x01,
    XL9555_CMD_REG_OUTPUT_PORT_0 = 0x02,
    XL9555_CMD_REG_OUTPUT_PORT_1 = 0x03,
    XL9555_CMD_REG_POLARITY_INVERSION_PORT_0 = 0x04, // 极性反转端口
    XL9555_CMD_REG_POLARITY_INVERSION_PORT_1 = 0x05, // 极性反转端口
    XL9555_CMD_REG_CONFIG_PORT_0 = 0x06,             // 配置端口
    XL9555_CMD_REG_CONFIG_PORT_1 = 0x07,             // 配置端口
};

// XL9555的io连接到开发板上的引脚:
enum
{
    XL9555_IO_AP_INT = ((uint16_t)0x01 << 0),
    XL9555_IO_QMA_INT = ((uint16_t)0x01 << 1),
    XL9555_IO_SPK_EN = ((uint16_t)0x01 << 2),
    XL9555_IO_BEEP = ((uint16_t)0x01 << 3),
    XL9555_IO_OV_PWDN = ((uint16_t)0x01 << 4),
    XL9555_IO_OV_RESET = ((uint16_t)0x01 << 5),
    XL9555_IO_GBC_LED = ((uint16_t)0x01 << 6),
    XL9555_IO_GBC_KEY = ((uint16_t)0x01 << 7),
    XL9555_IO_LCD_BL = ((uint16_t)0x01 << 8),
    XL9555_IO_CT_RST = ((uint16_t)0x01 << 9),
    XL9555_IO_SLCD_RST = ((uint16_t)0x01 << 10),
    XL9555_IO_SLCD_PWR = ((uint16_t)0x01 << 11),
    XL9555_IO_KEY3 = ((uint16_t)0x01 << 12),
    XL9555_IO_KEY2 = ((uint16_t)0x01 << 13),
    XL9555_IO_KEY1 = ((uint16_t)0x01 << 14),
    XL9555_IO_KEY0 = ((uint16_t)0x01 << 15),
};

void bsp_xl9555_init(void);

int bsp_xl9555_get_int(void);

#endif
