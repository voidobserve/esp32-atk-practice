#ifndef __KEY_H__
#define __KEY_H__

#include "driver/gpio.h"
#include "key_driver.h" // key_driver_param_t

#define KEY_PIN_BOOT GPIO_NUM_0

// 按键扫描周期，单位：ms
#define KEY_SCAN_PERIOD 10
// 按键消抖时间，单位：ms
#define KEY_DEBOUNCE_TIME 30
#define KEY_SCAN_LONG_TIME 750 // 按键长按时间，单位：ms
#define KEY_SCAN_HOLD_TIME 500 // 按键长按后不松手的时间，单位：ms
#define KEY_SCAN_DELAY_TIME 0 // 按键按下松手后，等待连击的延时

#define KEY_EVENT_MAX_NUMS (5) // 按键事件最大数量

// 定义io按键的键值
enum
{	
	IO_KEY_VAL_BOOT,

};

// 定义io按键的按键事件
enum
{
	IO_KEY_EVENT_NONE, // 无效的按键事件
	
	IO_KEY_EVENT_BOOT_PRESS,
	IO_KEY_EVENT_BOOT_CLICK,
	IO_KEY_EVENT_BOOT_LONG,
	IO_KEY_EVENT_BOOT_HOLD,
	IO_KEY_EVENT_BOOT_LOOSE,

	
};

// typedef struct
// {
// 	uint8_t key_val; // 键值
// 	uint8_t *key_event_buf_ptr; // 指向
// } ;

extern volatile key_driver_param_t io_key_driver_param;

void io_key_init(void);

void io_key_driver_scan_time_add(void);
void io_key_event_handle(void);

#endif