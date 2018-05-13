/*
*********************************************************************************************************
*********************************************************************************************************
* File : ucos.h
* By : Minglie
* Date :
*********************************************************************************************************
*/

#ifndef __Ucos_H
#define __Ucos_H

#include "HT66F018.h" //announce all the head files
#include "Main_Constant.h"

void os_init();
void task0(void);


#define OSTCBCur 0
#define OSTimeDly(k) {task[OSTCBCur].one.rdy =0; task[OSTCBCur].delay =k-1; }

#define OSTimeTick()\
{\
if(task[0].delay ==0) task[0].one.rdy=1;\
else task[0].delay --;\
task0();\
}\

#define config_max_tasks 1 //最大任务个数
#define configTICK_RATE_us 800 //800us 配置心跳周期

#define config_single_wire_task0_mode 0 //单线工作模式，0为主机，1为从机
#define config_com_task0_lengh 8 //配置通信有效位数，只能取2,4,6,8
//引脚配置
#define COM _pc0
#define COM_C _pcc0

//ming_single_wire用到的变量
#pragma rambank0
OS_EXT OSTCB_TypeDef task[config_max_tasks];
//task0的变量
OS_EXT unsigned char task0_dat_cur;
OS_EXT unsigned char task0_wait_com_h_count;
OS_EXT unsigned char task0_wait_com_l_count;
OS_EXT unsigned char task0_tx_buf;
OS_EXT unsigned char task0_tx_buf_temp;
OS_EXT unsigned char task0_rx_buf;
OS_EXT unsigned char task0_rx_buf_temp;
OS_EXT unsigned char task0_level_count;
OS_EXT bit task0_bit_no_back;
OS_EXT bit task0_bit_level_s;
OS_EXT bit task0_bit_com_err;
#pragma norambank

#endif

