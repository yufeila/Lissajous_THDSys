#include "./fft_hp_estimate/fft_hp_estimate.h"
#include "usart.h"
#include "adc.h"
#include <stdio.h>
#include <math.h>

static float      win[N_RAW];          // Hann(n)
static float      buf[N_FFT];          // FFT 输入 & 输出共用
static float      mag[N_FFT/2];        // 单边功率谱（P = |X|^2）
static uint8_t    win_ready = 0;
static uint8_t    fft_ready = 0;
static arm_rfft_fast_instance_f32 cfg;

/* 外部变量声明 */
extern volatile uint16_t adc_buffer[N_RAW];
extern volatile uint8_t ADC3_BufferReadyFlag;
extern DMA_HandleTypeDef hdma_adc3;
extern ADC_HandleTypeDef hadc3;
extern TIM_HandleTypeDef htim3;

#define PI_F 3.1415926535

static inline void init_hann_fft(void)
{
    if (!win_ready) {
        for (uint32_t n = 0; n < N_RAW; ++n)
            win[n] = 0.5f * (1.0f - cosf(2.0f * PI_F * n / (N_RAW-1)));
        win_ready = 1;
    }
    if (!fft_ready) {
        arm_rfft_fast_init_f32(&cfg, N_FFT);
        fft_ready = 1;
    }
}

/**
 * @brief 对ADC原始数据进行去零偏处理
 *
 * 此函数计算输入ADC原始数据的均值，并将每个采样值减去均值后乘以ADC的电压分辨率（ADC_LSB_VOLT），
 * 得到去零偏后的电压值。结果存储在adc_zeroed数组中。
 *
 * @param[in]  adc_raw     ADC原始数据数组指针
 * @param[out] adc_zeroed  去零偏后的电压值数组指针
 * @param[in]  len         数据长度
 */
void adc_zero_bias(const uint16_t *adc_raw, float* adc_zeroed, uint32_t len)
{
    float sum = 0.0f;
    for (uint32_t i = 0; i < len; i++)
        sum += (float)adc_raw[i];
    float mean = sum / (float)len;
    
    for (uint32_t i = 0; i < len; i++)
        adc_zeroed[i] = ((float)adc_raw[i] - mean) * ADC_LSB_VOLT;
}


void fft_top2_hann_zero_interp(const float *adc,
                               float *f1_est, float *A1_est,
                               float *f2_est, float *A2_est)
{
    init_hann_fft();                          /* 懒加载 */

    /* ---------- 1) 乘 Hann 窗 + 0 补 ---------- */
    for (uint32_t n = 0; n < N_RAW; ++n)
        buf[n] = adc[n] * win[n];
    memset(&buf[N_RAW], 0, (N_FFT - N_RAW) * sizeof(float));

    /* ---------- 2) 实数 FFT ---------- */
    arm_rfft_fast_f32(&cfg, buf, buf, 0);     /* 原地复用 */

    /* ---------- 3) 单边功率谱 & 找 top-2 ---------- */
    float P1 = -1.0f, P2 = -1.0f;
    uint32_t k1 = 1,   k2 = 1;

    for (uint32_t k = 1; k < N_FFT/2; ++k) {  /* 跳过 DC */
        float re = buf[2*k];                  /* CMSIS: 实部在偶，下标=2k */
        float im = buf[2*k+1];                /*          虚部在奇，下标=2k+1 */
        float P  = re*re + im*im;             /* 幅度平方 = 功率谱 */
        mag[k] = P;                           /* 存入数组便于后面插值 */

        if (P > P1) {                         /* 更新第一名，顺推第二名 */
            P2 = P1;  k2 = k1;
            P1 = P;   k1 = k;
        } else if (P > P2) {                  /* 只更新第二名 */
            P2 = P;   k2 = k;
        }
    }

    /* —— 峰太近时重新找第二峰（可按需求关闭此段） —— */
    if (fabsf((int32_t)k1 - (int32_t)k2) < 2) {
        P2 = -1.0f; k2 = 1;
        for (uint32_t k = 1; k < N_FFT/2; ++k) {
            if (k == k1) continue;
            if (mag[k] > P2) { P2 = mag[k]; k2 = k; }
        }
    }

//    /* ---------- 4) 对数抛物线插值 (1 bin 精度 → <0.1 bin) ---------- */
//#define PARABOLA_INTERP(_k, _f_out, _A_out)                          \
//    do {                                                             \
//        uint32_t km1 = (_k==0U)?_k : _k-1U;                          \
//        uint32_t kp1 = (_k==N_FFT/2-1U)?_k : _k+1U;                  \
//        float a = logf(mag[km1]);                                    \
//        float b = logf(mag[_k]);                                     \
//        float c = logf(mag[kp1]);                                    \
//        float delta = 0.5f * (c - a) / (2.0f*b - a - c);             \
//        float k_ref = (float)_k + delta;                             \
//        *(_f_out) = k_ref * (FS_HZ / (float)N_FFT);                  \
//        /* 幅值补偿：pk_corr = exp(b - 0.25*(c-a)*delta)            */ \
//        float pk_corr = expf(b - 0.25f*(c - a)*delta);               \
//        *(_A_out) = 2.0f * sqrtf(pk_corr) / ((float)N_RAW * HANN_CG);\
//    } while(0)

//    /* 先保证按频率升序输出 */
//    if (k1 > k2) { uint32_t kt=k1; k1=k2; k2=kt; float Pt=P1; P1=P2; P2=Pt; }

//    PARABOLA_INTERP(k1, f1_est, A1_est);
//    PARABOLA_INTERP(k2, f2_est, A2_est);

	// 先保证按频率升序输出

	// 保证k1时钟小于k2
    if (k1 > k2) {
        uint32_t kt = k1; k1 = k2; k2 = kt;
        float Pt = P1;    P1 = P2; P2 = Pt;
    }

    *f1_est = k1 * (FS_HZ / (float)N_FFT);                 // bin转Hz
    *A1_est = 2.0f * sqrtf(P1) / ((float)N_RAW * HANN_CG); // Hann窗归一化幅值
    *f2_est = k2 * (FS_HZ / (float)N_FFT);
    *A2_est = 2.0f * sqrtf(P2) / ((float)N_RAW * HANN_CG);
}

void fft_top5_hann_zero_nointp(const float *adc)
{
    init_hann_fft();

    // 1) 乘 Hann 窗 + 0 补
    for (uint32_t n = 0; n < N_RAW; ++n)
        buf[n] = adc[n] * win[n];
    memset(&buf[N_RAW], 0, (N_FFT - N_RAW) * sizeof(float));

    // 2) 实数 FFT
    arm_rfft_fast_f32(&cfg, buf, buf, 0);

    // 3) 单边功率谱，找top-5
    #define TOPN 5
    float topP[TOPN] = {-1,-1,-1,-1,-1};
    uint32_t topK[TOPN] = {1,1,1,1,1};

    for (uint32_t k = 1; k < N_FFT/2; ++k) {
        float re = buf[2*k];
        float im = buf[2*k+1];
        float P  = re*re + im*im;
        // 插入排序维护top-5
        for(int i=0;i<TOPN;i++) {
            if(P > topP[i]) {
                for(int j=TOPN-1;j>i;j--) { topP[j]=topP[j-1]; topK[j]=topK[j-1]; }
                topP[i]=P; topK[i]=k;
                break;
            }
        }
    }
    // 打印top-5（已按幅值降序）
    printf("Top 5 Peaks (Descending):\n");
    for(int i=0;i<TOPN;i++) {
        float freq = topK[i]*(FS_HZ/(float)N_FFT);
        float amp  = 2.0f * sqrtf(topP[i]) / ((float)N_RAW * HANN_CG);
        printf("Peak %d: F = %.2f Hz,  Amp = %.4f V\n", i+1, freq, amp);
    }
}


/**
 * @brief  基于Hann窗和FFT的谐波及THD分析函数
 *
 * 本函数对输入信号进行Hann窗加窗和零填充后，执行FFT，
 * 根据给定的基波频率估计值f0_hint，自动搜索各次谐波的幅值，
 * 并计算总谐波失真（THD）等参数。
 *
 * @param[in]  adc      输入的采样信号数组，长度为N_RAW
 * @param[in]  f0_hint  基波频率的估计值（单位Hz）
 * @param[out] out      结果输出结构体，包含基波、各次谐波幅值和THD百分比
 */
void fft_tdh_hann(const float *adc, float f0_hint, thd_result_t *out)
{
    init_hann_fft();  // 初始化窗口和FFT

    // 1. 乘Hann窗 + 0填充
    for (uint32_t n = 0; n < N_RAW; n++)
        buf[n] = adc[n] * win[n];
    memset(&buf[N_RAW], 0, (N_FFT - N_RAW) * sizeof(float));

    // 2. FFT
    arm_rfft_fast_f32(&cfg, buf, buf, 0);

    // 3. 计算每个谐波幅值
    float fs = FS_HZ;
    float f_bin = fs / (float)N_FFT;
    uint32_t k1 = (uint32_t)roundf(f0_hint / f_bin);  // 基波bin
    float Vn[HARM_MAX + 1] = {0};                     // 谐波幅值（1~HARM_MAX）

    float mag[N_FFT/2];
    arm_cmplx_mag_f32(buf, mag, N_FFT/2);             // 复频谱转幅值谱

    #define SEARCH 2
    for (uint8_t n = 1; n <= HARM_MAX; n++) {
        uint32_t k_est = k1 * n;
        if (k_est >= N_FFT/2) break;                  // 越界保护

        uint32_t start = (k_est > SEARCH) ? (k_est - SEARCH) : 0U;
        uint32_t end   = MIN(k_est + SEARCH, N_FFT/2 - 1U);
        float peakVal;
        uint32_t peakIdxLocal;
        arm_max_f32(&mag[start], (end - start + 1U), &peakVal, &peakIdxLocal);

        // 单边→峰值：RFFT 单边谱需 ×2；再除以样本数和 Hann 能量系数
        float Vpk = 2.0f * peakVal / ((float)N_RAW * HANN_CG);
        Vn[n] = Vpk * 0.70710678f;  // 峰值转rms
    }

    // 4. THD计算
    float sum_sq = 0.0f;
    for (uint8_t n = 2; n <= HARM_MAX; n++)
        sum_sq += Vn[n] * Vn[n];

    out->fundamental = k1 * f_bin;
    memcpy(out->V_rms, Vn, sizeof(Vn));
    out->thd_percent = sqrtf(sum_sq) / Vn[1] * THD_SCALE;
}


void MeasureTHD(float * thd_value)
{
    // 1. 启动本轮ADC+DMA+TIM采集（彻底修复启动时序）
    ADC3_BufferReadyFlag = BUFFER_READY_FLAG_NONE;

    // 清除可能的DMA标志位
    __HAL_DMA_CLEAR_FLAG(&hdma_adc3, DMA_FLAG_TCIF0_4);

    // 先启动ADC+DMA，确保准备就绪
    if (HAL_ADC_Start_DMA(&hadc3, (uint32_t*)adc_buffer, N_RAW) != HAL_OK)
    {
        Error_Handler();
    }       

    // 启动定时器，第一次TRGO将在完整周期后发生
    HAL_TIM_Base_Start(&htim3);

    // 2. 等待本轮数据采集完成
    uint32_t timeout = 0;
    while (ADC3_BufferReadyFlag != BUFFER_READY_FLAG_FULL && timeout < 1000)
    {
        HAL_Delay(1);  // 短暂延时，避免CPU占用过高
        timeout++;
    }

    // 3. 检查是否超时
    if (timeout >= 1000)
    {
        // 超时处理：停止ADC+TIM，返回无新数据
        HAL_ADC_Stop_DMA(&hadc3);
        HAL_TIM_Base_Stop(&htim3);
    }

    // 4. 停止当前轮次的ADC+TIM（为下次启动做准备）
    HAL_ADC_Stop_DMA(&hadc3);
    HAL_TIM_Base_Stop(&htim3);

    // 清除URS位，恢复默认行为
    htim3.Instance->CR1 &= ~TIM_CR1_URS;

    // 5. 处理本轮采集的数据
    const uint16_t *src = (const uint16_t *)&adc_buffer[0];
    float adc_zeroed[N_RAW];
    adc_zero_bias(src, adc_zeroed, N_RAW);
    fft_tdh_hann(adc_zeroed, f1_est, out);
    *thd_value = out->thd_percent;
}
