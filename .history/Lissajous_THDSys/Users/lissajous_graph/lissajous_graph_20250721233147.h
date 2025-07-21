#ifndef __LISSAJOUS_GRAPH_H__
#define __LISSAJOUS_GRAPH_H__

#include "stm32f4xx_hal.h"
#include "./serial_screen/hmi_driver.h"
#include "./serial_screen/cmd_queue.h"
#include "./serial_screen/cmd_process.h"
#include "tim.h"
#include "adc.h"

typedef struct {
    float x_voltage;   // X通道电压
    float y_voltage;   // Y通道电压
    uint16_t x_bin;    // X通道采样值
    uint16_t y_bin;    // Y通道采样值
    uint16_t x_label;  // 屏幕横坐标
    uint16_t y_label;  // 屏幕纵坐标
} lissajous_point_t;

#define LISSAJOUS_POINT_NUM  200   // 显示点数
#define X_SCREEN_MIN         250    // 屏幕X轴起点
#define X_SCREEN_MAX         1000   // 屏幕X轴终点
#define X_WIDTH              1000   // 屏幕X轴宽度
#define Y_SCREEN_MIN         32
#define Y_SCREEN_MAX         220
#define Y_WIDTH              200    // 屏幕Y轴宽度
#define ADC_REF_VOLTAGE      3.3f
#define ADC_RESOLUTION       4096.0f

void Lissajous_Graph_Display(void);

#endif