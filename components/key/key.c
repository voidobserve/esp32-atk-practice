#include "key.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>

#include "bsp_xl9555.h"

#include "key_driver.h"

static uint8_t get_key_val(void);
volatile key_driver_param_t io_key_driver_param = {
    .scan_times = KEY_SCAN_PERIOD,
    .last_key = NO_KEY,

    .filter_time = KEY_DEBOUNCE_TIME / KEY_SCAN_PERIOD,

    .long_time = KEY_SCAN_LONG_TIME / KEY_SCAN_PERIOD,
    .hold_time = KEY_SCAN_HOLD_TIME / KEY_SCAN_PERIOD,
    .click_delay_time = KEY_SCAN_DELAY_TIME,
    .get_value = get_key_val,
};

// io按键键值-按键事件映射表
static uint8_t io_key_event_table[][KEY_EVENT_MAX_NUMS + 1] = {
    {IO_KEY_VAL_BOOT, IO_KEY_EVENT_BOOT_PRESS, IO_KEY_EVENT_BOOT_CLICK,
     IO_KEY_EVENT_BOOT_LONG, IO_KEY_EVENT_BOOT_HOLD, IO_KEY_EVENT_BOOT_LOOSE},
};

void io_key_init(void)
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
    return (gpio_get_level(KEY_PIN_BOOT) == 0 ? IO_KEY_VAL_BOOT : NO_KEY);
}

/**
 * @brief 将 key_driver_scan 得到的按键事件转换成自定义的按键事件
 *
 */
static uint8_t key_driver_event_to_io_key_event(uint8_t key_val,
                                                uint8_t key_event)
{
    uint8_t i;
    uint8_t io_key_event = IO_KEY_EVENT_NONE;
    uint8_t io_key_event_idx = 0; // 在 io_key_event_table 中的索引

    // 将 key_driver 事件转换成 io_key_event_table 对应的索引
    switch (key_event) {
    case KEY_EVENT_PRESS:
        io_key_event_idx = 1;
        break;
    case KEY_EVENT_CLICK:
        io_key_event_idx = 2;
        break;
    case KEY_EVENT_LONG:
        io_key_event_idx = 3;
        break;
    case KEY_EVENT_HOLD:
        io_key_event_idx = 4;
        break;
    case KEY_EVENT_UP:
        io_key_event_idx = 5;
        break;
    default:
        // io_key_event_table 中没有这个事件，直接返回默认的无效事件
        goto label_ret_io_key_event;
        break;
    }

    for (i = 0; i < KEY_EVENT_MAX_NUMS; i++) {
        if (key_val == io_key_event_table[i][0]) {
            // 找到了对应的键值，获取键值对应的按键事件
            io_key_event = io_key_event_table[i][io_key_event_idx];
            break;
        }
    }

label_ret_io_key_event:
    return io_key_event;
}

/**
 * @brief 累加按键扫描时间
 *
 */
void io_key_driver_scan_time_add(void)
{
    if (io_key_driver_param.cur_scan_times < (UINT8_MAX - 10)) {
        io_key_driver_param.cur_scan_times += 10;
    }
}

void io_key_event_handle(void)
{
    uint8_t io_key_event = IO_KEY_EVENT_NONE;
    uint8_t key_event = io_key_driver_param.latest_key_event;
    uint8_t key_val = io_key_driver_param.latest_key_val;

    static uint8_t val = 0;

    if (key_event == KEY_EVENT_NONE) {
        // key_driver 没有获取到有效事件，直接返回
        return;
    }

    io_key_event = key_driver_event_to_io_key_event(key_val, key_event);
    if (IO_KEY_EVENT_NONE == io_key_event) {
        return;
    }
    // 获取io按键事件后，清空key_driver_param中的按键事件
    io_key_driver_param.latest_key_event = KEY_EVENT_NONE;

    switch (io_key_event) {
    case IO_KEY_EVENT_BOOT_PRESS:
        printf("boot press\n");
        break;
    case IO_KEY_EVENT_BOOT_CLICK:
        printf("boot click\n");
        // TODO 未生效
        bsp_xl9555_set_pin_lev(XL9555_IO_MASK_BEEP, !val);
        break;
    case IO_KEY_EVENT_BOOT_LONG:
        printf("boot long\n");
        break;
    case IO_KEY_EVENT_BOOT_HOLD:
        // TODO HOLD事件只生效了一次
        printf("boot hold\n");
        break;
    case IO_KEY_EVENT_BOOT_LOOSE:
        printf("boot loose\n");
        break;
    default:
        break;
    }
}
