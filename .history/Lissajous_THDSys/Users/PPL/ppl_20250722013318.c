#include "./ppl.h"

void SetPLLMultiply(uint16_t ppl_value)
{
    float VCO_in = 0.0f;

    // 这里需要知道VCO_in - ppl_value 的曲线，需要标定
    // 设 ppl_value = KVCO * VCO_in + FMIN;
    VCO_in = (ppl_value - FMIN) / KVCO;

    uint16_t dac_value = (uint16_t)((Vref / 3.3f) * 4095);
}