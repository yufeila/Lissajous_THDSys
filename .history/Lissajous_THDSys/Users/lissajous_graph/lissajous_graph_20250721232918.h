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



void Lissajous_Graph_Display(void);

#endif