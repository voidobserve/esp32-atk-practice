#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "esp_psram.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include <string.h>

#include "esp_timer.h"
#include "driver/uart.h"

#include "bsp_xl9555.h"
#include "exit.h"
#include "gptimer.h"
#include "i2c.h"
#include "key.h"
#include "key_driver.h"
#include "led.h"
#include "test_bsp_xl9555.h"
#include "timer.h"
#include "uart0.h"
#include "user_debug_io.h"

#include "freertos/queue.h"

#include "tcp_client_demo.h"
#include "wifi_sta_demo.h"

// clang-format off
#define KEY_MESSAGE_MAX_LEN (48)

/* 按键事件回调：运行在 app_main 任务上下文（不是 ISR），可以安全地做阻塞 send */
static void on_key_event(uint8_t io_key_event)
{
    static char message[KEY_MESSAGE_MAX_LEN];   /* 静态分配，无 malloc */
    const char *event_name = "UNKNOWN";
    uint32_t elapsed_ms = (uint32_t)(esp_timer_get_time() / 1000LL);

    switch (io_key_event) {
    case IO_KEY_EVENT_BOOT_PRESS: event_name = "PRESS"; break;
    case IO_KEY_EVENT_BOOT_CLICK: event_name = "CLICK"; break;
    case IO_KEY_EVENT_BOOT_LONG:  event_name = "LONG";  break;
    case IO_KEY_EVENT_BOOT_HOLD:  event_name = "HOLD";  break;
    case IO_KEY_EVENT_BOOT_LOOSE: event_name = "LOOSE"; break;
    default: break;
    }

    snprintf(message, sizeof(message), "KEY,%s,%lu\n", event_name, (unsigned long)elapsed_ms);
    tcp_client_send_once(TCP_CLIENT_SERVER_IP, TCP_CLIENT_SERVER_PORT, message);
}
// clang-format on

void user_10ms_isr(void)
{
    io_key_driver_scan_time_add();
}

/**
 * @brief 程序入口
 * @param 无
 * @retval 无
 */
void app_main(void)
{

    esp_err_t ret;

    ret = nvs_flash_init(); /* 初始化NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

#if 0
    user_debug_io_init();

#if TEST_BSP_XL9555_ENABLE
    // 自检内部会调用 bsp_xl9555_init()，这里不再重复初始化。
    test_bsp_xl9555_run();
#else
    esp_err_t xl9555_err = bsp_xl9555_init();

    if (xl9555_err != ESP_OK) {
        ESP_LOGE("app", "bsp_xl9555_init failed: %s",
                 esp_err_to_name(xl9555_err));
    }
#endif

    led_init(); /* 初始化LED */

    uart0_init();

#endif

    io_key_init();
    timer_init();

    io_key_register_cb(on_key_event); // 注册按键事件回调

    if (wifi_sta_demo_start()) {
        tcp_client_send_once(TCP_CLIENT_SERVER_IP, TCP_CLIENT_SERVER_PORT,
                             "hello\n");
    }

    while (1) {
#if 1
        key_driver_scan((void *)&io_key_driver_param);
        io_key_event_handle();
#endif

        vTaskDelay(10);
    }
}