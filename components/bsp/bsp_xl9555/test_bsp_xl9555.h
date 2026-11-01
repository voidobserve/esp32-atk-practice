#ifndef __TEST_BSP_XL9555_H__
#define __TEST_BSP_XL9555_H__

#include "esp_err.h"

/*
    置 1：上电后先运行 XL9555 自检，main 不再重复调用 bsp_xl9555_init()。
    置 0：走正常初始化流程。
    自检结果从串口日志查看，关键字为 "xl9555_test"。
*/
#define TEST_BSP_XL9555_ENABLE 0

/**
 * @brief 运行 XL9555 自检
 *
 * 依次检查：初始化返回值、方向寄存器、输出寄存器、输入寄存器、INT 引脚。
 *
 * @return ESP_OK: 全部通过；其他: 有检查项失败
 */
esp_err_t test_bsp_xl9555_run(void);

#endif
