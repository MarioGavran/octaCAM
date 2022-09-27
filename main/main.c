#include <stdio.h>
#include <string.h>

// FreeRTOS includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

// ESP32 native includes
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_wifi.h"
#include "esp_netif.h"

#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_camera.h"
#include "img_converters.h"
//#include "fb_gfx.h"
#include "driver/ledc.h"
#include "sdkconfig.h"

#include "esp_log.h"

#define index_ov2640_html_gz_len 3013 // 6787
const uint8_t index_ov2640_html_gz[] = {
    0x1f, 0x8b, 0x08, 0x08, 0x72, 0xf1, 0x84, 0x62, 0x00, 0xff, 0x69, 0x6e, 0x64, 0x65, 0x78, 0x2e,
    0x68, 0x74, 0x6d, 0x6c, 0x2e, 0x67, 0x7a, 0x00, 0xdd, 0x1b, 0x6b, 0x93, 0xdb, 0xb6, 0xf1, 0xbb,
    0x7f, 0x05, 0x2d, 0x3b, 0x96, 0x6e, 0x7c, 0x94, 0x4e, 0xe7, 0xab, 0xed, 0x28, 0xd2, 0xb9, 0x4e,
    0x72, 0xa9, 0x33, 0x63, 0x37, 0x9d, 0xd8, 0xc9, 0x4c, 0x3f, 0xd5, 0x10, 0x09, 0x49, 0xf0, 0x51,
    0x24, 0x0b, 0x82, 0x7a, 0xd4, 0x73, 0xbf, 0xb3, 0x7f, 0x27, 0xbb, 0x00, 0xf8, 0x02, 0xc1, 0xc7,
    0xd9, 0xd7, 0x4c, 0xe7, 0xa4, 0x19, 0x89, 0x22, 0x16, 0x8b, 0xc5, 0x3e, 0x1f, 0xa0, 0xe6, 0x0f,
    0xfd, 0xc8, 0x13, 0xc7, 0x98, 0x3a, 0x1b, 0xb1, 0x0d, 0x2e, 0x1f, 0xcc, 0xd5, 0x17, 0x7c, 0x53,
    0xe2, 0x5f, 0x3e, 0x70, 0xe0, 0x35, 0xdf, 0x52, 0x41, 0x1c, 0x6f, 0x43, 0x78, 0x42, 0xc5, 0x62,
    0x90, 0x8a, 0x95, 0xfb, 0x72, 0x50, 0x1e, 0x0a, 0xc9, 0x96, 0x2e, 0x06, 0x3b, 0x46, 0xf7, 0x71,
    0xc4, 0xc5, 0xc0, 0xf1, 0xa2, 0x50, 0xd0, 0x10, 0x40, 0xf7, 0xcc, 0x17, 0x9b, 0x85, 0x4f, 0x77,
    0xcc, 0xa3, 0xae, 0xfc, 0x71, 0xca, 0x42, 0x26, 0x18, 0x09, 0xdc, 0xc4, 0x23, 0x01, 0x5d, 0x4c,
    0x33, 0x3c, 0x82, 0x89, 0x80, 0x5e, 0x5e, 0xbd, 0xff, 0xc7, 0xb3, 0x73, 0xe7, 0x97, 0xdf, 0x5f,
    0x3c, 0x7f, 0x71, 0x36, 0x9f, 0xa8, 0x7b, 0x6a, 0x3c, 0x11, 0xc7, 0xec, 0x1a, 0x5f, 0xcb, 0xc8,
    0x3f, 0x3a, 0x9f, 0xf3, 0x9f, 0xf8, 0x5a, 0xc1, 0xa2, 0xee, 0x8a, 0x6c, 0x59, 0x70, 0x9c, 0x39,
    0xaf, 0x39, 0xac, 0x71, 0xea, 0xbc, 0xa1, 0xc1, 0x8e, 0x0a, 0xe6, 0x91, 0x53, 0x27, 0x21, 0x61,
    0xe2, 0x26, 0x94, 0xb3, 0xd5, 0x77, 0x95, 0x69, 0x4b, 0xe2, 0x5d, 0xaf, 0x79, 0x94, 0x86, 0xfe,
    0xcc, 0x79, 0x34, 0x7d, 0x89, 0xef, 0x2a, 0x80, 0x17, 0x05, 0x11, 0x87, 0xb1, 0xab, 0x9f, 0xf0,
    0x5d, 0x1d, 0x93, 0x6b, 0x26, 0xec, 0x3f, 0x74, 0xe6, 0x4c, 0x9f, 0xc7, 0x87, 0x7c, 0xec, 0xe6,
    0x41, 0x7e, 0xb9, 0x39, 0xb7, 0xd1, 0xa9, 0xe7, 0xbc, 0xb4, 0xcf, 0x49, 0xa8, 0x27, 0x58, 0x14,
    0x8e, 0xb7, 0x84, 0x85, 0xc6, 0x6c, 0x9f, 0x25, 0x71, 0x40, 0x60, 0x87, 0xab, 0x80, 0x5a, 0xe7,
    0x3e, 0xda, 0xd2, 0x30, 0x3d, 0xed, 0x83, 0x0a, 0x31, 0xb8, 0x3e, 0xe3, 0x0a, 0x62, 0x86, 0x1b,
    0x4d, 0xb7, 0x61, 0x23, 0xce, 0x26, 0x42, 0xc2, 0x28, 0xa4, 0x06, 0x57, 0x10, 0xf1, 0x9e, 0x93,
    0x18, 0x07, 0xf1, 0xbb, 0x3a, 0xbc, 0x65, 0xa1, 0xd2, 0x85, 0x99, 0xf3, 0xec, 0xe2, 0x2c, 0x3e,
    0xb4, 0xc8, 0xe3, 0xd9, 0x73, 0x7c, 0x57, 0x01, 0x62, 0xe2, 0xfb, 0x2c, 0x5c, 0xcf, 0x1c, 0x60,
    0x9e, 0x31, 0x35, 0xe2, 0x3e, 0xe5, 0x2e, 0x27, 0x3e, 0x4b, 0x93, 0x99, 0x73, 0x61, 0x8e, 0x6f,
    0x09, 0x5f, 0xc3, 0xda, 0x22, 0x02, 0xc2, 0xdc, 0x69, 0x6d, 0x65, 0x3d, 0xcc, 0xd9, 0x7a, 0x23,
    0x40, 0x36, 0x95, 0xf1, 0x32, 0x33, 0xb4, 0x76, 0xb7, 0x09, 0xa6, 0x91, 0x1f, 0x75, 0x6e, 0x90,
    0x80, 0xad, 0x43, 0x97, 0x09, 0xba, 0x05, 0x92, 0x13, 0xc1, 0xa9, 0xf0, 0x36, 0xb6, 0x65, 0x57,
    0x6c, 0x9d, 0x72, 0x6a, 0x2c, 0x9a, 0xf3, 0xa2, 0x61, 0x33, 0x30, 0x50, 0xbd, 0xed, 0xee, 0xe9,
    0xf2, 0x9a, 0x09, 0x57, 0xef, 0x75, 0x49, 0x57, 0x11, 0x07, 0x45, 0x34, 0xa0, 0xb2, 0xd1, 0x20,
    0xf2, 0xae, 0xdd, 0x44, 0x10, 0x0e, 0xfc, 0x68, 0x47, 0x44, 0x56, 0x82, 0x82, 0x91, 0xb4, 0xe1,
    0xa1, 0x28, 0xd1, 0x76, 0x2c, 0xf6, 0xa5, 0xf4, 0x20, 0x0b, 0x03, 0x16, 0xd2, 0x7e, 0xe4, 0xd8,
    0xd6, 0xaa, 0xa2, 0x51, 0x10, 0x2d, 0x8c, 0x66, 0xdb, 0x75, 0x93, 0x84, 0xe5, 0x7e, 0xaa, 0xc8,
    0xb5, 0x3e, 0x4f, 0xcf, 0xce, 0xbe, 0xa9, 0x0e, 0x6c, 0xa8, 0x52, 0x27, 0x92, 0x8a, 0xe8, 0xcb,
    0xb5, 0xb5, 0xa2, 0xea, 0x25, 0x5a, 0xff, 0xba, 0xa5, 0x3e, 0x23, 0xce, 0xa8, 0x64, 0x52, 0x2f,
    0xcf, 0x40, 0x17, 0x4e, 0x1c, 0x12, 0xfa, 0xce, 0x28, 0xe2, 0x0c, 0x14, 0x95, 0x48, 0xf3, 0x0e,
    0xe0, 0x0e, 0xf8, 0xdc, 0x98, 0x9e, 0x18, 0xdb, 0x6a, 0xd0, 0xe7, 0x0e, 0x9d, 0xee, 0x61, 0xe7,
    0x9d, 0xda, 0x6d, 0x6c, 0xa6, 0x95, 0xf9, 0x5d, 0x02, 0x50, 0x1c, 0x03, 0x72, 0x9a, 0x04, 0x91,
    0x01, 0x64, 0x02, 0x81, 0xf0, 0xe3, 0x8d, 0x00, 0x6c, 0xb7, 0x71, 0x5c, 0x07, 0xdd, 0xd0, 0x49,
    0x1d, 0x5e, 0x23, 0xab, 0xcb, 0xce, 0x94, 0x6c, 0x8f, 0x2d, 0xd5, 0xb7, 0x53, 0x18, 0xaf, 0x7a,
    0x9b, 0x0a, 0xd0, 0x62, 0xc6, 0xfd, 0x4d, 0xb9, 0xa7, 0x39, 0xf7, 0x36, 0xe9, 0x5e, 0x66, 0xdd,
    0xdb, 0xb4, 0xfb, 0x9a, 0x77, 0x4f, 0x13, 0xef, 0x34, 0x73, 0x25, 0x9e, 0xe6, 0x88, 0xfb, 0x68,
    0x99, 0x0a, 0x11, 0x85, 0xc9, 0x17, 0xf9, 0x76, 0x9b, 0x0d, 0x7c, 0x4a, 0x13, 0xc1, 0x56, 0x47,
    0x57, 0xdb, 0x18, 0xd8, 0x40, 0x4c, 0x20, 0x15, 0x5a, 0x52, 0xb1, 0xa7, 0xd4, 0x1a, 0x6b, 0x0d,
    0x4a, 0xdc, 0xfd, 0xff, 0x8e, 0x16, 0x0f, 0x3e, 0x29, 0xef, 0x43, 0x04, 0x49, 0xfc, 0xff, 0x07,
    0x32, 0x0e, 0x66, 0xce, 0xf7, 0x67, 0x08, 0xa6, 0x08, 0xfc, 0x21, 0xd9, 0x81, 0x3b, 0x5e, 0xaf,
    0x03, 0xd3, 0x9a, 0xbd, 0x94, 0x27, 0x98, 0x23, 0xc6, 0x11, 0xc3, 0xad, 0x54, 0x57, 0xaa, 0x7a,
    0x2d, 0x6b, 0x46, 0x51, 0x20, 0x76, 0xbd, 0xa5, 0x81, 0x3b, 0x4a, 0x05, 0x6a, 0x72, 0x4d, 0xd7,
    0x23, 0xa0, 0x97, 0x09, 0x40, 0x6b, 0xdc, 0xd7, 0x3e, 0xcb, 0xb8, 0x9b, 0x39, 0x2b, 0x6b, 0xc4,
    0xab, 0xae, 0x3f, 0xf3, 0x36, 0xd4, 0xbb, 0xa6, 0xfe, 0xd3, 0xd6, 0xac, 0xaf, 0x29, 0xfd, 0x1c,
    0xb3, 0x30, 0x4e, 0x85, 0x8b, 0x19, 0x5c, 0x7c, 0x67, 0xc2, 0x92, 0xa6, 0x9c, 0x6d, 0xe1, 0xfc,
    0xbc, 0x29, 0xd7, 0xf9, 0x4b, 0x7c, 0xb0, 0x6f, 0xb0, 0x4c, 0xd4, 0x65, 0x40, 0x96, 0x34, 0x68,
    0x22, 0x4d, 0xbb, 0x0d, 0x4b, 0x80, 0xd1, 0xde, 0xda, 0x9e, 0x1e, 0x1a, 0x69, 0xed, 0xc5, 0x8b,
    0x6f, 0x3a, 0x79, 0x23, 0xaf, 0x8b, 0x3c, 0xbd, 0x32, 0x96, 0xd0, 0x00, 0x34, 0xdf, 0x96, 0xae,
    0xc3, 0xf8, 0x1e, 0x16, 0xb7, 0x62, 0xe7, 0x24, 0x5c, 0x53, 0x70, 0x8f, 0x87, 0x12, 0x56, 0x7d,
    0xaf, 0xb9, 0x8c, 0xe8, 0xdc, 0x30, 0x86, 0x27, 0x60, 0xac, 0x6d, 0x45, 0x65, 0x96, 0xa5, 0xd5,
    0xd4, 0x8d, 0x5b, 0xa4, 0x4d, 0x25, 0xc9, 0x35, 0x2e, 0x3e, 0xad, 0xc9, 0x5b, 0x65, 0x4e, 0x35,
    0x1d, 0xaf, 0x6a, 0x49, 0xad, 0x3a, 0x68, 0x35, 0xd2, 0xac, 0xc8, 0x5b, 0xad, 0xda, 0xca, 0xc3,
    0xd5, 0xea, 0xd9, 0xd9, 0xb3, 0x8b, 0xd6, 0x34, 0xae, 0xb6, 0x13, 0xa3, 0x44, 0x34, 0x8c, 0x38,
    0x37, 0x6e, 0xab, 0x44, 0x13, 0xb2, 0xab, 0x65, 0xfc, 0x51, 0x02, 0xf5, 0x33, 0x96, 0x6b, 0x64,
    0x99, 0x40, 0xc1, 0x26, 0x8c, 0xda, 0x4b, 0xab, 0xe7, 0x79, 0x8d, 0x0e, 0x99, 0x47, 0xd6, 0x94,
    0x36, 0x63, 0x57, 0x9d, 0xb4, 0x0a, 0x37, 0xeb, 0xc3, 0x25, 0x01, 0xd5, 0x32, 0x57, 0x41, 0x0f,
    0xc2, 0xf5, 0xa9, 0x17, 0x71, 0x95, 0x7a, 0x5a, 0x4a, 0x44, 0x43, 0x18, 0xcd, 0xda, 0x35, 0xdb,
    0x44, 0x3b, 0xca, 0x0d, 0x26, 0x18, 0x42, 0xb9, 0xf8, 0xf6, 0xc2, 0x6f, 0xc1, 0x40, 0x20, 0x88,
    0xd4, 0xf8, 0x58, 0x45, 0x71, 0x3e, 0xf5, 0xce, 0xad, 0x46, 0xa5, 0x50, 0x8c, 0x41, 0x87, 0xc9,
    0x32, 0xa0, 0x66, 0x10, 0xcc, 0x76, 0xe1, 0xd3, 0x15, 0x49, 0x03, 0xd1, 0xa2, 0x39, 0xe4, 0x0c,
    0xdf, 0x95, 0xb0, 0x52, 0x8a, 0x2c, 0x56, 0x7b, 0xcf, 0xb4, 0x7c, 0x0a, 0x5e, 0x0d, 0x24, 0xcd,
    0x7c, 0x7b, 0x39, 0x5c, 0xd6, 0xaf, 0x9a, 0x24, 0x9a, 0x7d, 0x66, 0x53, 0x58, 0xa9, 0xab, 0xb3,
    0xdd, 0x93, 0x6d, 0x09, 0xf8, 0x15, 0x0c, 0x9e, 0xd0, 0x57, 0xa8, 0xc9, 0xa7, 0x50, 0x52, 0x4e,
    0x03, 0xd0, 0x81, 0x9d, 0x21, 0xfd, 0x92, 0xab, 0x9c, 0x3e, 0x07, 0x9d, 0xb4, 0x2e, 0xe1, 0x05,
    0x51, 0xf2, 0x85, 0xda, 0x6f, 0x57, 0xfe, 0xda, 0xdd, 0x4e, 0xdb, 0xce, 0x69, 0xec, 0x6f, 0x36,
    0x06, 0xff, 0xa0, 0xc0, 0xa8, 0xf9, 0xa1, 0x26, 0x57, 0x23, 0xcd, 0x46, 0x16, 0x4c, 0x59, 0x4e,
    0xd4, 0x66, 0x91, 0x5d, 0xfe, 0xcd, 0xca, 0xd3, 0x0d, 0xf3, 0x7d, 0xda, 0xe8, 0xa0, 0xd1, 0x4c,
    0xab, 0x2a, 0x5a, 0x8a, 0x4e, 0x2a, 0x4c, 0xd8, 0x1c, 0x7c, 0xd5, 0xef, 0x36, 0x76, 0x22, 0x6a,
    0x2a, 0x98, 0xbb, 0x10, 0xd4, 0x70, 0x50, 0x5d, 0x84, 0xa8, 0x3a, 0x93, 0x12, 0xe9, 0x32, 0x6a,
    0x8f, 0x75, 0x92, 0xa2, 0xf3, 0x42, 0xd7, 0x16, 0xca, 0xbf, 0x32, 0x17, 0x93, 0x61, 0xd8, 0x5c,
    0xc7, 0xba, 0xed, 0x3f, 0x21, 0x31, 0x6b, 0x21, 0x26, 0xcf, 0xd2, 0xb2, 0x06, 0x5f, 0x15, 0xaa,
    0xa7, 0x88, 0x65, 0x6b, 0x75, 0xa2, 0x7b, 0xab, 0xf3, 0x89, 0x6a, 0xf9, 0x3e, 0x98, 0x63, 0x77,
    0x35, 0xeb, 0xbb, 0x6a, 0x7c, 0x5e, 0x40, 0x92, 0x64, 0x31, 0xc0, 0x36, 0xa2, 0x6e, 0xd9, 0xca,
    0x61, 0x9f, 0xed, 0x1c, 0xe6, 0x2f, 0x06, 0x41, 0xb4, 0x8e, 0x4a, 0xf7, 0xe5, 0x98, 0x92, 0x0e,
    0x54, 0xa5, 0x8b, 0x41, 0x25, 0xc1, 0x1c, 0xc8, 0x19, 0xc5, 0xad, 0xc1, 0xe5, 0x93, 0x47, 0xdf,
    0xbe, 0x78, 0xf1, 0xfc, 0xbb, 0x27, 0xe1, 0x32, 0x89, 0xf5, 0xe7, 0x07, 0x95, 0x67, 0xab, 0x56,
    0x30, 0x38, 0x48, 0x21, 0x40, 0x55, 0x92, 0xf9, 0x44, 0x22, 0x2d, 0x11, 0x30, 0x01, 0x0a, 0x2c,
    0xf4, 0xe8, 0xb4, 0xde, 0x24, 0x29, 0x1b, 0x4e, 0x98, 0x4f, 0x97, 0x84, 0x1b, 0xc3, 0x12, 0x44,
    0xf2, 0xdc, 0xc1, 0x86, 0x38, 0x60, 0x41, 0x1e, 0x2f, 0xa3, 0x83, 0x49, 0xb1, 0xdc, 0x84, 0x16,
    0x80, 0x86, 0xa2, 0xbe, 0x0d, 0x19, 0x4c, 0x91, 0x53, 0x31, 0x93, 0x86, 0xf1, 0x6a, 0x63, 0x20,
    0x87, 0xca, 0x78, 0x8c, 0x90, 0xba, 0xd4, 0xb1, 0x20, 0xcb, 0xc1, 0xb5, 0x36, 0x22, 0xf4, 0x9a,
    0x82, 0xef, 0x17, 0x2c, 0x08, 0x06, 0x97, 0x7f, 0xa3, 0xc2, 0x79, 0x8f, 0x97, 0xf3, 0x89, 0x02,
    0xe8, 0x87, 0x21, 0x53, 0x1b, 0x68, 0xcd, 0x90, 0xed, 0xe0, 0xf2, 0x3d, 0x96, 0xdf, 0x80, 0x07,
    0x7f, 0xb5, 0x23, 0x02, 0xb5, 0x51, 0x64, 0xc3, 0xb6, 0xfa, 0xee, 0xca, 0xdd, 0xf7, 0xdc, 0xd7,
    0x5e, 0x6b, 0xf9, 0xe0, 0x72, 0xdf, 0x9b, 0x8a, 0xde, 0x44, 0x40, 0x31, 0xdb, 0x93, 0x0c, 0x92,
    0x93, 0x41, 0x6e, 0xc7, 0xd5, 0x24, 0x9f, 0x08, 0x2a, 0x7b, 0x9b, 0x89, 0x7e, 0x3e, 0xd1, 0xbf,
    0xfb, 0x8d, 0x1f, 0x8e, 0x3d, 0xf7, 0x7d, 0xc8, 0xa9, 0x38, 0xdc, 0x8e, 0xfc, 0x63, 0x3e, 0xf1,
    0xf8, 0xe5, 0xda, 0x33, 0x9f, 0x80, 0xd9, 0x18, 0x86, 0x5b, 0x35, 0x73, 0x79, 0x4b, 0xf5, 0xd5,
    0x2c, 0x46, 0x97, 0x1b, 0xb9, 0xd4, 0xe2, 0x22, 0x49, 0x01, 0x93, 0x55, 0x4e, 0xcc, 0x4c, 0x5e,
    0x54, 0x54, 0x6c, 0xe0, 0xcd, 0x9c, 0x28, 0x64, 0x90, 0x8a, 0x6b, 0x53, 0x73, 0x36, 0x9c, 0xae,
    0x16, 0x83, 0x47, 0x39, 0x3e, 0xcd, 0x00, 0x04, 0x19, 0x38, 0x7e, 0xb4, 0x0f, 0x83, 0x88, 0xa0,
    0x5f, 0x20, 0xb1, 0x00, 0x0a, 0xc7, 0x9f, 0xe2, 0x35, 0x18, 0x16, 0x0c, 0xce, 0x27, 0xa4, 0x61,
    0x0d, 0x24, 0x59, 0x23, 0x93, 0x69, 0x8f, 0x72, 0x38, 0xf2, 0x32, 0x37, 0xcd, 0xff, 0x5a, 0x98,
    0x50, 0x78, 0x2d, 0x68, 0x98, 0x16, 0x7b, 0x1e, 0x38, 0x09, 0xf7, 0x16, 0x03, 0x20, 0x90, 0x47,
    0x09, 0x44, 0x43, 0x06, 0x01, 0xd8, 0xc2, 0x28, 0x0b, 0x53, 0x27, 0x26, 0x57, 0x4b, 0x40, 0x86,
    0xca, 0xcd, 0x13, 0x8f, 0xb3, 0x58, 0x14, 0xa0, 0x70, 0x8e, 0x98, 0x82, 0xa3, 0x13, 0x63, 0x88,
    0xeb, 0x57, 0x3b, 0xb8, 0x78, 0xcb, 0x12, 0x70, 0xc2, 0x94, 0x8f, 0x86, 0x3f, 0xfe, 0xf2, 0xee,
    0x07, 0xe5, 0x91, 0xdf, 0x02, 0x6b, 0xa8, 0x3f, 0x3c, 0x75, 0x56, 0x69, 0xa8, 0x34, 0x74, 0x44,
    0x11, 0xd6, 0xec, 0x4a, 0xef, 0x08, 0x87, 0xfc, 0x2c, 0xa1, 0x6f, 0xa2, 0x44, 0x38, 0x8b, 0x02,
    0x37, 0xc4, 0x6d, 0x59, 0x54, 0x8c, 0xd5, 0xae, 0x6a, 0x73, 0x14, 0x03, 0x7e, 0xe3, 0x01, 0x4c,
    0xca, 0xe7, 0x3f, 0x75, 0x86, 0xb3, 0x97, 0xd3, 0xa1, 0xd1, 0x97, 0xcd, 0x08, 0x58, 0x61, 0x53,
    0x1a, 0x66, 0x8c, 0x52, 0x0e, 0x47, 0x86, 0xde, 0xd2, 0x24, 0x45, 0x02, 0x23, 0x0c, 0x02, 0x9c,
    0x58, 0xf9, 0x3f, 0x16, 0x1b, 0x1a, 0x8e, 0x8a, 0x2d, 0x71, 0x9a, 0xc4, 0x60, 0x71, 0xb5, 0x5e,
    0x7b, 0xf9, 0xc5, 0x56, 0x05, 0xdc, 0x18, 0xda, 0x9e, 0x22, 0x4d, 0x9c, 0x87, 0x8b, 0x05, 0x64,
    0x50, 0x67, 0x6d, 0xd3, 0x64, 0x8e, 0xb3, 0x34, 0x67, 0x9e, 0x42, 0xa6, 0x5d, 0xb9, 0xf1, 0x01,
    0x92, 0x49, 0x4b, 0x47, 0x3b, 0x0f, 0xfd, 0x0e, 0x0d, 0x6a, 0xd9, 0xb5, 0xf9, 0xca, 0x51, 0x62,
    0x66, 0x3a, 0x3a, 0x31, 0x37, 0xe9, 0x13, 0x41, 0xba, 0x28, 0xd5, 0xd4, 0xc2, 0x9e, 0x4e, 0x1d,
    0x09, 0xdf, 0x4c, 0x92, 0x24, 0xeb, 0x64, 0x0c, 0xe2, 0x05, 0x4e, 0x97, 0xb4, 0x83, 0xf3, 0x9e,
    0x8b, 0xb8, 0xd3, 0x53, 0x07, 0xa1, 0xbb, 0x96, 0x68, 0xe1, 0x8a, 0x75, 0xe4, 0xa6, 0x41, 0xe6,
    0xb7, 0xa4, 0xb4, 0x93, 0x42, 0x93, 0xb2, 0x72, 0xef, 0x51, 0x15, 0x0e, 0x21, 0xe8, 0x32, 0x38,
    0x2b, 0x0a, 0xba, 0x0d, 0x99, 0xd5, 0xe2, 0xd2, 0xb2, 0x1a, 0x24, 0xc9, 0xd2, 0x91, 0xa0, 0xed,
    0xa1, 0x21, 0x8e, 0x86, 0xca, 0xbb, 0x0d, 0xab, 0x9b, 0xa8, 0xee, 0x54, 0x61, 0x4e, 0x36, 0xd1,
    0xbe, 0x2f, 0x66, 0x4e, 0xb7, 0x50, 0x97, 0x37, 0x22, 0xb7, 0x60, 0xd7, 0x25, 0xf4, 0xad, 0x48,
    0xcf, 0xca, 0x6e, 0x03, 0xbf, 0x06, 0xce, 0x8b, 0xf2, 0x85, 0x23, 0x78, 0x5a, 0xa4, 0xb5, 0x8d,
    0x24, 0xd0, 0xf0, 0x36, 0x14, 0x64, 0x5b, 0xec, 0x4d, 0xc4, 0x8a, 0x80, 0x49, 0x75, 0x53, 0x91,
    0xc6, 0x60, 0x08, 0xf4, 0x77, 0x12, 0xa4, 0x48, 0xca, 0x88, 0x82, 0xcf, 0xd9, 0xe1, 0x8f, 0x53,
    0x3d, 0xf2, 0x2b, 0xac, 0x2b, 0xc0, 0x73, 0x58, 0x29, 0x2c, 0x83, 0xc0, 0xec, 0xea, 0xcf, 0x85,
    0x13, 0xa6, 0x41, 0xe0, 0xbc, 0x92, 0xec, 0x70, 0x66, 0x95, 0xd1, 0x1a, 0xa6, 0x00, 0xf2, 0x45,
    0xfd, 0x2c, 0x86, 0xa4, 0xa5, 0x06, 0x50, 0x1e, 0x94, 0x3c, 0x1b, 0x4b, 0x32, 0x6d, 0x4c, 0x90,
    0x03, 0x00, 0x63, 0x07, 0x98, 0x4c, 0x4c, 0x46, 0x64, 0xee, 0xaf, 0x42, 0xfe, 0x93, 0x27, 0xd5,
    0x25, 0xd1, 0x17, 0x4a, 0x84, 0x4d, 0x46, 0xa5, 0x66, 0x43, 0x5c, 0x81, 0x98, 0x05, 0x6c, 0xb4,
    0x58, 0x55, 0xdd, 0x9e, 0xa5, 0xd7, 0xc3, 0xa5, 0x1f, 0x56, 0x99, 0xfd, 0xb9, 0x4e, 0x60, 0x75,
    0xb2, 0x79, 0xa8, 0x97, 0x59, 0xbd, 0x49, 0x84, 0x85, 0x56, 0xe4, 0xb4, 0x9d, 0x33, 0xc9, 0x9e,
    0x81, 0x0b, 0x41, 0x1d, 0x18, 0x63, 0xc1, 0xd1, 0xb4, 0x51, 0x0f, 0xe2, 0x98, 0x33, 0x54, 0x59,
    0xc6, 0x70, 0x66, 0x05, 0xd1, 0x1d, 0x28, 0xfb, 0xa0, 0xf2, 0xe5, 0x90, 0x8b, 0x54, 0xc3, 0xa5,
    0x65, 0x5b, 0x85, 0x96, 0xfe, 0x3b, 0xa5, 0xfc, 0x08, 0x22, 0xfd, 0xf8, 0xf8, 0x73, 0x16, 0x46,
    0x6f, 0x26, 0x98, 0x32, 0xf1, 0x28, 0x78, 0x05, 0x81, 0x76, 0xf1, 0xf8, 0x33, 0x50, 0xcd, 0xfc,
    0x9b, 0x27, 0xb0, 0x35, 0xf8, 0x21, 0x37, 0x78, 0xf3, 0xb1, 0x8e, 0x4e, 0x45, 0x4e, 0x89, 0xae,
    0x2d, 0x76, 0x66, 0xb1, 0xc6, 0xae, 0xf6, 0x65, 0xda, 0xa2, 0x80, 0x42, 0x16, 0xb0, 0x1e, 0x7d,
    0xe4, 0x14, 0xb0, 0x02, 0xa9, 0x22, 0x72, 0x1e, 0x7f, 0x96, 0x0b, 0xdc, 0xc0, 0x61, 0x6b, 0xc8,
    0x92, 0x0d, 0xf5, 0xe1, 0x79, 0x1f, 0x19, 0x06, 0x67, 0x30, 0x64, 0x44, 0xc6, 0x9b, 0x8f, 0x76,
    0x3a, 0x0c, 0x37, 0x6f, 0x70, 0x26, 0x4b, 0x40, 0x6a, 0x53, 0xc7, 0x72, 0xe5, 0xf7, 0xb2, 0x83,
    0x17, 0xf1, 0xd7, 0x41, 0x30, 0x1a, 0xaa, 0xd6, 0x95, 0xc5, 0x5f, 0x8c, 0xa1, 0x18, 0xbe, 0x22,
    0xc0, 0x8e, 0x26, 0x07, 0xa4, 0xed, 0x29, 0x0a, 0xbd, 0x80, 0x79, 0xd7, 0xe8, 0x1e, 0x1a, 0xfc,
    0x40, 0xde, 0x3e, 0x80, 0x68, 0x80, 0xea, 0x13, 0x13, 0x0e, 0xc4, 0xfd, 0x3d, 0xf2, 0x69, 0xc3,
    0xe6, 0xea, 0x92, 0x3f, 0x29, 0x37, 0x78, 0x0a, 0xc1, 0xe3, 0x53, 0x5c, 0xe5, 0x84, 0x0b, 0x4a,
    0xcc, 0xab, 0x80, 0xe2, 0xe5, 0xf7, 0xc7, 0x9f, 0xc1, 0x27, 0xab, 0x04, 0xcb, 0xd8, 0x5c, 0x31,
    0x15, 0xd3, 0x3c, 0x95, 0x55, 0x77, 0xe2, 0x28, 0x32, 0x70, 0x2b, 0x36, 0x99, 0x6c, 0x7f, 0xaf,
    0x52, 0xeb, 0x16, 0x5c, 0x79, 0x09, 0xdc, 0x80, 0x04, 0x57, 0xea, 0xc6, 0x52, 0x29, 0x83, 0xad,
    0x98, 0xa4, 0x4c, 0xbb, 0x11, 0x95, 0x73, 0x76, 0x3b, 0x45, 0x50, 0x08, 0x74, 0xa3, 0x29, 0xca,
    0x0d, 0x40, 0x62, 0xc1, 0xb2, 0xef, 0x46, 0x91, 0xd5, 0xd0, 0x76, 0x2a, 0x7a, 0x90, 0xd0, 0x36,
    0x9f, 0x74, 0xcf, 0xcf, 0x8a, 0x67, 0xeb, 0x7c, 0xbf, 0x7b, 0x7e, 0x56, 0x0a, 0x5b, 0xe7, 0x1f,
    0xba, 0xe7, 0x67, 0x45, 0xac, 0x75, 0xfe, 0xb1, 0x7b, 0x7e, 0x56, 0xcb, 0xda, 0x05, 0x90, 0x40,
    0x4b, 0x59, 0xb5, 0x49, 0x5a, 0xac, 0x74, 0xcf, 0x42, 0x28, 0x07, 0xc1, 0xef, 0x44, 0xf1, 0xc8,
    0x12, 0x9e, 0xca, 0xca, 0x09, 0x1d, 0x56, 0x30, 0x84, 0x37, 0x1f, 0xde, 0xbd, 0x05, 0x7c, 0xc3,
    0x72, 0x1b, 0x66, 0xd8, 0x9d, 0x52, 0xc8, 0x87, 0x26, 0x3a, 0xa9, 0x41, 0xf3, 0x1c, 0x43, 0x61,
    0xa8, 0xbc, 0x7a, 0x5e, 0x28, 0xdd, 0x40, 0xff, 0x0f, 0x2f, 0x3f, 0xd6, 0xe9, 0x83, 0x84, 0x70,
    0x54, 0x31, 0xea, 0xba, 0x73, 0x69, 0xdb, 0x43, 0x14, 0xf7, 0xd9, 0x02, 0xa4, 0x07, 0xaf, 0x85,
    0x00, 0xaf, 0xe8, 0xe0, 0x41, 0x0d, 0x3e, 0x7c, 0x01, 0x0e, 0x5d, 0xb7, 0x2c, 0x2a, 0x80, 0x25,
    0x87, 0xd0, 0xc3, 0x43, 0x16, 0x02, 0x1a, 0xd5, 0xa9, 0xae, 0xb2, 0xa2, 0x14, 0xe0, 0x54, 0xc9,
    0xfe, 0xea, 0x5f, 0xde, 0x12, 0x62, 0xda, 0x8f, 0x10, 0xd9, 0xc7, 0x70, 0x1e, 0x3d, 0x3a, 0x81,
    0xc0, 0x76, 0x5b, 0xe6, 0x98, 0x82, 0x2a, 0xdc, 0xc7, 0xd7, 0x12, 0x2f, 0x7d, 0x7e, 0xff, 0x95,
    0x2b, 0x12, 0xea, 0x5e, 0xba, 0xec, 0x37, 0xaf, 0x64, 0xc2, 0x8c, 0xb9, 0x6d, 0x93, 0x98, 0x21,
    0x47, 0x6b, 0x16, 0x74, 0x96, 0xe6, 0x55, 0x90, 0x35, 0x25, 0x39, 0xad, 0x5b, 0x6e, 0x2d, 0x57,
    0x4b, 0xda, 0x6f, 0x9b, 0xda, 0xc6, 0x1b, 0xed, 0x46, 0x7b, 0xb3, 0xc5, 0x9a, 0x15, 0x29, 0x5d,
    0x95, 0x49, 0xd1, 0x5e, 0x66, 0x43, 0xfb, 0x7b, 0x95, 0x06, 0x55, 0x58, 0x7d, 0xc7, 0xfc, 0x4a,
    0x24, 0xbf, 0x92, 0xba, 0x75, 0xdd, 0x0f, 0x76, 0xe9, 0x28, 0x79, 0x67, 0xec, 0x22, 0x92, 0x5d,
    0xe4, 0xbe, 0xb2, 0x4b, 0x27, 0x05, 0x77, 0xc6, 0x2e, 0x5f, 0xb2, 0xcb, 0xbf, 0xaf, 0xec, 0xd2,
    0x39, 0xd0, 0x9d, 0xb1, 0xeb, 0x20, 0xd9, 0x75, 0xb8, 0xaf, 0xec, 0xd2, 0x29, 0xdf, 0x9d, 0xb1,
    0xeb, 0x28, 0xd9, 0x75, 0xbc, 0x4f, 0xec, 0xaa, 0xfa, 0xfa, 0xbc, 0x50, 0xe9, 0xc1, 0x32, 0x6c,
    0xbd, 0x7b, 0x24, 0xdc, 0x91, 0xa4, 0x9c, 0x52, 0x7b, 0x10, 0x93, 0x05, 0xd5, 0x59, 0xf5, 0x08,
    0x0e, 0x44, 0x10, 0x60, 0x60, 0xc9, 0x84, 0xd5, 0xc8, 0x58, 0x1e, 0x91, 0x63, 0x1f, 0x09, 0xd3,
    0x33, 0xf9, 0xa3, 0x11, 0x54, 0x9d, 0x9b, 0x67, 0xb0, 0xea, 0x57, 0x1d, 0x38, 0xa7, 0x04, 0x4f,
    0xb4, 0xc7, 0x24, 0x8e, 0xe1, 0x79, 0xe0, 0x1f, 0x36, 0x2c, 0xf0, 0x47, 0x0a, 0x8d, 0x85, 0x14,
    0xb9, 0x13, 0x3c, 0xab, 0x38, 0x20, 0x76, 0xbd, 0x1a, 0xd4, 0x06, 0xf2, 0xfc, 0x02, 0xda, 0xe0,
    0xc3, 0x73, 0xe8, 0x04, 0x5a, 0xc8, 0x52, 0xc3, 0x63, 0x9f, 0x93, 0xfd, 0xcf, 0x78, 0xbe, 0x24,
    0x33, 0xb4, 0x53, 0x07, 0xfa, 0xde, 0x67, 0x16, 0x70, 0x01, 0xaa, 0x65, 0x17, 0x3a, 0xae, 0x8f,
    0x9d, 0xf2, 0xdf, 0x7e, 0xc5, 0x2c, 0x5a, 0xaf, 0x2f, 0x22, 0xc8, 0x45, 0xf1, 0xd6, 0x68, 0x28,
    0x0f, 0xaf, 0x26, 0x9f, 0x62, 0xba, 0xb6, 0x91, 0x61, 0x08, 0x0e, 0x4f, 0xaa, 0x50, 0x20, 0x6a,
    0xb2, 0x1d, 0x5c, 0x2e, 0x08, 0x40, 0x21, 0xd4, 0xfd, 0x98, 0xf1, 0xda, 0x2a, 0x15, 0x03, 0x6d,
    0x76, 0xc6, 0x85, 0xa8, 0x91, 0x37, 0x3f, 0x41, 0xdb, 0xf1, 0x9f, 0x94, 0x70, 0xd0, 0x8e, 0xa7,
    0xce, 0x68, 0x70, 0x36, 0xc0, 0x2f, 0x39, 0xf2, 0x0e, 0xd8, 0xb2, 0x91, 0xb7, 0xa7, 0x27, 0x27,
    0xe3, 0x04, 0xb4, 0x88, 0x8e, 0xdc, 0xf3, 0x12, 0x98, 0x84, 0x52, 0xcb, 0x36, 0x8f, 0xbf, 0x89,
    0xe0, 0xa1, 0x8e, 0x36, 0x80, 0x77, 0x2c, 0x84, 0x67, 0x81, 0x5a, 0x41, 0xde, 0xc3, 0xf3, 0x68,
    0xf0, 0x0f, 0x08, 0x13, 0x64, 0x20, 0x8f, 0xe7, 0x2c, 0xad, 0x43, 0x60, 0xbd, 0xea, 0xcd, 0x35,
    0x76, 0xe5, 0xb4, 0x65, 0x42, 0x43, 0x3f, 0xe2, 0x00, 0xd6, 0xa7, 0xfd, 0xa8, 0xc5, 0x59, 0x74,
    0x6b, 0x74, 0x97, 0xb9, 0x45, 0x27, 0x1b, 0xab, 0x25, 0xdd, 0xf2, 0x73, 0xf0, 0xe1, 0x8c, 0x0d,
    0x3e, 0xe6, 0xa9, 0xeb, 0xa7, 0x2f, 0x6f, 0x5b, 0x69, 0x8c, 0xae, 0xc2, 0xf3, 0x75, 0xfd, 0x2b,
    0x45, 0x51, 0xe6, 0x30, 0xcc, 0x2e, 0xa9, 0xbd, 0x1b, 0x65, 0xec, 0xf3, 0x47, 0x2a, 0xf4, 0x21,
    0x36, 0xfe, 0x99, 0x65, 0xc5, 0xe1, 0x4f, 0x86, 0xf8, 0x9c, 0x9b, 0xa5, 0xfe, 0xcd, 0xc7, 0xda,
    0xea, 0xf9, 0x1c, 0x08, 0x0b, 0xfa, 0xe2, 0x19, 0x18, 0x45, 0x0b, 0x9c, 0x6d, 0xea, 0xd3, 0x4c,
    0x38, 0xb7, 0x96, 0x4f, 0xbf, 0xe0, 0xe3, 0x30, 0xf8, 0x4f, 0xc8, 0x3f, 0x00, 0x1b, 0xb5, 0x76,
    0x03, 0x20, 0x39, 0x00, 0x00};

static const char *TAG = "camera_httpd";

typedef struct
{
    httpd_req_t *req;
    size_t len;
} jpg_chunking_t;

#define PART_BOUNDARY "123456789000000000000987654321"
static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\nX-Timestamp: %d.%06d\r\n\r\n";

httpd_handle_t stream_httpd = NULL;
httpd_handle_t camera_httpd = NULL;

typedef struct
{
    size_t size;  // number of values used for filtering
    size_t index; // current value index
    size_t count; // value count
    int sum;
    int *values; // array to be filled with values
} ra_filter_t;

static ra_filter_t ra_filter;

static ra_filter_t *ra_filter_init(ra_filter_t *filter, size_t sample_size)
{
    memset(filter, 0, sizeof(ra_filter_t));

    filter->values = (int *)malloc(sample_size * sizeof(int));
    if (!filter->values)
    {
        return NULL;
    }
    memset(filter->values, 0, sample_size * sizeof(int));

    filter->size = sample_size;
    return filter;
}

static int ra_filter_run(ra_filter_t *filter, int value)
{
    if (!filter->values)
    {
        return value;
    }
    filter->sum -= filter->values[filter->index];
    filter->values[filter->index] = value;
    filter->sum += filter->values[filter->index];
    filter->index++;
    filter->index = filter->index % filter->size;
    if (filter->count < filter->size)
    {
        filter->count++;
    }
    return filter->sum / filter->count;
}

static size_t jpg_encode_stream(void *arg, size_t index, const void *data, size_t len)
{
    jpg_chunking_t *j = (jpg_chunking_t *)arg;
    if (!index)
    {
        j->len = 0;
    }
    if (httpd_resp_send_chunk(j->req, (const char *)data, len) != ESP_OK)
    {
        return 0;
    }
    j->len += len;
    return len;
}

static esp_err_t capture_handler(httpd_req_t *req)
{
    camera_fb_t *fb = NULL;
    esp_err_t res = ESP_OK;
    int64_t fr_start = esp_timer_get_time();

    fb = esp_camera_fb_get();

    if (!fb)
    {
        ESP_LOGE(TAG, "Camera capture failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    char ts[32];
    snprintf(ts, 32, "%ld.%06ld", fb->timestamp.tv_sec, fb->timestamp.tv_usec);
    httpd_resp_set_hdr(req, "X-Timestamp", (const char *)ts);

    size_t fb_len = 0;
    if (fb->format == PIXFORMAT_JPEG)
    {
        fb_len = fb->len;
        res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
    }
    else
    {
        jpg_chunking_t jchunk = {req, 0};
        res = frame2jpg_cb(fb, 20, jpg_encode_stream, &jchunk) ? ESP_OK : ESP_FAIL;
        httpd_resp_send_chunk(req, NULL, 0);
        fb_len = jchunk.len;
    }
    esp_camera_fb_return(fb);
    int64_t fr_end = esp_timer_get_time();
    ESP_LOGI(TAG, "JPG: %uB %ums", (uint32_t)(fb_len), (uint32_t)((fr_end - fr_start) / 1000));
    return res;
}

static esp_err_t stream_handler(httpd_req_t *req)
{
    camera_fb_t *fb = NULL;
    struct timeval _timestamp;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len = 0;
    uint8_t *_jpg_buf = NULL;
    char *part_buf[128];

    static int64_t last_frame = 0;
    if (!last_frame)
    {
        last_frame = esp_timer_get_time();
    }

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK)
    {
        return res;
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "X-Framerate", "60");

    while (true)
    {

        fb = esp_camera_fb_get();
        if (!fb)
        {
            ESP_LOGE(TAG, "Camera capture failed");
            res = ESP_FAIL;
        }
        else
        {
            _timestamp.tv_sec = fb->timestamp.tv_sec;
            _timestamp.tv_usec = fb->timestamp.tv_usec;

            if (fb->format != PIXFORMAT_JPEG)
            {
                bool jpeg_converted = frame2jpg(fb, 20, &_jpg_buf, &_jpg_buf_len);
                esp_camera_fb_return(fb);
                fb = NULL;
                if (!jpeg_converted)
                {
                    ESP_LOGE(TAG, "JPEG compression failed");
                    res = ESP_FAIL;
                }
            }
            else
            {
                _jpg_buf_len = fb->len;
                _jpg_buf = fb->buf;
            }
        }
        if (res == ESP_OK)
        {
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        }
        if (res == ESP_OK)
        {
            size_t hlen = snprintf((char *)part_buf, 128, _STREAM_PART, _jpg_buf_len, _timestamp.tv_sec, _timestamp.tv_usec);
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if (res == ESP_OK)
        {
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        }
        if (fb)
        {
            esp_camera_fb_return(fb);
            fb = NULL;
            _jpg_buf = NULL;
        }
        else if (_jpg_buf)
        {
            free(_jpg_buf);
            _jpg_buf = NULL;
        }
        if (res != ESP_OK)
        {
            ESP_LOGE(TAG, "send frame failed failed");
            break;
        }
        int64_t fr_end = esp_timer_get_time();

        int64_t frame_time = fr_end - last_frame;
        last_frame = fr_end;
        frame_time /= 1000;
        uint32_t avg_frame_time = ra_filter_run(&ra_filter, frame_time);
        ESP_LOGI(TAG, "MJPG: %uB %ums (%.1ffps), AVG: %ums (%.1ffps)"

                 ,
                 (uint32_t)(_jpg_buf_len),
                 (uint32_t)frame_time, 1000.0 / (uint32_t)frame_time,
                 avg_frame_time, 1000.0 / avg_frame_time

        );
    }

    last_frame = 0;
    return res;
}

static esp_err_t parse_get(httpd_req_t *req, char **obuf)
{
    char *buf = NULL;
    size_t buf_len = 0;

    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1)
    {
        buf = (char *)malloc(buf_len);
        if (!buf)
        {
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK)
        {
            *obuf = buf;
            return ESP_OK;
        }
        free(buf);
    }
    httpd_resp_send_404(req);
    return ESP_FAIL;
}

////////////
//#define LEDC_DUTY               (4095) // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095
#define USER_LED_PIN 2
static void example_ledc_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_1,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz = 25, // Set output frequency at 5 kHz
        .clk_cfg = LEDC_AUTO_CLK};
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .timer_sel = LEDC_TIMER_1,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = USER_LED_PIN,
        .duty = 4095, // Set duty to 0%
        .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}


////////////
static esp_err_t button_handler(httpd_req_t *req)
{
    char *buf = NULL;
    char variable[3];
    char value[3];

    if (parse_get(req, &buf) != ESP_OK)
    {
        return ESP_FAIL;
    }
    if (httpd_query_key_value(buf, "var", variable, sizeof(variable)) != ESP_OK ||
        httpd_query_key_value(buf, "val", value, sizeof(value)) != ESP_OK)
    {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    free(buf);

    int res = 0;
    static int x_var = 0;
    static int y_var = 0;
    int time_var = 0;

    if ((!strcmp(variable, "w") && (!strcmp(value, "w"))))
    {
        printf("w\n");
        x_var = 1;
        time_var = 200;
    }
    else if ((!strcmp(variable, "s") && (!strcmp(value, "s"))))
    {
        printf("s\n");
        x_var = 1;
        time_var = 250;
    }
    else if ((!strcmp(variable, "a") && (!strcmp(value, "a"))))
    {
        printf("a\n");
        x_var = 1;
        time_var = 500;
    }
    else if ((!strcmp(variable, "d") && (!strcmp(value, "d"))))
    {
        printf("d\n");
        x_var = 1;
        time_var = 750;
    }
    else if ((!strcmp(variable, "x") && (!strcmp(value, "x"))))
    {
        printf("x\n");
        x_var = 1;
        time_var = 1000;
    }
    else if ((!strcmp(variable, "y") && (!strcmp(value, "y"))))
    {
        printf("y\n");
        if (y_var == 0)
        {
            y_var = 1;
            gpio_set_level(USER_LED_PIN, 0);
            example_ledc_init();
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 6550));
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1));
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            gpio_set_direction(USER_LED_PIN, GPIO_MODE_OUTPUT);
            gpio_set_level(USER_LED_PIN, 0);
            y_var = 0;
        }
        else
        {
            y_var = 0;
            gpio_set_direction(USER_LED_PIN, GPIO_MODE_OUTPUT);
            gpio_set_level(USER_LED_PIN, 0);
        }
    }
    else
    {
        ESP_LOGI(TAG, "Unknown command: %s", variable);
        res = -1;
    }

    if (x_var == 1)
    {
        x_var = 1;
        gpio_set_level(USER_LED_PIN, 1);
        vTaskDelay(time_var / portTICK_PERIOD_MS);
        gpio_set_level(USER_LED_PIN, 0);
        x_var = 0;
    }
    else
    {
        x_var = 0;
        gpio_set_level(USER_LED_PIN, 0);
    }

    if (res < 0)
    {
        return httpd_resp_send_500(req);
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

static esp_err_t index_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    sensor_t *s = esp_camera_sensor_get();
    if (s != NULL)
    {

        return httpd_resp_send(req, (const char *)index_ov2640_html_gz, index_ov2640_html_gz_len);
    }
    else
    {
        ESP_LOGE(TAG, "Camera sensor not found");
        return httpd_resp_send_500(req);
    }
}

void startCameraServer()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 16;
    config.lru_purge_enable = true;

    httpd_uri_t index_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = index_handler,
        .user_ctx = NULL};

    httpd_uri_t capture_uri = {
        .uri = "/capture",
        .method = HTTP_GET,
        .handler = capture_handler,
        .user_ctx = NULL};

    httpd_uri_t stream_uri = {
        .uri = "/stream",
        .method = HTTP_GET,
        .handler = stream_handler,
        .user_ctx = NULL};

    httpd_uri_t button_uri = {
        .uri = "/button",
        .method = HTTP_GET,
        .handler = button_handler,
        .user_ctx = NULL};

    ra_filter_init(&ra_filter, 20);

    ESP_LOGI(TAG, "Starting web server on port: '%d'", config.server_port);
    if (httpd_start(&camera_httpd, &config) == ESP_OK)
    {
        httpd_register_uri_handler(camera_httpd, &index_uri);
        httpd_register_uri_handler(camera_httpd, &capture_uri);
        httpd_register_uri_handler(camera_httpd, &button_uri);
    }

    config.server_port += 1;
    config.ctrl_port += 1;
    ESP_LOGI(TAG, "Starting stream server on port: '%d'", config.server_port);
    if (httpd_start(&stream_httpd, &config) == ESP_OK)
    {
        httpd_register_uri_handler(stream_httpd, &stream_uri);
    }
}

#define WIFI_EH_LOG_TAG "WIFI_EH"
#define IP_EH_LOG_TAG "IP_EH"

#define MY_SSID "A1_1514733267"
#define MY_PASS "SdTiOdaS"

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void ip_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

//~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
//
//~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        ESP_LOGI(WIFI_EH_LOG_TAG, "WiFi connecting ...");
        break;

    case WIFI_EVENT_STA_CONNECTED:
        ESP_LOGI(WIFI_EH_LOG_TAG, "WiFi connected !");
        break;

    case WIFI_EVENT_STA_DISCONNECTED:
        ESP_LOGI(WIFI_EH_LOG_TAG, "WiFi lost connection !");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGI(WIFI_EH_LOG_TAG, "WiFi try to reconnect ...");
        esp_wifi_start();   // 3
        esp_wifi_connect(); // 4
        break;

    default:
        ESP_LOGI(WIFI_EH_LOG_TAG, "Unhandled event: ");
        break;
    }
}

//~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
//
//~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
static void ip_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case IP_EVENT_STA_GOT_IP:
        ESP_LOGI(IP_EH_LOG_TAG, "WiFi got IP !");
        break;

    default:
        ESP_LOGI(IP_EH_LOG_TAG, "Unhandled event: ");
        break;
    }
}

//~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
//
//~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=
void wifi_connection()
{
    // 1
    esp_netif_init();                    // TCP/IP init
    esp_event_loop_create_default();     // create event loop
    esp_netif_create_default_wifi_sta(); // create wifi station
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation); //

    // 2
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, ip_event_handler, NULL);

    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = MY_SSID,
            .password = MY_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    esp_wifi_start();   // 3
    esp_wifi_connect(); // 4
}

#define OCTADEVBRD1 1
//#define DEVKITV1 1

#ifdef DEVKITV1
#define CAM_PIN_PWDN -1
#define CAM_PIN_RESET -1 // software reset will be performed
#define CAM_PIN_SIOD 21  // SDA
#define CAM_PIN_SIOC 22  // SCL
#define CAM_PIN_VSYNC 34
#define CAM_PIN_HREF 35
#define CAM_PIN_XCLK 32
#define CAM_PIN_PCLK 33
#define CAM_PIN_D0 36 //27
#define CAM_PIN_D1 17
#define CAM_PIN_D2 16
#define CAM_PIN_D3 5
#define CAM_PIN_D4 19
#define CAM_PIN_D5 18
#define CAM_PIN_D6 39 //12
#define CAM_PIN_D7 4

#elif OCTADEVBRD1
#define CAM_PIN_PWDN -1
#define CAM_PIN_RESET -1 // software reset will be performed
#define CAM_PIN_SIOD 21  // SDA
#define CAM_PIN_SIOC 22  // SCL
#define CAM_PIN_VSYNC 34
#define CAM_PIN_HREF 35
#define CAM_PIN_XCLK 32
#define CAM_PIN_PCLK 33
#define CAM_PIN_D0 18 //27
#define CAM_PIN_D1 36
#define CAM_PIN_D2 39
#define CAM_PIN_D3 19
#define CAM_PIN_D4 5
#define CAM_PIN_D5 17
#define CAM_PIN_D6 16 //12
#define CAM_PIN_D7 4
#endif

#define CONFIG_OV7670_SUPPORT 1

#define CAMERA_LOG_TAG "CAMERA"

void app_main(void)
{
    nvs_flash_init();
    wifi_connection();

//****
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
//****
    
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;

    config.pin_d0 = CAM_PIN_D0;
    config.pin_d1 = CAM_PIN_D1;
    config.pin_d2 = CAM_PIN_D2;
    config.pin_d3 = CAM_PIN_D3;
    config.pin_d4 = CAM_PIN_D4;
    config.pin_d5 = CAM_PIN_D5;
    config.pin_d6 = CAM_PIN_D6;
    config.pin_d7 = CAM_PIN_D7;

    config.pin_xclk = CAM_PIN_XCLK;
    config.pin_pclk = CAM_PIN_PCLK;

    config.pin_vsync = CAM_PIN_VSYNC;
    config.pin_href = CAM_PIN_HREF;

    config.pin_sscb_sda = CAM_PIN_SIOD;
    config.pin_sscb_scl = CAM_PIN_SIOC;

    config.pin_pwdn = CAM_PIN_PWDN;
    config.pin_reset = CAM_PIN_RESET;

    config.xclk_freq_hz = 10000000;
    config.frame_size = FRAMESIZE_QQVGA;
    config.pixel_format = PIXFORMAT_YUV422; // for streaming
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_DRAM;
    config.jpeg_quality = 12;
    config.fb_count = 1;

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        printf("Camera init failed with error 0x%x\n", err);
        return;
    }

    sensor_t *s = esp_camera_sensor_get();

    // drop down frame size for higher initial frame rate
    if (config.pixel_format == PIXFORMAT_JPEG)
    {
        s->set_framesize(s, FRAMESIZE_QVGA);
    }

    startCameraServer();

    gpio_set_level(USER_LED_PIN, 0);
    gpio_set_direction(USER_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(USER_LED_PIN, 0);
    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
