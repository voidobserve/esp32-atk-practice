#include "user_debug_io.h"

#include "driver/gpio.h"

void user_debug_io_init(void)
{
    gpio_config_t gpio_config_structure = {0};
    gpio_config_structure.intr_type = GPIO_INTR_DISABLE;
    gpio_config_structure.mode = GPIO_MODE_INPUT_OUTPUT;
    // gpio_config_structure.pull_up_en = GPIO_PULLDOWN_DISABLE;
    // gpio_config_structure.pull_down_en = GPIO_PULLDOWN_DISABLE;
    /* 设置的引脚的位掩码 */
    gpio_config_structure.pin_bit_mask = ((1ull) << USER_DEBUG_IO_NUM_IDX);
    gpio_config(&gpio_config_structure);

    gpio_set_level(USER_DEBUG_IO_NUM_IDX, 1); // 高电平，关灯
}

void user_debug_io_toggle(void)
{
    gpio_set_level(USER_DEBUG_IO_NUM_IDX,
                   !gpio_get_level(USER_DEBUG_IO_NUM_IDX));
}
