
/*
*********************************************************************************************************


*********************************************************************************************************
* File : subprogram.c
* By : Minglie
* Date :
*********************************************************************************************************
*/

#define OS_GLOBALS
#include <HT66F018.h> //announce all the head files
#include "Function.h"
#include "Main_Constant.h"
#include "ucos.h"
//--------------------Mcu pow-up initialize-----------------
void mcu_init()
{
	_smod = 0b00000001; //system clock is HIRC,other function is disable
	_lvdc = 0b00000000; //Lvd is disabled
	_wdtc = 0b01010001; //WDT enable, fs is LIRC 32KHz,timeout 30ms
	_tbc = 0b11100101; //enable the tb0 each 4096ms time-out
	_tmpc = 0b00000000; //disenable the tp1b_0 and tp0_0
	_ctrl = 0x00;
	_cpc = 0b00001000; //comparer off and route off, PA3 I/O can working
	_integ = 0b00000000; //INT1 falling interrupt
	_intc0 = 0b00000000; //Global interrupt enable
	_intc1 = 0b00000000;
	_intc2 = 0b00000000;
	_mfi0 = 0b00000000;
	_mfi1 = 0b00000000; //Enable t1pe for clearing the wdt
	_mfi2 = 0b00000000;

	_adcr0 = 0b00010000; //AD converter is enable,AD value is right format
	_adcr1 = 0b00000010; //AD clock source is 1/12M*8=667ns
	_acerl = 0b00000000;

	_pawu = 0b00000000;
	_papu = 0b11110000;
	_pac = 0b01110000;
	_pa = 0b01110000;

	_pbpu = 0b00010100;
	_pbc = 0b00010100;
	_pb = 0b00010100;

	_pcpu = 0b00000110;
	_pcc = 0b11111111;
	_pc = 0b11111111;

	//////////////////////////////////////////////////////////////////////////
	_clrwdt();
}

void variable_init()
{
	bt.all[0] = 0;
	bt.all[1] = 0;
	key_dis_state.all = 0b000000;
	counter_sw[0] = 0;
	counter_sw[1] = 0;
	counter_sw[2] = 0;
	walk_comand_new = 0;
	walk_comand_old = 0;
	bt.one.stow_s = 1;
	bt.one.clut_tr_new = 0;
	os_init();
	com_err_count = 0;
	bt.one.main_no_back = 0;
	sleep_delay = 0;
	bt.one.sleep_s = 0;
	lamp_flash_count = 0;
	lamp_flash_delay = 0;
	sound_delay = 0;
	sound_false = 0;
	sound_ture = 0;
	sound_state = 0;
	nothing_delay = 0;
}



//--------------------sense the switch status---------------
void get_keystatus()
{
	//离合
	if (bt.one.clut_tr_new)
	{
		if (CLUTCH == 0) counter_sw[0]++; //judge the trigger witch
		else counter_sw[0] = 0;
		if (counter_sw[0] >= 2)
		{
			bt.one.clut_tr_new = 0;
			counter_sw[0] = 0;
		}
	}
	else
	{
		if (CLUTCH == 1) counter_sw[0]++;
		else counter_sw[0] = 0;
		if (counter_sw[0] >= 2)
		{
			bt.one.clut_tr_new = 1;
			counter_sw[0] = 0;
		}
	}


	//扫雪触发
	if (bt.one.snow_tr)
	{
		if (SNOW_TR == 0) counter_sw[1]++; //judge the trigger witch
		else counter_sw[1] = 0;
		if (counter_sw[1] >= 2)
		{
			bt.one.snow_tr = 0;
			counter_sw[1] = 0;
		}
	}
	else
	{
		if (SNOW_TR == 1) counter_sw[1]++;
		else counter_sw[1] = 0;
		if (counter_sw[1] >= 2)
		{
			bt.one.snow_tr = 1;
			counter_sw[1] = 0;
		}
	}
	//转向开关
	if (bt.one.blow_tr_left)
	{
		if (BLOW_LEFT == 0) counter_sw[2]++; //judge the trigger witch
		else counter_sw[2] = 0;
		if (counter_sw[2] >= 2)
		{
			bt.one.blow_tr_left = 0;
			counter_sw[2] = 0;
		}
	}
	else
	{
		if (BLOW_LEFT == 1) counter_sw[2]++;
		else counter_sw[2] = 0;
		if (counter_sw[2] >= 2)
		{
			bt.one.blow_tr_left = 1;
			counter_sw[2] = 0;
		}
	}

	if (bt.one.blow_tr_right)
	{
		if (BLOW_RIGHT == 0) counter_sw[3]++; //judge the trigger witch
		else counter_sw[3] = 0;
		if (counter_sw[3] >= 2)
		{
			bt.one.blow_tr_right = 0;
			counter_sw[3] = 0;
		}
	}
	else
	{
		if (BLOW_RIGHT == 1) counter_sw[3]++;
		else counter_sw[3] = 0;
		if (counter_sw[3] >= 2)
		{
			bt.one.blow_tr_right = 1;
			counter_sw[3] = 0;
		}
	}
}


void key_scan()
{
	KEY_IO2C = 1; KEY_IO1C = 1;
	key.all = 0xff;
	if (KEY_IO3 == 0)
	{
		key.all = 0b11110111; //KEY4
	}
	else if (!KEY_IO2 && !KEY_IO1)
	{
		key.all = 0b11111011; //KEY3
	}
	else if (!KEY_IO2)
	{
		key.all = 0b11111101; //KEY2
	}
	else if (!KEY_IO1)
	{
		key.all = 0b11111110; //KEY1
	}
	else
	{
		KEY_IO1C = 1; KEY_IO1 = 1;
		KEY_IO2C = 0; KEY_IO2 = 0;
		KEY_IO1C = 1; KEY_IO1 = 1;
		if (!KEY_IO1)
		{
			KEY_IO2C = 1; KEY_IO2 = 1;
			KEY_IO1C = 0; KEY_IO1 = 0;
			KEY_IO2C = 1; KEY_IO2 = 1;
			if (!KEY_IO2)
			{
				key.all = 0xff; //两个IO短路KEY7
			}
			else
			{
				key.all = 0b11101111; //KEY5
			}
		}
		else
		{
			KEY_IO2C = 1; KEY_IO2 = 1;
			KEY_IO1C = 0; KEY_IO1 = 0;
			KEY_IO2C = 1; KEY_IO2 = 1;
			if (!KEY_IO2)
			{
				key.all = 0b11011111; //KEY6
			}
			else
			{
				key.all = 0xff; //无按键
			}
		}
	}
	KEY_IO2C = 1; KEY_IO1C = 1;
	_clrwdt();

	if (key.all != 0xff)
	{
		key_d_delay++;
		key_u_delay = 0;
		if (key_d_delay>con_key_d_delay)//
		{
			bt.one.key_d_s = 1;
			key_temp = key.all;
		}
	}
	else
	{
		key_d_delay = 0;
		if (bt.one.key_d_s == 1)
		{
			key_u_delay++;
			if (key_u_delay>con_key_u_delay)
			{
				bt.one.key_d_s = 0;
				bt.one.key_u_s = 1;
				key_u_delay = 0;
			}
		}

		if (bt.one.key_u_s == 1)
		{
			bt.one.key_u_s = 0;
			switch (key_temp)
			{
			case 0b11111110: key_num = 1; break;//前进1档
			case 0b11111101: key_num = 2; break;//前进2档
			case 0b11111011: key_num = 3; break;//前进3档
			case 0b11110111: key_num = 4; break;//前进4档
			case 0b11101111: key_num = 5; break;//倒退1档
			case 0b11011111: key_num = 6; break;//照明显示
			case 0b10111111: key_num = 7; break;//电量显示
			default: key_num = 0; break;
			}
		}
		else key_num = 0;
	}
}
void key_serve()
{
	switch (key_num)
	{
	case 1: {
		if (walk_comand_old != 5)
		{
			key_dis_state.one.b1 ^= 1;
			key_dis_state.one.b2 = 0;
			key_dis_state.one.b3 = 0;
			key_dis_state.one.b4 = 0;
			key_dis_state.one.b5 = 0;
			if (key_dis_state.one.b1 == 1) walk_comand_new = 1;
			else walk_comand_new = 0;
		}
		else
		{
			walk_comand_new = 0;
			key_dis_state.one.b1 = 0;
			key_dis_state.one.b2 = 0;
			key_dis_state.one.b3 = 0;
			key_dis_state.one.b4 = 0;
			key_dis_state.one.b5 = 0;
		}break;
	}

	case 2: {
		if (walk_comand_old != 5)
		{
			key_dis_state.one.b1 = 0;
			key_dis_state.one.b2 ^= 1;
			key_dis_state.one.b3 = 0;
			key_dis_state.one.b4 = 0;
			key_dis_state.one.b5 = 0;
			if (key_dis_state.one.b2 == 1) walk_comand_new = 2;
			else walk_comand_new = 0;
		}
		else
		{
			walk_comand_new = 0;
			key_dis_state.one.b1 = 0;
			key_dis_state.one.b2 = 0;
			key_dis_state.one.b3 = 0;
			key_dis_state.one.b4 = 0;
			key_dis_state.one.b5 = 0;
		}break;
	}

	case 3: {
		if (walk_comand_old != 5)
		{
			key_dis_state.one.b1 = 0;
			key_dis_state.one.b2 = 0;
			key_dis_state.one.b3 ^= 1;
			key_dis_state.one.b4 = 0;
			key_dis_state.one.b5 = 0;
			if (key_dis_state.one.b3 == 1) walk_comand_new = 3;
			else walk_comand_new = 0;
		}
		else
		{
			walk_comand_new = 0;
			key_dis_state.one.b1 = 0;
			key_dis_state.one.b2 = 0;
			key_dis_state.one.b3 = 0;
			key_dis_state.one.b4 = 0;
			key_dis_state.one.b5 = 0;

		}break;
	}


	case 4: {
		if (walk_comand_old != 5)
		{
			key_dis_state.one.b1 = 0;
			key_dis_state.one.b2 = 0;
			key_dis_state.one.b3 = 0;
			key_dis_state.one.b4 ^= 1;
			key_dis_state.one.b5 = 0;

			if (key_dis_state.one.b4 == 1) { walk_comand_new = 4; }
			else walk_comand_new = 0;
		}
		else
		{
			walk_comand_new = 0;
			key_dis_state.one.b1 = 0;
			key_dis_state.one.b2 = 0;
			key_dis_state.one.b3 = 0;
			key_dis_state.one.b4 = 0;
			key_dis_state.one.b5 = 0;

		}break;
	}


	case 5: {
		if (walk_comand_old != 1 && walk_comand_old != 2 && walk_comand_old != 3 && walk_comand_old != 4)
		{
			key_dis_state.one.b1 = 0;
			key_dis_state.one.b2 = 0;
			key_dis_state.one.b3 = 0;
			key_dis_state.one.b4 = 0;
			key_dis_state.one.b5 ^= 1;
			if (key_dis_state.one.b5 == 1) walk_comand_new = 5;
			else walk_comand_new = 0;
		}
		else
		{
			walk_comand_new = 0;
			key_dis_state.one.b1 = 0;
			key_dis_state.one.b2 = 0;
			key_dis_state.one.b3 = 0;
			key_dis_state.one.b4 = 0;
			key_dis_state.one.b5 = 0;
		}break;
	}
	case 6: {
		key_dis_state.one.b6 ^= 1; break;
	}
	case 7: {
		key_dis_state.one.b7 ^= 1; break;
	}

	}

	if (!bt.one.clut_tr_new && (walk_comand_old == 0) && (walk_comand_new>0))
	{
		bt.one.stow_s = 0;
	}
	walk_comand_old = walk_comand_new;
	if (bt.one.stow_s == 0)
	{
		if (bt.one.clut_tr_old == 1 && bt.one.clut_tr_new == 0) bt.one.stow_s = 1;
		bt.one.clut_tr_old = bt.one.clut_tr_new;
	}







	//send dat
	if (walk_comand_new == 4 || walk_comand_new == 5)//前进4档与倒档不能扫雪
	{
		task0_tx_stack.one.snow_comand_b0 = 0;
	}
	else
	{
		task0_tx_stack.one.snow_comand_b0 = !bt.one.snow_tr;//扫雪
	}



	if (!bt.one.clut_tr_new && bt.one.stow_s && walk_comand_new>0 && walk_comand_new<6)//行走
	{
		task0_tx_stack.one.walk_comand_b321 = walk_comand_new;
		if (walk_comand_new == 4) bt.one.flash = 1;
		else
		{
			bt.one.flash = 0;
			bt.one.flash_swi = 1;
		}
	}
	else
	{

		task0_tx_stack.one.walk_comand_b321 = 0;
		bt.one.flash = 0;
		bt.one.flash_swi = 1;
	}
	task0_tx_stack.one.swing_comand_b54 = ~(bt.one.blow_tr_left << 1 | bt.one.blow_tr_right);//转向
	task0_tx_stack.one.nc_comand_b76 = 0;
	////////////////////////////////////////////////////////
	task0_tx_buf = task0_tx_stack.all; //发送数据
}


void dis_refresh()
{
	task0_rx_stack.all = task0_rx_buf; //读取数据
	if (task0_bit_no_back == 1)
	{
		com_err_count++;
		if (com_err_count>100)bt.one.main_no_back = 1;
		task0_rx_stack.one.up_vol_num_b543 = 0;
		task0_rx_stack.one.low_vol_num_b210 = 0;
	}
	else
	{
		com_err_count = 0;
		bt.one.main_no_back = 0;
	}
	if (bt.one.main_no_back == 0)//主板回复了
	{
		if (task0_bit_no_back == 0)//主板及时回复了
		{
			if (task0_rx_stack.one.low_vol_num_b210 != 4) vol_dis_state_temp.half.low = (0b1 << task0_rx_stack.one.low_vol_num_b210) - 1;
			else vol_dis_state_temp.half.low = 0;//欠压
			if (task0_rx_stack.one.up_vol_num_b543 != 4) vol_dis_state_temp.half.up = (0b1 << task0_rx_stack.one.up_vol_num_b543) - 1;
			else vol_dis_state_temp.half.up = 0;//欠压
		}
	}

	vol_dis_state.all = 0xff - vol_dis_state_temp.all;
	hc595dis_state.one.b0 = key_dis_state.one.b5;

	hc595dis_state.one.b1 = vol_dis_state.one.b4;
	hc595dis_state.one.b2 = vol_dis_state.one.b5;
	hc595dis_state.one.b3 = vol_dis_state.one.b6;

	hc595dis_state.one.b4 = vol_dis_state.one.b2;
	hc595dis_state.one.b5 = vol_dis_state.one.b1;
	hc595dis_state.one.b6 = vol_dis_state.one.b0;

	hc595dis_state.one.b7 = key_dis_state.one.b6;
	if (bt.one.sleep_s == 0)
	{
		hc595_drive(hc595dis_state.all);
		F1_GRE_LED = key_dis_state.one.b1;
		F2_GRE_LED = key_dis_state.one.b2;
		F3_GRE_LED = key_dis_state.one.b3;
		F4_GRE_LED = key_dis_state.one.b4;

		if (bt.one.flash && bt.one.flash_swi)
		{
			lamp_flash_delay++;
			if (lamp_flash_delay>60)
			{
				lamp_flash_delay = 0;
				LAMP ^= 1;
				lamp_flash_count++;
				if (lamp_flash_count >= 10) { LAMP = key_dis_state.one.b6; lamp_flash_count = 0; bt.one.flash_swi = 0; }
			}
		}
		else
		{

			LAMP = key_dis_state.one.b6;
		}
	}
}

void hc595_drive(unsigned char dat_h/*,unsigned char dat_l*/)
{
	unsigned char i;
	unsigned char dat_temp;
	HC595_SCLK = 0;
	HC595_RCLK = 0;
	dat_temp = dat_h;
	for (i = 0; i<8; i++)
	{
		if (dat_temp >= 0x80)HC595_SDAT = 1;
		else HC595_SDAT = 0;

		HC595_SCLK = 0;
		_delay(1);
		HC595_SCLK = 1;
		_delay(1);
		dat_temp = dat_temp << 1;
	}
	/*dat_temp=dat_l;
	for(i=0;i<8;i++)
	{
	if(dat_temp>=0x80)HC595_SDAT=1;
	else HC595_SDAT=0;

	HC595_SCLK=0;
	_delay(1);
	HC595_SCLK=1;
	_delay(1);

	dat_temp=dat_temp<<1;
	}*/
	HC595_RCLK = 0;
	_delay(1);
	HC595_RCLK = 1;
	_delay(1);

	HC595_SCLK = 0;
	HC595_RCLK = 0;
	HC595_SDAT = 0;
}


//--------------------AD test-------------------------------
void ad_convert(unsigned char addres) //AD test shoud be on average
{
	unsigned char ad_times;
	_adcr0 = addres; //slect the AD channel
	_adrfs = 1;
	adres = 0; //clear the adres
	for (ad_times = 4; ad_times>0; ad_times--)
	{
		_start = 1;
		_start = 0; //start the a ad conversion
		_delay(4);
		while (_eocb); //wait for the AD conversion end
		union_ad.adr[0] = _adrl;
		union_ad.adr[1] = _adrh;
		adres += union_ad.adres;
	}
	adres /= 4; //ad conversion 16 times
}

//--------------------system counter rountine----------------------------------
void tm2_counter() //provide a 5ms clock signal for the system
{
	if (_t2af)//200us
	{
		OSTimeTick();
		_tm2dl = 0x00;
		_tm2dh = 0x00;
		_t2af = 0;
		bt.one.ms1_s = 1;
		_clrwdt();
	}
}
//--------------------delay time function-------------------
/* this function is standard delay time routine. the base unit is 4096ms.*/

void delay_5ms(unsigned char delay_parameter)
{
	while (1)
	{
		if (_tb0f)
		{
			delay_parameter--;
			_tb0f = 0;
			_clrwdt();
		}
		if (delay_parameter <= 0) break;
	}
}


//--------------------sleep process-------------------------
void sleep()
{
	if (task0_rx_stack.one.low_vol_num_b210 == 4 || task0_rx_stack.one.up_vol_num_b543 == 4 | bt.one.main_no_back)//主板不回复
	{
		sleep_delay++;
	}
	else
	{
		sleep_delay = 0;
		EN = 1;
		bt.one.sleep_s = 0;
	}
	if (sleep_delay >= con_sleep_delay)///2s
	{
		sleep_delay = con_sleep_delay;
		bt.one.sleep_s = 1;
		EN = 0;
		F1_GRE_LED = 0;
		F2_GRE_LED = 0;
		F3_GRE_LED = 0;
		LAMP = 0;
		hc595_drive(0xff);
	}

	if (task0_tx_stack.all == 0 && LAMP == 0)//不行走，不转向，不扫雪，不照明持续三分钟就蜂鸣器一直叫
	{
		nothing_delay++;
	}
	else
	{
		nothing_delay = 0;
	}

	if (nothing_delay>con_nothing_delay)sound_state = 2;
}



void sound_process(void)
{
	if (sound_state)
	{
		if (sound_delay == 0)
		{
			sound_delay = 40;
			if (sound_false == 0)
			{
				sound_ture = sound_state;
				sound_false = sound_state;
				ALARM = 1;
			}
			else if (sound_ture)
			{
				if (ALARM)
				{
					ALARM = 0;
					sound_ture--;
				}
				else
				{
					ALARM = 1;
				}
			}
			else
			{
				sound_false--;
			}
		}
		else sound_delay--;
	}
	else ALARM = 0;
}



void peripheral_init() // 初始化外围
{
	task0_rx_stack.all = 0;
	EN = 0;
	F1_GRE_LED = 0;
	F2_GRE_LED = 0;
	F3_GRE_LED = 0;
	LAMP = 0;
	hc595_drive(0xff);
	delay_5ms(240);
	task0_rx_buf = 0;
	vol_dis_state_temp.all = 0x00;
	EN = 1;
}