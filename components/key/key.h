#ifndef __KEY_H__
#define __KEY_H__

#include "driver/gpio.h"

#define KEY_PIN_BOOT GPIO_NUM_0

// 按键扫描周期，单位：ms
#define KEY_SCAN_PERIOD 10
// 按键消抖时间，单位：ms
#define KEY_DEBOUNCE_TIME 30
#define KEY_SCAN_LONG_TIME 750 // 按键长按时间，单位：ms
#define KEY_SCAN_HOLD_TIME 500 // 按键长按后不松手的时间，单位：ms
#define KEY_SCAN_DELAY_TIME 0 // 按键按下松手后，等待连击的延时

void key_init(void);

#endif