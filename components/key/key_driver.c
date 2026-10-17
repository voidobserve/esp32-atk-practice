#include "key_driver.h"

//=======================================================//
// 按键扫描函数: 扫描所有注册的按键驱动
//=======================================================//
volatile key_driver_param_t *this_scan_param;
void key_driver_scan(void *scan_param)
{
    static volatile uint8_t is_key_active; // 按键是否有效的计数值
    uint8_t key_event = KEY_EVENT_NONE;    // 存放待发送的按键事件
    uint8_t cur_key_value = NO_KEY;        // 存放待发送的按键键值
    uint8_t key_value = NO_KEY;
    this_scan_param = (struct key_driver_param *)scan_param;

    // 如果扫描时间未到来，不执行按键扫描
    if (this_scan_param->cur_scan_times < this_scan_param->scan_times) {
        return;
    }

    this_scan_param->cur_scan_times = 0; // 清空扫描时间

    cur_key_value = this_scan_param->get_value(); // 调用用户自定义的获取键值函数

    // if (cur_key_value != NO_KEY)
    //     printf("key id %bu\n", cur_key_value); // 打印获取到的键值

    // 判断按键是否有效
    if (cur_key_value != NO_KEY) {
        is_key_active = 35; // 35*10 Ms
    } else if (is_key_active) {
        is_key_active--;
    }

    // if (cur_key_value != NO_KEY)
    // {
    //     printf("scan_times %bu\n", this_scan_param->scan_times);
    //     printf("cur_scan_times %bu\n", this_scan_param->cur_scan_times);
    //     printf("last_key %bu\n", this_scan_param->last_key);
    //     printf("filter_value %bu\n", this_scan_param->filter_value);
    //     printf("filter_cnt %bu\n", this_scan_param->filter_cnt);
    //     printf("filter_time %bu\n", this_scan_param->filter_time);
    //     printf("long_time %bu\n", ad_key_para.long_time);
    //     printf("long_time %bu\n", (*this_scan_param).long_time);
    //     printf("hold_time %bu\n", this_scan_param->hold_time);
    //     printf("press_cnt %bu\n", this_scan_param->press_cnt);

    //     printf("click_cnt %bu\n", this_scan_param->click_cnt);
    //     printf("click_delay_cnt %bu\n", this_scan_param->click_delay_cnt);
    //     printf("click_delay_time %bu\n", this_scan_param->click_delay_time);
    //     printf("notify_value %bu\n", this_scan_param->notify_value);
    //     printf("key_type %bu\n", this_scan_param->key_type);

    //     printf("latest_key_val %bu\n", this_scan_param->latest_key_val);
    //     printf("latest_key_event %bu\n", this_scan_param->latest_key_event);

    //     printf("ad key long time addr %p\n", &ad_key_para.long_time);
    // printf("scan para long time addr %p\n", &(this_scan_param->long_time));
    //     printf("scan para long time addr %p\n", (*this_scan_param).long_time);
    // }

    // 按键消抖
    if (cur_key_value != this_scan_param->filter_value && this_scan_param->filter_time) {
        // 当前按键值与上一次按键值如果不相等, 重新消抖处理, 注意filter_time != 0;
        this_scan_param->filter_cnt = 0;               // 消抖次数清0, 重新开始消抖
        this_scan_param->filter_value = cur_key_value; // 记录上一次的按键值
        return;                                   // 第一次检测, 返回不做处理
    }

    // 当前按键值与上一次按键值相等, filter_cnt 开始累加;
    if (this_scan_param->filter_cnt < this_scan_param->filter_time) {
        this_scan_param->filter_cnt++;
        return;
    }

    // 按键消抖结束, 开始判断按键类型 单击, 双击, 长按, 多击, HOLD,
    // 长按/HOLD后 抬起
    if (cur_key_value != this_scan_param->last_key) {
        /*
            如果当前的键值为空，上一次的键值又与当前的键值不一样，说明按键松开
            cur_key = NO_KEY; last_key = valid_key -> 按键被抬起
        */
        // printf("key event up\n");

        if (cur_key_value == NO_KEY) {
            if (this_scan_param->press_cnt >= this_scan_param->long_time) {
                // 长按/HOLD状态之后被按键抬起;
                key_event = KEY_EVENT_UP;
                key_value = this_scan_param->last_key;
                goto label_key_driver_scan_notify; // 发送抬起消息
            }

            // 如果没有特殊操作
            this_scan_param->click_delay_cnt = 1; // 按键等待下次连击延时开始
        } else {
            /*
                如果当前的键值不为空，上一次的键值又与当前的键值不一样，说明按键按下
                cur_key = valid_key, last_key = NO_KEY -> 按键被按下
            */
            // 用于判断long和hold事件的计数器重新开始计时;
            this_scan_param->press_cnt = 1;
            if (cur_key_value != this_scan_param->notify_value) {
                // 第一次单击/连击时按下的是不同按键,
                // 单击次数重新开始计数
                this_scan_param->click_cnt = 1;
                this_scan_param->notify_value = cur_key_value;
            } else {
                this_scan_param->click_cnt++; // 单击次数累加
            }

            key_event = KEY_EVENT_PRESS;
            key_value = cur_key_value;
            goto label_key_driver_scan_notify;
        }

        goto label_key_driver_scan_end; // 返回, 等待延时时间到
    } else {
        /*
            如果当前获取的键值与上次获取的键值一样，说明按键在长按或是按键未按下
            cur_key = last_key -> 没有按键按下/按键长按(HOLD)
        */
        if (cur_key_value == NO_KEY) {
            // last_key = NO_KEY; cur_key = NO_KEY -> 没有按键按下
            if (this_scan_param->click_cnt > 0) { // 有按键需要消息需要处理

                // 如果只响应单击事件，可以在这里添加/修改功能

                if (this_scan_param->click_delay_cnt >
                    this_scan_param->click_delay_time) {
                    // 按键被抬起后延时到
                    // TODO: 在此可以添加任意多击事件
                    // if (this_scan_param->click_cnt >= 5)
                    // {
                    //     key_event = KEY_EVENT_FIRTH_CLICK; // 五击
                    // }
                    // else if (this_scan_param->click_cnt >= 4)
                    // {
                    //     key_event = KEY_EVENT_FOURTH_CLICK; // 4击
                    // }
                    // else if (this_scan_param->click_cnt >= 3)
                    // {
                    //     key_event = KEY_EVENT_TRIPLE_CLICK; // 三击
                    // }
                    // else if (this_scan_param->click_cnt >= 2)
                    // if (this_scan_param->click_cnt >= 2)
                    // {
                    //     key_event = KEY_EVENT_DOUBLE_CLICK; // 双击
                    // }
                    // else
                    {
                        key_event = KEY_EVENT_CLICK; // 单击
                        // printf("click \n");
                    }
                    key_value = this_scan_param->notify_value;

                    goto label_key_driver_scan_notify;
                } else { // 按键抬起后等待下次延时时间未到
                    this_scan_param->click_delay_cnt++;
                    // 按键抬起后延时时间未到,返回
                    goto label_key_driver_scan_end;
                }
            } else {
                goto label_key_driver_scan_end; // 没有按键需要处理
            }
        } else {
            // last_key = valid_key; cur_key = valid_key,
            // press_cnt累加用于判断long和hold
            if (this_scan_param->press_cnt < 255) {
                this_scan_param->press_cnt++;
            }

            // printf("long time %bu\n", this_scan_param->long_time);
            // printf("press cnt %bu\n", this_scan_param->press_cnt);
            if (this_scan_param->press_cnt == this_scan_param->long_time) {
                key_event = KEY_EVENT_LONG;

                // printf("long time %bu\n", this_scan_param->long_time);

                // printf("key event long\n");
            } else if (this_scan_param->press_cnt == this_scan_param->hold_time) {
                key_event = KEY_EVENT_HOLD;
                // 下一次scan_para->press_cnt++,还是会进入到这里
                this_scan_param->press_cnt = this_scan_param->long_time;
                // printf("key event hold\n");
            } else {
                // press_cnt 没到长按和HOLD次数，返回
                goto label_key_driver_scan_end;
            }

            // press_cnt 没到长按和HOLD次数, 发消息
            key_value = cur_key_value;
            goto label_key_driver_scan_notify;
        }
    }

label_key_driver_scan_notify:

    // 这里可以判断响应的按键类型，进而跳转到对应的处理函数

    if (key_event != KEY_EVENT_PRESS) {
        this_scan_param->click_cnt = 0;         // 按下次数清0
        this_scan_param->notify_value = NO_KEY; // 在延时的待发送按键值清零
    }
    this_scan_param->latest_key_val = key_value;   // 存放得到的按键键值
    this_scan_param->latest_key_event = key_event; // 存放得到的按键事件

label_key_driver_scan_end:
    this_scan_param->last_key = cur_key_value;
    return;
}
