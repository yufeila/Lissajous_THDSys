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
    static uint8_t HC161N_inited = 0;
    if(!HC161N_inited)
    {
        HC161N_Init();
    }

    switch(prescaler_value)
    {
        case 16:
            // 0000
            P0(0); P1(0); P2(0); P3(0);
            break;
        case 15:
            // 0001
            P0(0); P1(0); P2(0); P3(1);
            break;
        case 14:
            // 0010
            P0(0); P1(0); P2(1); P3(0);
            break;
        case 13:
            // 0011
            P0(0); P1(0); P2(1); P3(1);
            break;
        case 12:
            // 0100
            P0(0); P1(1); P2(0); P3(0);
            break;
        case 11:
            // 0101
            P0(0); P1(1); P2(0); P3(1);
            break;
        case 10:
            // 0110
    }
}

void HC161N_Init(void)
{
    
}