#ifndef __LISSAJOUS_GRAPH_H__
#define __LISSAJOUS_GRAPH_H__

#include "stm32f4xx_hal.h"
#include "./serial_screen/hmi_driver.h"
#include "./serial_screen/cmd_queue.h"
#include "./serial_screen/cmd_process.h"
#include "tim.h"
#include "adc.h"

typedef struct {
    float x_voltage;   // Xͨ����ѹ
    float y_voltage;   // Yͨ����ѹ
    uint16_t x_bin;    // Xͨ������ֵ
    uint16_t y_bin;    // Yͨ������ֵ
    uint16_t x_label;  // ��Ļ������
    uint16_t y_label;  // ��Ļ������
} lissajous_point_t;

#define LISSAJOUS_POINT_NUM  200   // ��ʾ����
#define X_SCREEN_MIN         250    // ��ĻX�����
#define X_SCREEN_MAX         1000   // ��ĻX���յ�
#define X_WIDTH              750   // ��ĻX�����
#define Y_SCREEN_MIN         30     // ��ĻY�����
#define Y_SCREEN_MAX         570    // ��ĻY���յ�
#define Y_WIDTH              540    // ��ĻY�����
#define X_LABEL_MIN          250    // ��ĻX���ǩ���
#define X_LABEL_MAX          1000   // ��ĻX���ǩ�յ�
#define Y_LABEL_MIN          30     // ��ĻY���ǩ���
#define Y_LABEL_MAX          570    // ��ĻY���ǩ�յ�
#define ADC_REF_VOLTAGE      3.3f
#define ADC_RESOLUTION       4096.0f

void Lissajous_Graph_Display(void);

#endif