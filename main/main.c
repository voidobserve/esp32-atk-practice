#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_psram.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include <string.h>

#include "driver/uart.h"

#include "exit.h"
#include "gptimer.h"
#include "key.h"
#include "led.h"
#include "timer.h"
#include "uart0.h"
#include "user_debug_io.h"
#include "i2c.h"
#include "bsp_xl9555.h"

#include "freertos/queue.h"

QueueHandle_t queue;

/**
 * @brief 程序入口
 * @param 无
 * @retval 无
 */
void app_main(void)
{
    esp_err_t ret;

    ret = nvs_flash_init(); /* 初始化NVS */

    //
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) { //
        //
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
 

    user_debug_io_init();
    bsp_xl9555_init();
    led_init(); /* 初始化LED */
    uart0_init();
    timer_init();

      queue = xQueueCreate(10, sizeof(uint64_t));
    if (!queue) {
        printf("%s\n", __FUNCTION__);
        printf("xQueueCreate fail\n");
        return;
    }
    gptimer_init();

    while (1) {
        uint64_t alarm_val;
        if (xQueueReceive(queue, &alarm_val, portMAX_DELAY) == pdTRUE) {
            printf("alarm val = %llu\n", alarm_val);
        }

        vTaskDelay(10);
    }
}