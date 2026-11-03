#ifndef __TCP_CLIENT_DEMO_H__
#define __TCP_CLIENT_DEMO_H__

#include <stdbool.h>
#include <stdint.h>

/* 目标服务器：电脑的 IPv4，必须与板子同网段 */
#define TCP_CLIENT_SERVER_IP   "192.168.3.86"
// 目标服务器端口号
#define TCP_CLIENT_SERVER_PORT ((uint16_t)8888)

/* 连接 → 发送 payload → 关闭。成功返回 true。不使用堆内存。 */
bool tcp_client_send_once(const char *ip, uint16_t port, const char *payload);

#endif

