#include "uart0.h"
#include "driver/uart.h"
#include "driver/gpio.h"
void uart0_init(void)
{
    uart_config_t uart_config_param = {0}; /* 先整体清零，避免 flags 等未赋值成员使用随机值 */
    uart_config_param.baud_rate = UART0_BAUDRATE;
    uart_config_param.data_bits = UART_DATA_8_BITS;         /* 字长为8位数据格式 */
    uart_config_param.parity = UART_PARITY_DISABLE;         /* 无奇偶校验位 */
    uart_config_param.stop_bits = UART_STOP_BITS_1;         /* 一个停止位 */
    uart_config_param.flow_ctrl = UART_HW_FLOWCTRL_DISABLE; /* 无硬件控制流 */
    uart_config_param.source_clk = UART_SCLK_APB;           /* 配置时钟源 */
    uart_config_param.rx_flow_ctrl_thresh = 122;            /* 硬件控制流阈值 */
    uart_param_config(UART_NUM_0, &uart_config_param);      /* 配置uart端口 */

    /* 配置uart引脚 */
    uart_set_pin(UART_NUM_0,
                 UART0_TX_GPIO_NUM_IDX,
                 UART0_RX_GPIO_NUM_IDX,
                 UART_PIN_NO_CHANGE,  // RTS 保持默认配置(禁用)
                 UART_PIN_NO_CHANGE); // CTS 保持默认配置(禁用)

    /* 安装串口驱动 */
    /* 注意：uart_driver_install() 的形参顺序是 rx_buffer_size 在前、tx_buffer_size 在后 */
    uart_driver_install(UART_NUM_0,
                        UART0_RX_BUFFER_SIZE, // 接收缓冲区大小
                        UART0_TX_BUFFER_SIZE, // 发送缓冲区大小
                        20,   // 串口事件队列大小
                        NULL, // 串口事件队列句柄
                        0);   // 中断分配标志，0：默认
}