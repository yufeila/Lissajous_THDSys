#include "./lissajous_graph/lissajous_graph.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim2;

uint16_t lissajous_graph_state = 0;

void Lissajous_Graph_Start(void)
{
    lissajous_graph_state = 1;
    // 启动双缓冲DMA
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buf[0], N_POINTS * 2 * 2); // 2帧缓冲
    HAL_TIM_Base_Start(&htim2);
}


void Lissajous_Graph_Stop(void)
{
    lissajous_graph_state = 0;
        HAL_ADC_Stop_DMA(&hadc3);
    HAL_TIM_Base_Stop(&htim3);
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

        // 获取adc1, adc2的采样数据

    }
}