#ifndef __KEY_DRIVER_H__
#define __KEY_DRIVER_H__

#include <stdint.h>

#define NO_KEY (255) // 无效按键的键值

enum KEY_EVENT
{
    KEY_EVENT_NONE = 0,
    KEY_EVENT_PRESS,        // 刚按下
    KEY_EVENT_CLICK,        // 单击
    KEY_EVENT_DOUBLE_CLICK, // 双击
    // KEY_EVENT_TRIPLE_CLICK, // 三击
    // KEY_EVENT_FOURTH_CLICK, // 四击
    // KEY_EVENT_FIRTH_CLICK,  // 五击
    KEY_EVENT_LONG, // 长按
    KEY_EVENT_HOLD, // 持续按下(长按不松手)
    KEY_EVENT_UP,   // 长按/持续按下后，松开了按键
};

enum KEY_TYPE
{
    KEY_TYPE_NONE = 0,
    KEY_TYPE_IO,    // 普通IO按键
    KEY_TYPE_AD,    // AD按键
    KEY_TYPE_TOUCH, // 触摸按键
};

typedef struct io_key_driver_param
{
    const uint8_t scan_times; // 按键扫描频率, 单位ms
    /*
        当前按键扫描时间计数, 单位ms
        由ms的定时器中断内(1ms或者10ms)累加，在key_driver_scan()中清零
    */
    volatile uint8_t cur_scan_times;
    // 存放上一次调用get_value()之后得到的按键值
    volatile uint8_t last_key;

    //* 用于消抖类参数 //
    volatile uint8_t filter_value; // 用于按键消抖，存放消抖期间得到的键值
    volatile uint8_t filter_cnt;   // 用于按键消抖时的累加值
    const uint8_t filter_time;     // 当filter_cnt累加到base_cnt值时, 消抖有效

    //* 用于判定长按和HOLD事件参数
    const uint8_t long_time; // 按键判定长按数量
    const uint8_t hold_time; // 按键判定HOLD数量
    // 与long_time和hold_time对比, 判断long_event和hold_event
    volatile uint8_t press_cnt;

    //* 用于判定连击事件参数
    volatile uint8_t click_cnt;       // 按键按下次数
    volatile uint8_t click_delay_cnt; // 按键被抬起后等待连击事件延时计数
    const uint8_t click_delay_time;   // 按键被抬起后等待连击事件延时数量
    volatile uint8_t notify_value;    // 在延时的待发送按键值
    const uint8_t key_type;
    uint8_t (*get_value)(void); // 用户自定义的获取键值的函数

    //* 存放得到的按键键值和按键事件
    volatile uint8_t latest_key_val;
    volatile uint8_t latest_key_event;
} key_driver_param_t;

extern void key_driver_scan(void *_scan_param);

#endif
