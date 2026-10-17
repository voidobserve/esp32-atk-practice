#include "key.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "key_driver.h"

static uint8_t get_key_val(void);
volatile key_driver_param_t key_driver_param = {
    .scan_times = KEY_SCAN_PERIOD,
    .last_key = NO_KEY,

    .filter_time = KEY_DEBOUNCE_TIME / KEY_SCAN_PERIOD,

    .long_time = KEY_SCAN_LONG_TIME / KEY_SCAN_PERIOD,
    .hold_time = KEY_SCAN_HOLD_TIME / KEY_SCAN_PERIOD,
    .click_delay_time = KEY_SCAN_DELAY_TIME,
    .get_value = get_key_val,
};

void key_init(void)
{
    gpio_config_t gpio_config_structure;
    gpio_config_structure.intr_type = GPIO_INTR_DISABLE;
    gpio_config_structure.mode = GPIO_MODE_INPUT;
    gpio_config_structure.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config_structure.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config_structure.pin_bit_mask = (uint64_t)0x01 << KEY_PIN_BOOT;
    gpio_config(&gpio_config_structure);
}

/**
 * @brief 获取键值并返回
 *
 * @return uint8_t
 */
static uint8_t get_key_val(void)
{
    return gpio_get_level(KEY_PIN_BOOT);
}

void key_event_handle(void)
{
}
