/*
 * @Author: yyf 17786321727@163.com
 * @Date: 2025-07-21 21:54:36
 * @LastEditors: yyf 17786321727@163.com
 * @LastEditTime: 2025-07-22 01:40:01
 * @FilePath: /stm32f407/Lissajous_THDSys/Users/ppl/ppl.h
 * @Description: ����Ĭ������,������`customMade`, ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __PPL_H
#define __PPL_H

#include "stm32f4xx_hal.h"

#define FMIN 0
#define KVCO 1

#define HC161N_PIN_1 GPIO_PIN_1

void SetPLLMultiply(uint16_t ppl_value);

#endif

