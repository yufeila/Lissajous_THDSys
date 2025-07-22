#include "./duty/duty_set.h"
#include "dac.h" // ȷ�������� DAC ���ͷ�ļ�
#include "tim.h" // ȷ�������� TIM ���ͷ�ļ�
#include "./ad9833/bsp_ad9833.h"
#include <stdio.h>

extern DAC_HandleTypeDef hdac; // DAC �����ͨ���� dac.c �ﶨ��
extern TIM_HandleTypeDef htim4; // TIM4 �����ͨ���� tim.c �ﶨ��
extern float duty; // tim.c �ﶨ���


void SetDuty(uint16_t duty)
{
    printf("SetDuty: duty = %.2f%%\r\n", (float)duty);
    float fduty = (float)duty / 100.0f;
    float Vref = DDS_OUT_VPP * (1 - fduty);

    // ���� Vref ��Ҫӳ�䵽 0~3.3V DAC ���
    // �������Ҫ����ʵ�ʵ�·����Vref�ķ�Χ
    if (Vref < 0) Vref = 0;
    if (Vref > 3.3f) Vref = 3.3f;

    uint16_t dac_value = (uint16_t)((Vref / 3.3f) * 4095);

    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

    AD9833_Config(1000.0, AD9833_OUT_TRIANGLE, 0);
}

void MeasureDuty(float * duty_value)
{
    printf("MeasureDuty: Starting measurement on PD12...\r\n");
    
    // ����dutyֵ
    duty = 0.0f;
    PWM_RisingCount = 0;
    PWM_FallingCount = 0;
    
    // ������TIM4������ʱ��
    HAL_TIM_Base_Start(&htim4);
    printf("MeasureDuty: TIM4 base started\r\n");
    
    // ���� TIM4 ���벶��(������ͨ��1��2)
    if (HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1) != HAL_OK) {
        printf("MeasureDuty: Failed to start TIM4 CH1!\r\n");
    } else {
        printf("MeasureDuty: TIM4 CH1 started\r\n");
    }
    
    if (HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_2) != HAL_OK) {
        printf("MeasureDuty: Failed to start TIM4 CH2!\r\n");
    } else {
        printf("MeasureDuty: TIM4 CH2 started\r\n");
    }

    // �ȴ��жϻص��Զ����� duty
    // ���������ʱ����ѯ���ź����ȷ�ʽ�ȴ�һ��ʱ��
    HAL_Delay(100); // ���ӵ�100ms��ȷ�������ź�

    // ��ȡȫ�ֱ��� duty
    *duty_value = duty;

    // ��������ֹͣ���񣬽�ʡ����
    HAL_TIM_IC_Stop_IT(&htim4, TIM_CHANNEL_1);
    HAL_TIM_IC_Stop_IT(&htim4, TIM_CHANNEL_2);
    HAL_TIM_Base_Stop(&htim4);

    // �������� measuredDuty
    printf("MeasureDuty: Final values - Rising:%d, Falling:%d, duty = %.2f%%\r\n", 
           PWM_RisingCount, PWM_FallingCount, (*duty_value) * 100.0f);
    printf("\r\n");
}







