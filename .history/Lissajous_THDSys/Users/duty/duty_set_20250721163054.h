/*
 * @Author: yyf 17786321727@163.com
 * @Date: 2025-07-21 16:10:43
 * @LastEditors: yyf 17786321727@163.com
 * @LastEditTime: 2025-07-21 16:30:54
 * @FilePath: /stm32f407/Lissajous_THDSys/Users/duty_set/duty_set.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __DUTY_SET_H
#define __DUTY_SET_H

#include "main.h"
#include "./ad9833/bsp_ad9833.h"

#define DDS_OUT_VPP 0.6 /* DDS模块输出幅度 */

void SetDuty(uint16_t duty);
void MeasureDuty(float * duty_value);

#endif