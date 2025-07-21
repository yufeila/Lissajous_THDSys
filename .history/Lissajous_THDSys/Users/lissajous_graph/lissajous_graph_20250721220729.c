#include "./lissajous_graph/lissajous_graph.h"

extern volatile uint16_t adc_buffer[N_RAW];
extern volatile uint8_t ADC3_BufferReadyFlag;
extern DMA_HandleTypeDef hdma_adc3;
extern ADC_HandleTypeDef hadc3;
extern TIM_HandleTypeDef htim3;

void Lissajous_Graph_Display(void)
{
    if(ADC3_BufferReadyFlag == BUFFER_READY_FLAG_FULL)
}