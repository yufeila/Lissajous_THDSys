#include "./lissajous_graph/lissajous_graph.h"
#include <stdio.h>
#include <math.h>

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim2;

extern uint16_t adc_buf[2][N_POINTS * 2]; // [0]和[1]为Ping-Pong缓冲
extern volatile uint8_t buf_ready_q[4];   // 简单环形队列
extern volatile uint8_t head;
extern volatile uint8_t tail;

uint16_t lissajous_graph_state = 0;

/* 将这两个大数组从函数局部变量移到此处，并声明为static */
static uint16_t dot_xy[N_POINTS * 2];
static lissajous_point_t points[N_POINTS];

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

    // 关键改动：将 while 改为 if，避免指令风暴
    if (tail != head)
    {
        uint8_t which = buf_ready_q[tail];
        tail = (tail + 1) & 3;

        uint16_t *p = (which == 0) ? adc_buf[0] : adc_buf[1];
        /* 数组已移至外部，此处不再需要定义 */

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

            SetScreenUpdateEnable(0); // 禁止屏幕更新

            // 设置前景色为淡黄色
            uint16_t bg_color = ((255 >> 3) << 11) | ((255 >> 2) << 5) | (204 >> 3); // RGB565
            SetFcolor(bg_color);
            GUI_RectangleFill(X_SCREEN_MIN, Y_SCREEN_MIN, X_SCREEN_MAX, Y_SCREEN_MAX);
            // 设置前景色为黑色
            SetFcolor(0x0000);
            // 关键改动：使用能正常工作的绘图指令
            GUI_FcolorConDots(dot_xy, N_POINTS);
						
            SetScreenUpdateEnable(1); // 允许屏幕更新，将之前所有绘图操作一次性显示
        }
    }
}

void Phase_Detect(float *phase_val)
{
    /* 仅在李萨如显示打开且有新帧时检测 */
    if (lissajous_graph_state != 1 || phase_val == NULL || tail == head)
		{
				*phase_val = -1.0f; // 返回无效值
        return;
		}

    /* 取出本次要处理的半缓冲标志和指针 */
    uint8_t which = buf_ready_q[tail];
    uint16_t *p   = adc_buf[which];   // 指向 N_POINTS*2 的交错 X,Y 数据

		// 调用核心逻辑
		*phase_val = Phase_Detect_Logic(p, N_POINTS);

    /* 标记本帧已消费，让后续绘图 / 检测处理下一帧 */
    tail = (tail + 1) & 3;
}

/**
 * @brief  相位检测核心算法
 * @param  p           指向交错的X,Y数据的指针 (X0, Y0, X1, Y1, ...)
 * @param  num_points  采样点数
 * @return             计算出的相位差（度），如果无法计算则返回-1.0
 */
float Phase_Detect_Logic(uint16_t* p, int num_points)
{
    /* 1. 扫描本帧求出 X/Y 的 min/max —— O(N_POINTS) */
    uint16_t min_x = 0xFFFF, max_x = 0, min_y = 0xFFFF, max_y = 0;
    for (int i = 0; i < num_points; ++i)
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
    for (int i = 1; i < num_points; ++i)
    {
        uint16_t a = p[(i-1)*2], b = p[i*2];
        if (a < mid_x && b >= mid_x) { idx_x1 = i; break; }
    }
    if (idx_x1 < 0) return -1.0f; // 未找到 X 上升

    /* 3. 从 idx_x1 开始，找 Y 第一次上升沿穿过 mid_y —— idx_y1 */
    int idx_y1 = -1;
    for (int i = idx_x1; i < num_points; ++i)
    {
        uint16_t a = p[(i-1)*2 + 1], b = p[i*2 + 1];
        if (a < mid_y && b >= mid_y) { idx_y1 = i; break; }
    }
    if (idx_y1 < 0) return -1.0f; // 未找到 Y 上升

    /* 4. 再从 idx_y1+1 开始找 Y 下一次同样的上升沿 —— idx_y2 */
    int idx_y2 = -1;
    for (int i = idx_y1 + 1; i < num_points; ++i)
    {
        uint16_t a = p[(i-1)*2 + 1], b = p[i*2 + 1];
        if (a < mid_y && b >= mid_y) { idx_y2 = i; break; }
    }
    if (idx_y2 < 0) return -1.0f; // 未找到完整周期

    /* 5. 计算相位差 = diff / period * 360 */
    int32_t diff   = idx_y1 - idx_x1;
    int32_t period = idx_y2 - idx_y1;
    if (period > 0)
    {
        if (diff < 0) diff += period;  // 如果 Y 领先 X，转到 [0,period)
        return (float)diff * 360.0f / (float)period;
    }

    return -1.0f;
}

/**
 * @brief  相位检测测试函数
 * @param  expected_phase_deg 期望的相位差（单位：度）
 */
void Phase_Detect_Test(float expected_phase_deg)
{
    /* 遵循C89标准，将所有变量声明放在函数开头 */
    uint16_t test_buffer[N_POINTS * 2];
    const float PI = 3.1415926535f;
    float calculated_phase;
    int i;

    printf("\r\n--- Phase Detect Test ---\r\n");
    printf("Expected phase difference: %.2f degrees\r\n", expected_phase_deg);

    // 生成模拟数据
    // X: cos(t)
    // Y: cos(t - phase)
    for (i = 0; i < N_POINTS; i++)
    {
        float angle_rad_x = 2 * PI * i / N_POINTS;
        float angle_rad_y = 2 * PI * i / N_POINTS - (expected_phase_deg * PI / 180.0f);

        // 映射到 [0, 4095] 的 ADC 值范围
        // (cos(x) + 1) * 2047.5
        test_buffer[i*2]     = (uint16_t)((cosf(angle_rad_x) + 1.0f) * 2047.5f);
        test_buffer[i*2 + 1] = (uint16_t)((cosf(angle_rad_y) + 1.0f) * 2047.5f);
    }
    
    // 调用核心逻辑进行计算
    calculated_phase = Phase_Detect_Logic(test_buffer, N_POINTS);

    if (calculated_phase >= 0)
    {
        printf("Calculated phase difference: %.2f degrees\r\n", calculated_phase);
        // 简单比较结果
        if (fabsf(calculated_phase - expected_phase_deg) < 5.0f) // 允许5度的误差
        {
            printf("Result: PASS\r\n");
        }
        else
        {
            printf("Result: FAIL\r\n");
        }
    }
    else
    {
        printf("Phase calculation failed.\r\n");
    }
    printf("--- Test End ---\r\n\r\n");
}