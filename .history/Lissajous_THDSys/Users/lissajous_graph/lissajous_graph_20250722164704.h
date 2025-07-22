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
#define X_WIDTH              750   // 屏幕X轴宽度

#define Y_SCREEN_MIN         30     // 屏幕Y轴起点
#define Y_SCREEN_MAX         570    // 屏幕Y轴终点
#define Y_HEIGHT             540    // 屏幕Y轴高度

#define ADC_REF_VOLTAGE      3.3f   // 参考电压
#define ADC_RESOLUTION       4096.0f // 分辨率
#define UPDATE_INTERVAL      1     // 更新间隔，单位：帧

/* 全局变量声明 */
extern uint16_t lissajous_graph_state;
extern uint16_t adc_buf[2][200 * 2]; // [0]和[1]为Ping-Pong缓冲
extern volatile uint8_t buf_ready_q[4];   // 简单环形队列
extern volatile uint8_t head;
extern volatile uint8_t tail;



void Lissajous_Graph_Start(void);
void Lissajous_Graph_Stop(void);
void Lissajous_Graph_Display(uint16_t state);
void Phase_Detect(float *phase_val);
float Phase_Detect_Logic(uint16_t* p, int num_points);
void Phase_Detect_Test(float expected_phase_deg);

#endif
