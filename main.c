/*
*********************************************************************************************************

*********************************************************************************************************
* File : main.c
* By : Minglie
* Date : 2017.03.20
*********************************************************************************************************
*/
#include "HT66F018.h"
#include "Function.h"
#include "Main_Constant.h"
#include "ucos.h"
//====================Main routine==========================
void main()
{
	mcu_init();
	variable_init();
	peripheral_init();
	while (1)
	{
		tm2_counter();
		if (bt.one.ms1_s)//此if语句最长周期不能超过800us,否则可能通信错误,现在为250us
		{
			system_step++;
			if (system_step >= 5)system_step = 0;
			if (system_step == 0)//63us
			{
				get_keystatus();
				key_scan();
			}
			else if (system_step == 1)//56us
			{
				key_serve();
			}
			else if (system_step == 2)//240us
			{
				dis_refresh();
				sleep();
			}
			else if (system_step == 3)
			{

			}
			else if (system_step == 4)
			{
				sound_process();
			}
			bt.one.ms1_s = 0;
		}
		_clrwdt();
	}
}