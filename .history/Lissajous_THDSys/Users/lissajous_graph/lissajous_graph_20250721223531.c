#include "./lissajous_graph/lissajous_graph.h"

extern volatile uint16_t adc_buffer[N_RAW];
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim2;

uint16_t lissajous_graph_state = 0;

void Lissajous_Graph_Start(void)
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, N_RAW);
    HAL_TIM_Base_Start(&htim2);
    lissajous_graph_state = 1;
}

void Lissajous_Graph_Stop(void)
{
    HAL_ADC_Stop_DMA(&hadc1);
    HAL_TIM_Base_Stop(&htim2);
    lissajous_graph_state = 0;
}

void Lissajous_Graph_Display(uint16_t state)
{
    static uint16_t first_start = 1;
    if(state == 1)
    {
        if(first_start)
        {
            first_start = 0;
            HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, N_RAW);
            HAL_TIM_Base_Start(&htim2);
        }
        else
        {
            HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, N_RAW);
            HAL_TIM_Base_Start(&htim2);
        }
    }
}