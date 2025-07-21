#include "./lissajous_graph/lissajous_graph.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim2;

extern uint16_t adc_buf[2][N_POINTS * 2]; // [0]和[1]为Ping-Pong缓冲
extern volatile uint8_t buf_ready_q[4];   // 简单环形队列
extern volatile uint8_t head;
extern volatile uint8_t tail;

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
    HAL_ADC_Stop_DMA(&hadc1);
    HAL_TIM_Base_Stop(&htim2);
}

void Lissajous_Point_Config(lissajous_point_t* pt, uint16_t x_adc, uint16_t y_adc)
{
    pt->x_bin = x_adc;
    pt->y_bin = y_adc;
    pt->x_voltage = (float)x_adc * ADC_REF_VOLTAGE / ADC_RESOLUTION;
    pt->y_voltage = (float)y_adc * ADC_REF_VOLTAGE / ADC_RESOLUTION;
    // 映射到屏幕坐标
    pt->x_label = X_SCREEN_MIN + (uint16_t)((pt->x_voltage / ADC_REF_VOLTAGE) * (X_WIDTH));
    pt->y_label = Y_SCREEN_MIN + (uint16_t)((pt->y_voltage / ADC_REF_VOLTAGE) * (Y_HEIGHT));
}

void Lissajous_Graph_Display(uint16_t state)
{
    static uint8_t frame_skip = 0;
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

        if (++frame_skip >= UPDATE_INTERVAL) {
            frame_skip = 0;
            GUI_CleanScreen();
            GUI_ConDots(0, dot_xy, N_POINTS);
        }
    }
}

void Phase_Detect(float *phase_val)
{
    /* 仅在李萨如显示打开且有新帧时检测 */
    if (lissajous_graph_state != 1 || phase_val == NULL || tail == head)
        return;

    /* 取出本次要处理的半缓冲标志和指针 */
    uint8_t which = buf_ready_q[tail];
    uint16_t *p   = adc_buf[which];   // 指向 N_POINTS*2 的交错 X,Y 数据

    /* 1. 扫描本帧求出 X/Y 的 min/max —— O(N_POINTS) */
    uint16_t min_x = 0xFFFF, max_x = 0, min_y = 0xFFFF, max_y = 0;
    for (int i = 0; i < N_POINTS; ++i)
    {
        uint16_t xv = p[i*2], yv = p[i*2 + 1];
        if (xv < min_x) min_x = xv;
        if (xv > max_x) max_x = xv;
        if (yv < min_y) min_y = yv;
        if (yv > max_y) max_y = yv;
    }
    /* 中线 = (min + max) / 2 */
    uint16_t mid_x = (min_x + max_x) >> 1;
    uint16_t mid_y = (min_y + max_y) >> 1;

    /* 2. 找 X 第一次上升沿穿过 mid_x —— idx_x1 */
    int idx_x1 = -1;
    for (int i = 1; i < N_POINTS; ++i)
    {
        uint16_t a = p[(i-1)*2], b = p[i*2];
        if (a < mid_x && b >= mid_x) { idx_x1 = i; break; }
    }
    if (idx_x1 < 0) goto DONE;  // 本帧未找到 X 上升

    /* 3. 从 idx_x1 开始，找 Y 第一次上升沿穿过 mid_y —— idx_y1 */
    int idx_y1 = -1;
    for (int i = idx_x1; i < N_POINTS; ++i)
    {
        uint16_t a = p[(i-1)*2 + 1], b = p[i*2 + 1];
        if (a < mid_y && b >= mid_y) { idx_y1 = i; break; }
    }
    if (idx_y1 < 0) goto DONE;  // 本帧未找到 Y 上升

    /* 4. 再从 idx_y1+1 开始找 Y 下一次同样的上升沿 —— idx_y2 */
    int idx_y2 = -1;
    for (int i = idx_y1 + 1; i < N_POINTS; ++i)
    {
        uint16_t a = p[(i-1)*2 + 1], b = p[i*2 + 1];
        if (a < mid_y && b >= mid_y) { idx_y2 = i; break; }
    }
    if (idx_y2 < 0) goto DONE;  // 本帧未找到完整周期

    /* 5. 计算相位差 = diff / period * 360 */
    int32_t diff   = idx_y1 - idx_x1;
    int32_t period = idx_y2 - idx_y1;
    if (period > 0)
    {
        if (diff < 0) diff += period;  // 如果 Y 领先 X，转到 [0,period)
        *phase_val = (float)diff * 360.0f / (float)period;
    }

DONE:
    /* 标记本帧已消费，让后续绘图 / 检测处理下一帧 */
    tail = (tail + 1) & 3;
}