#include "key.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver_on_board.h"

void key_init(void)
{
    gpio_config_t gpio_config_structure;
    gpio_config_structure.intr_type = GPIO_INTR_DISABLE;
    gpio_config_structure.mode = GPIO_MODE_INPUT;
    gpio_config_structure.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config_structure.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config_structure.pin_bit_mask = 0x01 << BOOT_GPIO_NUM_IDX;
    gpio_config(&gpio_config_structure);
}

uint8_t key_scan(uint8_t mode)
{
    uint8_t keyval = 0;
    static uint8_t key_boot = 1;    /* 按键松开标志 */

    if(mode)
    {
        key_boot = 1;
    }

    if (key_boot && (BOOT_GET_LEV() == 0))    /* 按键松开标志为1，且有任意一个按键按下了 */
    {
        vTaskDelay(10);             /* 去抖动 */
        key_boot = 0;

        if (BOOT_GET_LEV() == 0)
        {
            keyval = 1;
        }
    }
    else if (BOOT_GET_LEV() == 1)
    {
        key_boot = 1;
    }

    return keyval;                  /* 返回键值 */
}
