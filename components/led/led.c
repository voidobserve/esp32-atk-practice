#include <stdio.h>
#include "led.h"

void led_init(void)
{
    gpio_config_t gpio_config_structure;
    gpio_config_structure.intr_type = GPIO_INTR_DISABLE;
    gpio_config_structure.mode = GPIO_MODE_INPUT_OUTPUT;
    gpio_config_structure.pull_up_en = GPIO_PULLUP_ENABLE;      /* 使能上拉 */
    gpio_config_structure.pull_down_en = GPIO_PULLDOWN_DISABLE; /* 失能下拉 */
    gpio_config_structure.pin_bit_mask = (1ull) << LED_GPIO_NUM_IDX;  /* 设置的引脚的位掩码 */
    gpio_config(&gpio_config_structure);

    gpio_set_level(LED_GPIO_NUM_IDX, 1); // 高电平，关灯
}