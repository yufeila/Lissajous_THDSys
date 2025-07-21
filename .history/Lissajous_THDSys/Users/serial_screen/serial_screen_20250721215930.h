/*
 * @Author: yyf 17786321727@163.com
 * @Date: 2025-07-21 11:25:22
 * @LastEditors: yyf 17786321727@163.com
 * @LastEditTime: 2025-07-21 21:59:30
 * @FilePath: /stm32f407/Lissajous_THDSys/Users/serial_screen/serial_screen.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#ifndef __SERIAL_SCREEN_H__
#define __SERIAL_SCREEN_H__

#include "./serial_screen/hmi_driver.h"
#include "./serial_screen/cmd_queue.h"
#include "./serial_screen/cmd_process.h"

#define TIME_100MS 10    

void UpdateUI(void);                                                                 //更新UI数据
void ProcessMessage( PCTRL_MSG msg, uint16 size );
#endif
