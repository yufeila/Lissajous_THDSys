#include "./duty/duty_set.h"
#include "dac.h" // 确保包含了 DAC 相关头文件
#include "./ad9833/bsp_ad9833.h"


extern DAC_HandleTypeDef hdac; // DAC 句柄，通常在 dac.c 里定义

void SetDuty(uint16_t duty)
{
    float fduty = (float)duty / 100.0f;
    float Vref = DDS_OUT_VPP * (1 - fduty);

    // 假设 Vref 需要映射到 0~3.3V DAC 输出
    // 你可能需要根据实际电路调整Vref的范围
    if (Vref < 0) Vref = 0;
    if (Vref > 3.3f) Vref = 3.3f;

    uint16_t dac_value = (uint16_t)((Vref / 3.3f) * 4095);

    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

    AD9833_SetFrequencyQuick(1000.0,AD9833_OUT_S;
}







