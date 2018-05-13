
/*
*********************************************************************************************************
*********************************************************************************************************
* File : Main_constant.h
* By : Minglie
* Date :
*********************************************************************************************************
*/


#ifndef __Main_Constant_H
#define __Main_Constant_H


#define con_major_cycle 35 //us
#define con_led_flash_delay 70 //560ms
#define con_key_d_delay 6 //24ms
#define con_key_u_delay 4 //16ms
#define con_sleep_delay 501 //5s
#define con_nothing_delay 45000 // 3min

//#define con_nothing_delay 45 // 3/10min
//--------------------Led deifne---------------------------------------------
#define KEY_IO1 _pc1
#define KEY_IO1C _pcc1
#define KEY_IO2 _pc2
#define KEY_IO2C _pcc2
#define KEY_IO3 _pb4


#define F1_GRE_LED _pa1
#define F2_GRE_LED _pa2
#define F3_GRE_LED _pb1
#define F4_GRE_LED _pa0
#define LAMP _pb0

#define EN _pb3
#define SNOW_TR _pa4
#define BLOW_LEFT _pa5
#define BLOW_RIGHT _pa6

#define CLUTCH _pb2
#define ALARM _pa7

#define HC595_SCLK _pa3
#define HC595_RCLK _pb6
#define HC595_SDAT _pb5

///////////



//----------------------------global variable define---------------------------------------------------
#ifdef OS_GLOBALS
#define OS_EXT
#else
#define OS_EXT extern
#endif

typedef union
{
	struct
	{
		unsigned low : 4;
		unsigned up : 4;

	}half;
	struct
	{
		unsigned b0 : 1;
		unsigned b1 : 1;
		unsigned b2 : 1;
		unsigned b3 : 1;
		unsigned b4 : 1;
		unsigned b5 : 1;
		unsigned b6 : 1;
		unsigned b7 : 1;
	}one;
	unsigned char all;
}CHAR_TypeDef;



typedef union
{
	struct
	{
		unsigned snow_comand_b0 : 1; //ɨѩ����
		unsigned walk_comand_b321 : 3; //��������
		unsigned swing_comand_b54 : 2; //ת������
		unsigned nc_comand_b76 : 2; //
	}one;
	unsigned char all;
} ptl_tx_stack_TypeDef;

typedef union
{
	struct
	{
		unsigned low_vol_num_b210 : 3; //�Ͱ���ѹֵ
		unsigned up_vol_num_b543 : 3; //�߰���ѹֵ
		unsigned charge_b6 : 1; //����������־
		unsigned bat_low_up_b7 : 1; //��ǰ���ڳ��İ���0Ϊ�£�1Ϊ��
	}one;
	unsigned char all;
} ptl_rx_stack_TypeDef;

typedef struct
{
	struct struct_tcb
	{
		unsigned rdy : 1;
		unsigned enable : 1;
	}one;
	unsigned char delay;
	unsigned char step;
}OSTCB_TypeDef;


#pragma rambank0
OS_EXT unsigned char system_counter;
OS_EXT unsigned char system_step;
OS_EXT unsigned char counter_sw[5];//΢�����ؼ���
OS_EXT unsigned char walk_comand_new, walk_comand_old;//
OS_EXT unsigned char led_flash_delay;
OS_EXT unsigned char lamp_flash_delay;
OS_EXT unsigned char lamp_flash_count;
OS_EXT unsigned char sound_state;
OS_EXT unsigned char sound_delay;
OS_EXT unsigned char sound_ture;
OS_EXT unsigned char sound_false;
OS_EXT unsigned int nothing_delay;
OS_EXT unsigned char com_err_count;
OS_EXT int sleep_delay; //announce this int store the delay time of sleep

OS_EXT volatile union bits_decompound //announce a union save special bits
{
	struct struct0
	{
		unsigned ms1_s : 1; //
		unsigned ms5_s : 1; //5ms timer sign bit
		unsigned clut_tr_new : 1; //���
		unsigned clut_tr_old : 1; //���
		unsigned main_no_back : 1; // ���岻�ظ�
		unsigned sleep_s : 1; // ���岻�ظ�

		unsigned blow_tr_left : 1; //ת����
		unsigned blow_tr_right : 1; //ת����
		unsigned stop_s : 1;
		unsigned snow_tr : 1; //ɨѩ���ر�־
		unsigned stow_s : 1; //����תʱΪ1��
		unsigned key_d_s : 1; //�а������ڰ���
		unsigned key_u_s : 1; //��������
		unsigned flash : 1; //��������
		unsigned flash_swi : 1; //��������
	}one;
	unsigned char all[2];
}bt;

//��ʾ����
OS_EXT volatile CHAR_TypeDef key_dis_state;
OS_EXT volatile CHAR_TypeDef key_dis_state_temp;
OS_EXT volatile CHAR_TypeDef hc595dis_state;
OS_EXT volatile CHAR_TypeDef vol_low_state;
OS_EXT volatile CHAR_TypeDef vol_up_state;
OS_EXT volatile CHAR_TypeDef vol_dis_state;
OS_EXT volatile CHAR_TypeDef vol_dis_state_temp;
//ͨ�ű���
OS_EXT volatile ptl_tx_stack_TypeDef task0_tx_stack;
OS_EXT volatile ptl_rx_stack_TypeDef task0_rx_stack;
OS_EXT volatile CHAR_TypeDef old_lamp;

OS_EXT unsigned char key_num;
OS_EXT unsigned char key_temp; //��ֵ�ݴ�
OS_EXT CHAR_TypeDef key;
OS_EXT unsigned char key_d_delay; //�м����¼���
OS_EXT unsigned char key_u_delay; //�޼����¼���

OS_EXT unsigned int adres; //announce a int as save the ad result
OS_EXT volatile union union3 //announce a union as save adres as address
{
	unsigned int adres;
	unsigned char adr[2];
}union_ad;
#pragma norambank
#endif
