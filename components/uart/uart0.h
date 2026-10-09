#ifndef __UART0_H__
#define __UART0_H__
 

#define UART0_BAUDRATE ((uint32_t)115200)
#define UART0_TX_GPIO_NUM_IDX GPIO_NUM_43
#define UART0_RX_GPIO_NUM_IDX GPIO_NUM_44

#define UART0_TX_BUFFER_SIZE (1024 * 2) // 发送 环形缓冲区大小
#define UART0_RX_BUFFER_SIZE (1024 * 2) // 接收 环形缓冲区大小

void uart0_init(void);

#endif