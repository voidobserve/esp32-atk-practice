#include "tcp_client_demo.h"

#include <errno.h>
#include <string.h>

#include "esp_log.h"
#include "lwip/sockets.h"

#define TCP_CLIENT_TAG "tcp client"
/* 发送超时：防火墙丢包时不至于永久阻塞（来源：经验值，5s 足够局域网） */
#define TCP_CLIENT_SEND_TIMEOUT_MS (5000)

bool tcp_client_send_once(const char *ip, uint16_t port, const char *payload)
{
    struct sockaddr_in dest = {0};          /* 静态清零，无 malloc */
    struct timeval timeout = {0};
    int sock = -1;
    int sent = -1;

    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &dest.sin_addr) != 1) {
        ESP_LOGE(TCP_CLIENT_TAG, "bad ip: %s", ip);
        return false;
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0) {
        ESP_LOGE(TCP_CLIENT_TAG, "socket() failed errno=%d", errno);
        return false;
    }

    timeout.tv_sec = TCP_CLIENT_SEND_TIMEOUT_MS / 1000;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    if (connect(sock, (struct sockaddr *)&dest, sizeof(dest)) != 0) {
        ESP_LOGE(TCP_CLIENT_TAG, "connect failed errno=%d (%s)", errno, strerror(errno));
        close(sock);
        return false;
    }

    sent = send(sock, payload, strlen(payload), 0);
    close(sock);

    if (sent < 0) {
        ESP_LOGE(TCP_CLIENT_TAG, "send failed errno=%d (%s)", errno, strerror(errno));
        return false;
    }

    ESP_LOGI(TCP_CLIENT_TAG, "tcp sent %d bytes to %s:%u", sent, ip, (unsigned)port);
    return true;
}
