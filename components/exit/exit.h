 

#ifndef __EXIT_H__
#define __EXIT_H__


#include "driver/gpio.h"



/* 引脚定义 */
#define BOOT_INT_GPIO_PIN   GPIO_NUM_0

/*IO操作*/
#define BOOT                gpio_get_level(BOOT_INT_GPIO_PIN)

/* 函数声明 */
void exit_init(void);   /* 外部中断初始化程序 */

#endif
