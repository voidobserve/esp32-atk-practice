#include "gptimer.h"
#include "driver/gptimer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h" 

extern QueueHandle_t queue; // 声明队列句柄

static bool IRAM_ATTR example_timer_on_alarm_cb(
    gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata,
    void *user_ctx)
{
    // 处理事件回调的一般流程：
    // 1. 从 user_ctx 中拿到用户上下文数据（需事先从
    // gptimer_register_event_callbacks 中传入）
    // 2. 从 edata 中获取警报事件数据，比如 edata->count_value
    // 3. 执行用户自定义操作
    // 4. 返回上述操作期间是否有高优先级的任务被唤醒了，以便通知调度器做切换任务

    // 从 user_ctx 中拿到用户上下文数据，目前外部传入的是 QueueHandle_t 类型的数据
    QueueHandle_t queue = (QueueHandle_t)user_ctx;
    uint64_t alarm_value;
    BaseType_t higher_task_woken = pdFALSE;
    // 将回调里的闹钟事件值封装成事件
    alarm_value = edata->alarm_value;
    // 
    xQueueSendFromISR((QueueHandle_t)queue, &alarm_value, &higher_task_woken);

    // 如果队列发送成功并且唤醒了更高优先级任务，就返回 true
    return higher_task_woken == pdTRUE;
}

void gptimer_init(void)
{
    gptimer_handle_t gptimer = NULL; // 存放申请到的定时器句柄 
    gptimer_config_t timer_config = {0}; // 定时器配置 
    gptimer_event_callbacks_t cbs = {0}; // 存放触发事件后，调用的回调函数 

    gptimer_alarm_config_t alarm_config = {0}; // 存放闹钟的配置参数 
 

    timer_config.clk_src = GPTIMER_CLK_SRC_DEFAULT;     // 选择默认的时钟源
    timer_config.direction = GPTIMER_COUNT_UP;          // 计数方向为向上计数
    timer_config.resolution_hz = (uint64_t)100 * 10000; // 1MHz, 1 tick=1us
    timer_config.intr_priority = 0;                     // 0：由驱动自动分配较低的中断优先级(1/2/3)

    // 创建定时器实例
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    alarm_config.reload_count = 0; // 当警报事件发生时，定时器会自动重载到 0
    /*
        设置实际的警报周期，因为配置的定时器分辨率是
        1us，所以 1000000 代表 1s
     */
    alarm_config.alarm_count = (uint64_t)100 * 10000;
    alarm_config.flags.auto_reload_on_alarm = true; // 使能自动重载功能
    // 设置定时器的警报动作
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    // 当警报事件发生时，调用用户回调函数：
    cbs.on_alarm = example_timer_on_alarm_cb;  

    // 注册定时器事件回调函数，允许携带用户上下文
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, queue));
    // 使能定时器
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    // 启动定时器
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}
