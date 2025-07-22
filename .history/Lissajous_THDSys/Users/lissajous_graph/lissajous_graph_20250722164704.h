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
#define X_WIDTH              750   // ��ĻX����

#define Y_SCREEN_MIN         30     // ��ĻY�����
#define Y_SCREEN_MAX         570    // ��ĻY���յ�
#define Y_HEIGHT             540    // ��ĻY��߶�

#define ADC_REF_VOLTAGE      3.3f   // �ο���ѹ
#define ADC_RESOLUTION       4096.0f // �ֱ���
#define UPDATE_INTERVAL      1     // ���¼������λ��֡

/* ȫ�ֱ������� */
extern uint16_t lissajous_graph_state;
extern uint16_t adc_buf[2][200 * 2]; // [0]��[1]ΪPing-Pong����
extern volatile uint8_t buf_ready_q[4];   // �򵥻��ζ���
extern volatile uint8_t head;
extern volatile uint8_t tail;



void Lissajous_Graph_Start(void);
void Lissajous_Graph_Stop(void);
void Lissajous_Graph_Display(uint16_t state);
void Phase_Detect(float *phase_val);
float Phase_Detect_Logic(uint16_t* p, int num_points);
void Phase_Detect_Test(float expected_phase_deg);

#endif
