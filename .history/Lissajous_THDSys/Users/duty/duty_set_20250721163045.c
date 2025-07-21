#include "./duty/duty_set.h"
#include "dac.h" // 确保包含了 DAC 相关头文件
#include "tim.h" // 确保包含了 TIM 相关头文件
#include "./ad9833/bsp_ad9833.h"


extern DAC_HandleTypeDef hdac; // DAC 句柄，通常在 dac.c 里定义
extern TIM_HandleTypeDef htim4; // TIM4 句柄，通常在 tim.c 里定义
extern float duty; // tim.c 里定义的


void SetDuty(uint16_t duty)
{
    float fduty = (float)duty / 100.0f;
    float Vref = DDS_OUT_VPP * (1 - fduty);

    // 假设 Vref 需要映射到 0~3.3V DAC 输出
    // 你可能需要根据实际电路调整Vref的范围
    if (Vref < 0) Vref = 0;
    if (Vref > 3.3f) Vref = 3.3f;

    uint16_t dac_value = (uint16_t)((Vref / 3.3f) * 4095);

    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

    AD9833_Config(1000.0, AD9833_OUT_TRIANGLE, 0);
}

void MeasureDuty(float * duty_value)
{
    // 启动 TIM4 输入捕获（假设用通道1和2）
    HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1); // 捕获周期
    HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_2); // 捕获高电平宽度

    // 等待中断回调自动更新 duty
    // 你可以用延时、轮询、信号量等方式等待一段时间
    HAL_Delay(10); // 例如延时10ms，确保捕获到一次PWM周期

    // 读取全局变量 duty
    *duty_value = duty;

    // 用完后可以停止捕获，节省功耗
    HAL_TIM_IC_Stop_IT(&htim4, TIM_CHANNEL_1);
    HAL_TIM_IC_Stop_IT(&htim4, TIM_CHANNEL_2);

    // 后续处理 measuredDuty
}







