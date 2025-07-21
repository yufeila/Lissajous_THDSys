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

    Set_Prescaler(ppl_value);

}

void Set_Prescaler(uint16_t prescaler_value)
{
    static uint8_t HC161N_inited = 0;
    if(!HC161N_inited)
    {
        HC161N_Init();
        CD4052_Init();
        CD4053_Init();
        HC161N_inited = 1;
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
            P0(0); P1(1); P2(1); P3(0);
            break;
        case 9:
            // 0111
            P0(0); P1(1); P2(1); P3(1);
            break;
        case 8:
            // 1000
            P0(1); P1(0); P2(0); P3(0);
            break;
        case 7:
            // 1001
            P0(1); P1(0); P2(0); P3(1);
            break;
        case 6:
            // 1010
            P0(1); P1(0); P2(1); P3(0);
            break;
        case 5:
            // 1011
            P0(1); P1(0); P2(1); P3(1);
            CD;CD4052_A(0); CD4052_B(0);
            break;
        case 4:
            // 1100
            P0(1); P1(1); P2(0); P3(0);
            CD4052_A(0); CD4052_B(1);
            break;
        case 3:
            // 1101
            P0(1); P1(1); P2(0); P3(1);
            CD4052_A(1); CD4052_B(0);
            break;
        case 2:
            // 1110
            P0(1); P1(1); P2(1); P3(0);
            CD4052_A(1); CD4052_B(1);
            break;
        case 1:
            // 1111
            P0(1); P1(1); P2(1); P3(1);
            CD4052_A(1); CD4052_B(1);
            break;
        default:
            // 0000
            P0(0); P1(0); P2(0); P3(0);
            break;
    }
}

void HC161N_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = HC161N_P0_PIN | HC161N_P1_PIN | HC161N_P2_PIN | HC161N_P3_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);   
}


void CD4052_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = CD4052_A_PIN | CD4052_B_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void CD4053_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = CD4053_A_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}