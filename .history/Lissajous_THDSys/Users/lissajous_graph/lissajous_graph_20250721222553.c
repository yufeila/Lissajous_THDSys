#include "./lissajous_graph/lissajous_graph.h"

extern volatile uint16_t adc_buffer[N_RAW];
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim2;

void Lissajous_Graph_Start(void)
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, N_RAW);
    HAL_TIM_Base_Start(&htim2);
}

voi

void Lissajous_Graph_Display(void)
{
    if(ADC3_BufferReadyFlag == BUFFER_READY_FLAG_FULL)
}