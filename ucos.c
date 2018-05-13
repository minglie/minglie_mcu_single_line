/*
*********************************************************************************************************
*********************************************************************************************************
* File : ucos.c
* By : Minglie
* Date :
*********************************************************************************************************
*/
#include "Main_Constant.h"
#include "ucos.h"

void os_init()
{
	//初始化时钟800us
	/////////////////////////////////////////////////////////////////
	_tm2c0 = 0b01010000;
	_tm2c1 = 0b11000001;
	_tm2dl = 0x00;
	_tm2dh = 0x00;
	_tm2al = configTICK_RATE_us & 0xff;
	_tm2ah = configTICK_RATE_us >> 8;
	_tm2rp = 0x00;
	_t2on = 1;
	task0_bit_level_s = 0;
	task0_dat_cur = 0;
	task0_bit_no_back = 0;
	task[0].step = 0;
	task0_bit_com_err = 0;
	task[0].one.rdy = 1; //任务就绪
	task[0].one.enable = 1; //任务使能
	task[0].delay = 0;
	task0_rx_buf = 0;
	task0_tx_buf = 0;
}

#if config_single_wire_task0_mode==0
void task0()
{
	if (task[OSTCBCur].one.rdy)
	{
		switch (task[OSTCBCur].step)
		{
		case 0: {
			//初始化发送，并发送起始位1ms
			COM_C = 0;
			COM = 0;
			task0_tx_buf_temp = task0_tx_buf;
			task0_dat_cur = 0;
			OSTimeDly(5);
			task[OSTCBCur].step = 1;
			break;
		}
		case 1: {//拉高延时1ms准备发送数据
			COM_C = 1;
			OSTimeDly(5);
			task[OSTCBCur].step = 2;
			break;
		}
		case 2: {//发送第0,2,4,6,8(从左数)
			COM_C = 0;
			COM = 0;
			if (task0_tx_buf_temp & 0b10000000)
			{
				OSTimeDly(10);
			}
			else
			{
				OSTimeDly(5);
			}
			task0_tx_buf_temp <<= 1;
			task0_dat_cur++;
			task[OSTCBCur].step = 3;
			break;
		}
		case 3: {//发送第1,3,5,7,9(从左数)
			COM_C = 1;
			if (task0_tx_buf_temp & 0b10000000)
			{
				OSTimeDly(10);
			}
			else
			{
				OSTimeDly(5);
			}
			task0_tx_buf_temp <<= 1;
			task0_dat_cur++;
			if (task0_dat_cur>config_com_task0_lengh + 1) //task0_dat_cur==10, bit[0:9]发送完
			{
				task0_level_count = 0;
				task0_dat_cur = 0;
				task[OSTCBCur].step = 4;
			}
			else
			{
				task[OSTCBCur].step = 2;
			}
			break;
		}
		case 4: { //////////////////////////////////////
				  //开始接收
			COM_C = 1;
			if (COM)//等待对方发送,对齐
			{
				task[OSTCBCur].step = 4;
				task0_level_count++; //
				if (task0_level_count>20) //4ms
				{
					task0_level_count = 0;
					task0_wait_com_h_count = 0;
					task0_wait_com_l_count = 0;
					task[OSTCBCur].step = 255; //溢出,跳到异常分支
				}
			}
			else
			{
				task0_bit_level_s = 0;
				task0_rx_buf_temp = 0;
				task0_dat_cur = 0;
				task0_level_count = 0;
				task[OSTCBCur].step = 5;
			}
			break;
		}
		case 5: { //////////////////////////////////////
			COM_C = 1;
			if (task0_bit_level_s == COM)
			{
				task0_level_count++;
				if (task0_level_count>20) //4ms溢出
				{
					task0_level_count = 0;
					task0_wait_com_h_count = 0;
					task0_wait_com_l_count = 0;
					task[OSTCBCur].step = 255; //溢出,跳到异常分支
				}
			}
			else
			{
				if (task0_dat_cur>0) task0_rx_buf_temp <<= 1; //如果已经收到一些数据，将得到的数据向左移动
				task[OSTCBCur].step = 6;
			}
			break;
		}
		case 6: { //////////////////////////////////////
			if (task0_level_count >= 7) task0_rx_buf_temp++; //超过1.4ms认为是1
			task0_bit_level_s ^= 1;
			task0_dat_cur++;
			if (task0_dat_cur >= config_com_task0_lengh) //task0_dat_cur==8说明[0:7]数据已经接收完毕
			{
				task0_level_count = 0;
				task[OSTCBCur].step = 7;
				task0_rx_buf = task0_rx_buf_temp;
			}
			else
			{
				task0_level_count = 0;
				task[OSTCBCur].step = 5; //接收下一位数据
			}
			break;
		}
		case 7: { //////////////////////////////////////
			COM_C = 1;
			if (COM == 0) //等待bit[8]
			{
				task0_level_count++;
				if (task0_level_count>20) //4ms
				{
					task0_level_count = 0;
					task0_wait_com_h_count = 0;
					task0_wait_com_l_count = 0;
					task[OSTCBCur].step = 255; //溢出,跳到异常分支
				}
			}
			else
			{
				COM_C = 1;
				OSTimeDly(25); //延时5ms，准备下一次通信
				task[OSTCBCur].step = 0;
				task0_bit_no_back = 0;
			}
			break;
		}
		case 255: { //////////////////////////////////////
					//异常分支
			COM_C = 1;
			task0_bit_no_back = 1;//主板不回复
			if (COM == 1)
			{
				task0_wait_com_l_count = 0;
				task0_wait_com_h_count++;
				task[OSTCBCur].step = 255;
				if (task0_wait_com_h_count>20) //持续高4ms,重新发送
				{

					task[OSTCBCur].step = 0;
					task0_wait_com_h_count = 0;
				}
			}
			else
			{ //总线被拉低
				task0_wait_com_h_count = 0;
				task0_wait_com_l_count++;
				if (task0_wait_com_l_count>80)//16ms
				{
					task0_bit_com_err = 1; //总线被拉低
					task0_wait_com_l_count = 0;
				}
			}
			break;
		}
		}
	}
}

#else 
void task0()
{
	if (task[OSTCBCur].one.rdy)
	{
		switch (task[OSTCBCur].step)
		{
		case 0: {
			////初始化分支
			COM_C = 1;
			task0_level_count = 0;
			task0_wait_com_l_count = 0;
			task0_wait_com_h_count = 0;
			task[OSTCBCur].step = 1;
			break;
		}
		case 1: { //等待起始位
			COM_C = 1;
			if (COM == 1)
			{
				task[OSTCBCur].step = 1;
				task0_wait_com_h_count++;
				if (task0_wait_com_h_count>100)//持续拉高200us*100=20ms
				{
					task[OSTCBCur].step = 0; //重新开始接收
				}
				if (task0_wait_com_l_count >= 2)//检测到大于400us以上低电平
				{
					task0_level_count = 0;
					task[OSTCBCur].step = 2;
				}
			}
			else
			{
				task0_wait_com_l_count++;
				task[OSTCBCur].step = 1;
			}
			break;
		}
		case 2: {//开始接收
			COM_C = 1;
			if (COM)//等待对方发送,对齐
			{
				task[OSTCBCur].step = 2;
				task0_level_count++; //
				if (task0_level_count>20) //4ms
				{

					task0_level_count = 0;
					task0_wait_com_h_count = 0;
					task0_wait_com_l_count = 0;
					task[OSTCBCur].step = 255; //溢出,跳到异常分支
				}
			}
			else
			{
				task0_level_count = 0;
				task0_bit_level_s = 0;
				task0_rx_buf_temp = 0;
				task0_dat_cur = 0;
				task[OSTCBCur].step = 3;
			}
			break;
		}

		case 3: {
			if (task0_bit_level_s == COM)
			{
				task0_level_count++;
				if (task0_level_count>20) //4ms溢出
				{
					task0_level_count = 0;
					task0_wait_com_h_count = 0;
					task0_wait_com_l_count = 0;
					task[OSTCBCur].step = 255; //溢出,跳到异常分支
				}
			}
			else
			{ //task0_dat_cur>0 ,说明已经得到至少1bit数据
				if (task0_dat_cur>0) task0_rx_buf_temp <<= 1; //将得到的数据向左移动
				task[OSTCBCur].step = 4;
			}
			break;
		}
		case 4: {
			if (task0_level_count >= 7) task0_rx_buf_temp++; //超过1.4ms认为是1
			task0_bit_level_s ^= 1;
			task0_dat_cur++;
			if (task0_dat_cur >= config_com_task0_lengh) //task0_dat_cur==8说明[0:7]数据已经接收完毕
			{
				task0_level_count = 0;
				task[OSTCBCur].step = 5;
				task0_rx_buf = task0_rx_buf_temp;
			}
			else
			{
				task0_level_count = 0;
				task[OSTCBCur].step = 3; //接收下一位数据
			}
			break;
		}
		case 5: {
			COM_C = 1;
			if (COM == 0) //等待bit[8]
			{
				task0_level_count++;
				if (task0_level_count>20) //4ms
				{
					task0_level_count = 0;
					task0_wait_com_h_count = 0;
					task0_wait_com_l_count = 0;
					task[OSTCBCur].step = 255; //溢出,跳到异常分支
				}
			}
			else
			{
				COM_C = 1;
				OSTimeDly(10); //延时2ms，准备发送
							   //初始化发送
				task0_tx_buf_temp = task0_tx_buf;
				task0_dat_cur = 0;
				task[OSTCBCur].step = 6;
			}
			break;
		}

		case 6: {//发送第0,2,4,6,8(从左数)
			COM_C = 0;
			COM = 0;
			if (task0_tx_buf_temp & 0b10000000)
			{
				OSTimeDly(10);
			}
			else
			{
				OSTimeDly(5);
			}
			task0_tx_buf_temp <<= 1;
			task0_dat_cur++;
			task[OSTCBCur].step = 7;
			break;
		}
		case 7: {//发送第1,3,5,7,9(从左数)
			COM_C = 1;
			if (task0_tx_buf_temp & 0b10000000)
			{
				OSTimeDly(10);
			}
			else
			{
				OSTimeDly(5);
			}
			task0_tx_buf_temp <<= 1;
			task0_dat_cur++;
			if (task0_dat_cur>config_com_task0_lengh + 1) //task0_dat_cur==10, bit[0:9]发送完
			{
				task0_level_count = 0;
				task0_dat_cur = 0;
				COM_C = 1;
				OSTimeDly(5);//延时1ms，准备下一次接收
				task[OSTCBCur].step = 0;
			}
			else
			{
				task[OSTCBCur].step = 6;
			}
			break;

		}
		case 255: { //异常分支
			COM_C = 1;
			task0_rx_buf = 0;
			if (COM == 1)
			{
				task0_wait_com_l_count = 0;
				task0_wait_com_h_count++;
				task[OSTCBCur].step = 255;
				if (task0_wait_com_h_count>20) //持续高4ms,重新接收
				{
					task[OSTCBCur].step = 0;
					task0_wait_com_h_count = 0;
				}
			}
			else
			{
				task0_wait_com_h_count = 0;
				task0_wait_com_l_count++;
				if (task0_wait_com_l_count>80)//16ms
				{
					task0_bit_com_err = 1; //总线被拉低
					task0_wait_com_l_count = 0;
				}
			}
			break;
		}
		}
	}
}
#endif 