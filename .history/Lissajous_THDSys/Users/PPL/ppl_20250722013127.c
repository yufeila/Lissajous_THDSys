#include "./ppl.h"

void SetPLLMultiply(uint16_t ppl_value)
{
    float VCO_in = 0.0f;

    // 这里需要知道VCO_in - ppl_value 的曲线，需要标定
    // 设 ppl_value = KVCO * VCO_in + b
}