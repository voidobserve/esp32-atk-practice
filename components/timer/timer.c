#include "timer.h"
#include "esp_timer.h"

#include "led.h"

void esp_timer_callback(void *arg); // 声明

void timer_init(void)
{
    esp_timer_create_args_t esp_timer_create_args = {
        .callback = esp_timer_callback,
        .arg = NULL,
    };

    esp_timer_handle_t esp_timer_handle;

    esp_timer_create(&esp_timer_create_args, &esp_timer_handle);
    esp_timer_start_periodic(esp_timer_handle, TIMER_PERIOD_US);
}

// extern void user_debug_io_toggle(void);

void esp_timer_callback(void *arg)
{
    // LED_TOGGLE();
    // user_debug_io_toggle();
}
