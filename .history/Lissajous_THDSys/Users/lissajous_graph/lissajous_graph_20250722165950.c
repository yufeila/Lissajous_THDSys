#include "./lissajous_graph/lissajous_graph.h"
#include <stdio.h>
#include <math.h>

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim2;

extern uint16_t adc_buf[2][N_POINTS * 2]; // [0]��[1]ΪPing-Pong����
extern volatile uint8_t buf_ready_q[4];   // �򵥻��ζ���
extern volatile uint8_t head;
extern volatile uint8_t tail;

uint16_t lissajous_graph_state = 0;

/* ��������������Ӻ����ֲ������Ƶ��˴���������Ϊstatic */
static uint16_t dot_xy[N_POINTS * 2];
static lissajous_point_t points[N_POINTS];

void Lissajous_Graph_Start(void)
{
    lissajous_graph_state = 1;
    // ����˫����DMA
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buf[0], N_POINTS * 2 * 2); // 2֡����
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
    // ӳ�䵽��Ļ����
    pt->x_label = X_SCREEN_MIN + (uint16_t)((pt->x_voltage / ADC_REF_VOLTAGE) * (X_WIDTH));
    pt->y_label = Y_SCREEN_MIN + (uint16_t)((pt->y_voltage / ADC_REF_VOLTAGE) * (Y_HEIGHT));
}

void Lissajous_Graph_Display(uint16_t state)
{
    static uint8_t frame_skip = 0;
    if(state != 1) return;

    // �ؼ��Ķ����� while ��Ϊ if������ָ��籩
    if (tail != head)
    {
        uint8_t which = buf_ready_q[tail];
        tail = (tail + 1) & 3;

        uint16_t *p = (which == 0) ? adc_buf[0] : adc_buf[1];
        /* �����������ⲿ���˴�������Ҫ���� */

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

            SetScreenUpdateEnable(0); // ��ֹ��Ļ����

            // ����ǰ��ɫΪ����ɫ
            uint16_t bg_color = ((255 >> 3) << 11) | ((255 >> 2) << 5) | (204 >> 3); // RGB565
            SetFcolor(bg_color);
            GUI_RectangleFill(X_SCREEN_MIN, Y_SCREEN_MIN, X_SCREEN_MAX, Y_SCREEN_MAX);
            // ����ǰ��ɫΪ��ɫ
            SetFcolor(0x0000);
            // �ؼ��Ķ���ʹ�������������Ļ�ͼָ��
            GUI_FcolorConDots(dot_xy, N_POINTS);
						
            SetScreenUpdateEnable(1); // ������Ļ���£���֮ǰ���л�ͼ����һ������ʾ
        }
    }
}

void Phase_Detect(float *phase_val)
{
    /* ������������ʾ��������֡ʱ��� */
    if (lissajous_graph_state != 1 || phase_val == NULL || tail == head)
		{
				*phase_val = -1.0f; // ������Чֵ
        return;
		}

    /* ȡ������Ҫ����İ뻺���־��ָ�� */
    uint8_t which = buf_ready_q[tail];
    uint16_t *p   = adc_buf[which];   // ָ�� N_POINTS*2 �Ľ��� X,Y ����

		// ���ú����߼�
		*phase_val = Phase_Detect_Logic(p, N_POINTS);

    /* ��Ǳ�֡�����ѣ��ú�����ͼ / ��⴦����һ֡ */
    tail = (tail + 1) & 3;
}

/**
 * @brief  ��λ�������㷨
 * @param  p           ָ�򽻴��X,Y���ݵ�ָ�� (X0, Y0, X1, Y1, ...)
 * @param  num_points  ��������
 * @return             ���������λ��ȣ�������޷������򷵻�-1.0
 */
float Phase_Detect_Logic(uint16_t* p, int num_points)
{
    /* 1. ɨ�豾֡��� X/Y �� min/max ���� O(N_POINTS) */
    uint16_t min_x = 0xFFFF, max_x = 0, min_y = 0xFFFF, max_y = 0;
    for (int i = 0; i < num_points; ++i)
    {
        uint16_t xv = p[i*2], yv = p[i*2 + 1];
        if (xv < min_x) min_x = xv;
        if (xv > max_x) max_x = xv;
        if (yv < min_y) min_y = yv;
        if (yv > max_y) max_y = yv;
    }
    /* ���� = (min + max) / 2 */
    uint16_t mid_x = (min_x + max_x) >> 1;
    uint16_t mid_y = (min_y + max_y) >> 1;

    /* 2. �� X ��һ�������ش��� mid_x ���� idx_x1 */
    int idx_x1 = -1;
    for (int i = 1; i < num_points; ++i)
    {
        uint16_t a = p[(i-1)*2], b = p[i*2];
        if (a < mid_x && b >= mid_x) { idx_x1 = i; break; }
    }
    if (idx_x1 < 0) return -1.0f; // δ�ҵ� X ����

    /* 3. �� idx_x1 ��ʼ���� Y ��һ�������ش��� mid_y ���� idx_y1 */
    int idx_y1 = -1;
    for (int i = idx_x1; i < num_points; ++i)
    {
        uint16_t a = p[(i-1)*2 + 1], b = p[i*2 + 1];
        if (a < mid_y && b >= mid_y) { idx_y1 = i; break; }
    }
    if (idx_y1 < 0) return -1.0f; // δ�ҵ� Y ����

    /* 4. �ٴ� idx_y1+1 ��ʼ�� Y ��һ��ͬ���������� ���� idx_y2 */
    int idx_y2 = -1;
    for (int i = idx_y1 + 1; i < num_points; ++i)
    {
        uint16_t a = p[(i-1)*2 + 1], b = p[i*2 + 1];
        if (a < mid_y && b >= mid_y) { idx_y2 = i; break; }
    }
    if (idx_y2 < 0) return -1.0f; // δ�ҵ���������

    /* 5. ������λ�� = diff / period * 360 */
    int32_t diff   = idx_y1 - idx_x1;
    int32_t period = idx_y2 - idx_y1;
    if (period > 0)
    {
        if (diff < 0) diff += period;  // ��� Y ���� X��ת�� [0,period)
        return (float)diff * 360.0f / (float)period;
    }

    return -1.0f;
}

/**
 * @brief  ��λ�����Ժ���
 * @param  expected_phase_deg ��������λ���λ���ȣ�
 */
void Phase_Detect_Test(float expected_phase_deg)
{
    /* ��ѭC89��׼�������б����������ں�����ͷ */
    uint16_t test_buffer[N_POINTS * 2];
    const float PI = 3.1415926535f;
    float calculated_phase;
    int i;

    printf("\r\n--- Phase Detect Test ---\r\n");
    printf("Expected phase difference: %.2f degrees\r\n", expected_phase_deg);

    // ����ģ������
    // X: cos(t)
    // Y: cos(t - phase)
    for (i = 0; i < N_POINTS; i++)
    {
        float angle_rad_x = 2 * PI * i / N_POINTS;
        float angle_rad_y = 2 * PI * i / N_POINTS - (expected_phase_deg * PI / 180.0f);

        // ӳ�䵽 [0, 4095] �� ADC ֵ��Χ
        // (cos(x) + 1) * 2047.5
        test_buffer[i*2]     = (uint16_t)((cosf(angle_rad_x) + 1.0f) * 2047.5f);
        test_buffer[i*2 + 1] = (uint16_t)((cosf(angle_rad_y) + 1.0f) * 2047.5f);
    }
    
    // ���ú����߼����м���
    calculated_phase = Phase_Detect_Logic(test_buffer, N_POINTS);

    if (calculated_phase >= 0)
    {
        printf("Calculated phase difference: %.2f degrees\r\n", calculated_phase);
        // �򵥱ȽϽ��
        if (fabsf(calculated_phase - expected_phase_deg) < 5.0f) // ����5�ȵ����
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