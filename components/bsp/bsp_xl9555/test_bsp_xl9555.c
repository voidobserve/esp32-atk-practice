#include "test_bsp_xl9555.h"

#include "bsp_xl9555.h"
#include "esp_log.h"

#include <stdbool.h>
#include <stdint.h>

// bsp_xl9555_init() 中配置的方向掩码：P0.0/P0.1、P1.4..P1.7 为输入。
#define XL9555_EXPECTED_DIR ((uint16_t)0xF003)

static const char *TAG = "xl9555_test";

static bool s_failed;

static void test_check(const char *item, bool passed)
{
    if (passed) {
        ESP_LOGI(TAG, "[PASS] %s", item);
    } else {
        ESP_LOGE(TAG, "[FAIL] %s", item);
        s_failed = true;
    }
}

esp_err_t test_bsp_xl9555_run(void)
{
    esp_err_t err;
    uint16_t dir = 0;
    uint16_t out_lev = 0;
    uint16_t in_lev = 0;
    int int_lev;

    s_failed = false;

    ESP_LOGI(TAG, "========== XL9555 self-test begin ==========");

    err = bsp_xl9555_init();
    test_check("bsp_xl9555_init() returns ESP_OK", err == ESP_OK);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "check wiring: SDA=GPIO41, SCL=GPIO42, addr=0x%02x, "
                      "and XL9555 power",
                 (unsigned)XL9555_ADDR);
        ESP_LOGE(TAG, "========== XL9555 self-test end: FAIL ==========");
        return err;
    }

    err = bsp_xl9555_get_pin_dir(XL9555_IO_MASK_ALL, &dir);
    test_check("read CONFIG registers", err == ESP_OK);
    ESP_LOGI(TAG, "CONFIG = 0x%04X, expect 0x%04X", (unsigned)dir,
             (unsigned)XL9555_EXPECTED_DIR);
    test_check("direction mask is 0xF003", dir == XL9555_EXPECTED_DIR);

    err = bsp_xl9555_get_pin_out_lev(XL9555_IO_MASK_ALL, &out_lev);
    test_check("read OUTPUT registers", err == ESP_OK);
    ESP_LOGI(TAG, "OUTPUT = 0x%04X", (unsigned)out_lev);
    test_check("BEEP (P0.3) outputs high",
               (out_lev & XL9555_IO_MASK_BEEP) != 0);
    test_check("SPK_EN (P0.2) outputs low",
               (out_lev & XL9555_IO_MASK_SPK_EN) == 0);

    err = bsp_xl9555_get_pin_lev(XL9555_IO_MASK_ALL, &in_lev);
    test_check("read INPUT registers", err == ESP_OK);
    ESP_LOGI(TAG, "INPUT = 0x%04X (depends on keys/board)", (unsigned)in_lev);

    int_lev = bsp_xl9555_get_int_lev();
    ESP_LOGI(TAG, "INT pin = %d (1: idle, 0: pending/key pressed)", int_lev);
    if (int_lev != 1) {
        ESP_LOGW(TAG, "INT pin is low after init");
    }

    if (s_failed) {
        ESP_LOGE(TAG, "========== XL9555 self-test end: FAIL ==========");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "========== XL9555 self-test end: ALL PASS ==========");
    return ESP_OK;
}
