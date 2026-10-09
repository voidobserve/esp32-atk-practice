#ifndef __USER_DEBUG_IO_H__
#define __USER_DEBUG_IO_H__

#include "driver/gpio.h"

#define USER_DEBUG_IO_NUM_IDX GPIO_NUM_5

void user_debug_io_init(void);
void user_debug_io_toggle(void);


#endif

