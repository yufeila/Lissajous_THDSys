#include "./lissajous_graph/lissajous_graph.h"

extern volatile uint16_t adc_buffer[N_RAW];
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc1;
ex


void Lissajous_Graph_Display(void)
{
    if(ADC3_BufferReadyFlag == BUFFER_READY_FLAG_FULL)
}