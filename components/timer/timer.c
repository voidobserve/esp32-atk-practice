#include "timer.h"
#include "esp_timer.h"

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

void esp_timer_callback(void *arg)
{
    extern void user_10ms_isr(void);
    user_10ms_isr();
}
