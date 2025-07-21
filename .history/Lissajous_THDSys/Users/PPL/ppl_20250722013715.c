#include "./ppl.h"
#include "dac.h"

extern DAC_HandleTypeDef hdac;

void SetPLLMultiply(uint16_t ppl_value)
{
    float VCO_in = 0.0f;

    // 这里需要知道VCO_in - ppl_value 的曲线，需要标定
    // 设 ppl_value = KVCO * VCO_in + FMIN;
    VCO_in = (ppl_value - FMIN) / KVCO;

    if (VCO_in < 0) VCO_in = 0;
    if (VCO_in > 3.3f) VCO_in = 3.3f;

    uint16_t vco_value = (uint16_t)((VCO_in / 3.3f) * 4095);

    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, vco_value);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
}

void Set_Prescaler(uint16_t prescaler_value)
{
    static uint16_t HC161_inited = 0;
    if(!HC161_inited)
    {
        HC161N_Init();
    }
    uint16_t prescaler_value = (uint16_t)((VCO_in / 3.3f) * 4095);

    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, prescaler_value);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
}