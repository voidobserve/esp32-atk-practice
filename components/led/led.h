#ifndef __LED_H__
#define __LED_H__

#include "driver/gpio.h"

/* 引脚定义 */
#define LED_GPIO_NUM_IDX GPIO_NUM_1 /* LED连接的GPIO端口 */
#define LED_TOGGLE()                                                 \
    do                                                               \
    {                                                                \
        gpio_set_level(LED_GPIO_NUM_IDX, !gpio_get_level(LED_GPIO_NUM_IDX)); \
    } while (0) /* LED翻转 */

void led_init(void);

#endif