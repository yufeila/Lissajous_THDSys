#include "./duty/duty_set.h"
#include "dac.h" // ȷ�������� DAC ���ͷ�ļ�
#include "./ad9833/bsp_ad9833.h"


extern DAC_HandleTypeDef hdac; // DAC �����ͨ���� dac.c �ﶨ��

void SetDuty(uint16_t duty)
{
    float fduty = (float)duty / 100.0f;
    float Vref = DDS_OUT_VPP * (1 - fduty);

    // ���� Vref ��Ҫӳ�䵽 0~3.3V DAC ���
    // �������Ҫ����ʵ�ʵ�·����Vref�ķ�Χ
    if (Vref < 0) Vref = 0;
    if (Vref > 3.3f) Vref = 3.3f;

    uint16_t dac_value = (uint16_t)((Vref / 3.3f) * 4095);

    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

    AD9833_SetFrequencyQuick(1000.0,AD9833_OUT_S;
}







