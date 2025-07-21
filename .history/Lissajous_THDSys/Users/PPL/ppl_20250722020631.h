/*
 * @Author: yyf 17786321727@163.com
 * @Date: 2025-07-21 21:54:36
 * @LastEditors: yyf 17786321727@163.com
 * @LastEditTime: 2025-07-22 02:06:27
 * @FilePath: /stm32f407/Lissajous_THDSys/Users/ppl/ppl.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __PPL_H
#define __PPL_H

#include "stm32f4xx_hal.h"

#define FMIN 0
#define KVCO 1

#define HC161N_P0_PIN           GPIO_PIN_1
#define HC161N_P0_PORT          GPIO_A
#define HC161N_P1_PIN           GPIO_PIN_2
#define HC161N_P1_PORT          GPIO_A
#define HC161N_P2_PIN           GPIO_PIN_3
#define HC161N_P2_PORT          GPIO_A
#define HC161N_P3_PIN           GPIO_PIN_4
#define HC161N_P3_PORT          GPIO_A

#define CD4052_A_PIN           GPIO_PIN_5
#define CD4052_A_PORT          GPIO_A
#define CD4052_B_PIN           GPIO_PIN_6
#define CD4052_B_PORT          GPIO_A

#define CD4053_A_PIN           GPIO_PIN_5

#define P0(STATE) HAL_GPIO_WritePin(HC161N_P0_PORT, HC161N_P0_PIN, (GPIO_PinState)(STATE))
#define P1(STATE) HAL_GPIO_WritePin(HC161N_P1_PORT, HC161N_P1_PIN, (GPIO_PinState)(STATE))
#define P2(STATE) HAL_GPIO_WritePin(HC161N_P2_PORT, HC161N_P2_PIN, (GPIO_PinState)(STATE))
#define P3(STATE) HAL_GPIO_WritePin(HC161N_P3_PORT, HC161N_P3_PIN, (GPIO_PinState)(STATE))

void SetPLLMultiply(uint16_t ppl_value);

#endif

