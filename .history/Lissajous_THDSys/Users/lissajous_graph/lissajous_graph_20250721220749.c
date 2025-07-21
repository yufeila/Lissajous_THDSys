#include "./lissajous_graph/lissajous_graph.h"

extern volatile uint16_t adc_buffer[N_RAW];
ex ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;


void Lissajous_Graph_Display(void)
{
    if(ADC3_BufferReadyFlag == BUFFER_READY_FLAG_FULL)
}