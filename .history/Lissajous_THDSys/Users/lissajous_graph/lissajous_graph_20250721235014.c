#include "./lissajous_graph/lissajous_graph.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim2;

uint16_t lissajous_graph_state = 0;

void Lissajous_Graph_Start(void)
{
    lissajous_graph_state = 1;
    // Æô¶¯Ë«»º³åDMA
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buf[0], N_POINTS * 2 * 2); // 2Ö¡»º³å
    HAL_TIM_Base_Start(&htim2);
}


void Lissajous_Graph_Stop(void)
{
    lissajous_graph_state = 0;
    HAL_ADC_Stop_DMA(&hadc1);
    HAL_TIM_Base_Stop(&htim2);
}

void Lissajous_Point_Config(lissajous_point_t* pt, uint16_t x_adc, uint16_t y_adc)
{
    pt->x_bin = x_adc;
    pt->y_bin = y_adc;
    pt->x_voltage = (float)x_adc * ADC_REF_VOLTAGE / ADC_RESOLUTION;
    pt->y_voltage = (float)y_adc * ADC_REF_VOLTAGE / ADC_RESOLUTION;
    // Ó³Éäµ½ÆÁÄ»×ø±ê
    pt->x_label = X_SCREEN_MIN + (uint16_t)((pt->x_voltage / ADC_REF_VOLTAGE) * (X_WIDTH));
    pt->y_label = Y_SCREEN_MIN + (uint16_t)((pt->y_voltage / ADC_REF_VOLTAGE) * (Y_HEIGHT));
}

void Lissajous_Graph_Display(uint16_t state)
{
    if(state != 1) return;

    while (tail != head)
    {
        uint8_t which = buf_ready_q[tail];
        tail = (tail + 1) & 3;

        uint16_t *p = (which == 0) ? adc_buf[0] : adc_buf[1];
        uint16_t dot_xy[N_POINTS * 2];
        lissajous_point_t points[N_POINTS];

        for (int i = 0; i < N_POINTS; ++i)
        {
            uint16_t x_adc = p[i*2];
            uint16_t y_adc = p[i*2 + 1];

            Lissajous_Point_Config(&points[i], x_adc, y_adc);

            dot_xy[i*2]     = points[i].x_label;
            dot_xy[i*2 + 1] = points[i].y_label;
        }

        GUI_ConDots(0, dot_xy, N_POINTS);
    }
}
