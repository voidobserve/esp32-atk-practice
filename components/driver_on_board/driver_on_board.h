#ifndef __DRIVER_ON_BOARD_H__
#define __DRIVER_ON_BOARD_H__

#include "driver/gpio.h"

// BOOT按键对应的引脚编号
#define BOOT_GPIO_NUM_IDX GPIO_NUM_0
// 获取BOOT对应的引脚电平
#define BOOT_GET_LEV() \
    (gpio_get_level(BOOT_GPIO_NUM_IDX))

#endif
