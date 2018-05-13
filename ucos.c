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
	//��ʼ��ʱ��800us
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
	task[0].one.rdy = 1; //�������
	task[0].one.enable = 1; //����ʹ��
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
			//��ʼ�����ͣ���������ʼλ1ms
			COM_C = 0;
			COM = 0;
			task0_tx_buf_temp = task0_tx_buf;
			task0_dat_cur = 0;
			OSTimeDly(5);
			task[OSTCBCur].step = 1;
			break;
		}
		case 1: {//������ʱ1ms׼����������
			COM_C = 1;
			OSTimeDly(5);
			task[OSTCBCur].step = 2;
			break;
		}
		case 2: {//���͵�0,2,4,6,8(������)
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
		case 3: {//���͵�1,3,5,7,9(������)
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
			if (task0_dat_cur>config_com_task0_lengh + 1) //task0_dat_cur==10, bit[0:9]������
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
				  //��ʼ����
			COM_C = 1;
			if (COM)//�ȴ��Է�����,����
			{
				task[OSTCBCur].step = 4;
				task0_level_count++; //
				if (task0_level_count>20) //4ms
				{
					task0_level_count = 0;
					task0_wait_com_h_count = 0;
					task0_wait_com_l_count = 0;
					task[OSTCBCur].step = 255; //���,�����쳣��֧
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
				if (task0_level_count>20) //4ms���
				{
					task0_level_count = 0;
					task0_wait_com_h_count = 0;
					task0_wait_com_l_count = 0;
					task[OSTCBCur].step = 255; //���,�����쳣��֧
				}
			}
			else
			{
				if (task0_dat_cur>0) task0_rx_buf_temp <<= 1; //����Ѿ��յ�һЩ���ݣ����õ������������ƶ�
				task[OSTCBCur].step = 6;
			}
			break;
		}
		case 6: { //////////////////////////////////////
			if (task0_level_count >= 7) task0_rx_buf_temp++; //����1.4ms��Ϊ��1
			task0_bit_level_s ^= 1;
			task0_dat_cur++;
			if (task0_dat_cur >= config_com_task0_lengh) //task0_dat_cur==8˵��[0:7]�����Ѿ��������
			{
				task0_level_count = 0;
				task[OSTCBCur].step = 7;
				task0_rx_buf = task0_rx_buf_temp;
			}
			else
			{
				task0_level_count = 0;
				task[OSTCBCur].step = 5; //������һλ����
			}
			break;
		}
		case 7: { //////////////////////////////////////
			COM_C = 1;
			if (COM == 0) //�ȴ�bit[8]
			{
				task0_level_count++;
				if (task0_level_count>20) //4ms
				{
					task0_level_count = 0;
					task0_wait_com_h_count = 0;
					task0_wait_com_l_count = 0;
					task[OSTCBCur].step = 255; //���,�����쳣��֧
				}
			}
			else
			{
				COM_C = 1;
				OSTimeDly(25); //��ʱ5ms��׼����һ��ͨ��
				task[OSTCBCur].step = 0;
				task0_bit_no_back = 0;
			}
			break;
		}
		case 255: { //////////////////////////////////////
					//�쳣��֧
			COM_C = 1;
			task0_bit_no_back = 1;//���岻�ظ�
			if (COM == 1)
			{
				task0_wait_com_l_count = 0;
				task0_wait_com_h_count++;
				task[OSTCBCur].step = 255;
				if (task0_wait_com_h_count>20) //������4ms,���·���
				{

					task[OSTCBCur].step = 0;
					task0_wait_com_h_count = 0;
				}
			}
			else
			{ //���߱�����
				task0_wait_com_h_count = 0;
				task0_wait_com_l_count++;
				if (task0_wait_com_l_count>80)//16ms
				{
					task0_bit_com_err = 1; //���߱�����
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
			////��ʼ����֧
			COM_C = 1;
			task0_level_count = 0;
			task0_wait_com_l_count = 0;
			task0_wait_com_h_count = 0;
			task[OSTCBCur].step = 1;
			break;
		}
		case 1: { //�ȴ���ʼλ
			COM_C = 1;
			if (COM == 1)
			{
				task[OSTCBCur].step = 1;
				task0_wait_com_h_count++;
				if (task0_wait_com_h_count>100)//��������200us*100=20ms
				{
					task[OSTCBCur].step = 0; //���¿�ʼ����
				}
				if (task0_wait_com_l_count >= 2)//��⵽����400us���ϵ͵�ƽ
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
		case 2: {//��ʼ����
			COM_C = 1;
			if (COM)//�ȴ��Է�����,����
			{
				task[OSTCBCur].step = 2;
				task0_level_count++; //
				if (task0_level_count>20) //4ms
				{

					task0_level_count = 0;
					task0_wait_com_h_count = 0;
					task0_wait_com_l_count = 0;
					task[OSTCBCur].step = 255; //���,�����쳣��֧
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
				if (task0_level_count>20) //4ms���
				{
					task0_level_count = 0;
					task0_wait_com_h_count = 0;
					task0_wait_com_l_count = 0;
					task[OSTCBCur].step = 255; //���,�����쳣��֧
				}
			}
			else
			{ //task0_dat_cur>0 ,˵���Ѿ��õ�����1bit����
				if (task0_dat_cur>0) task0_rx_buf_temp <<= 1; //���õ������������ƶ�
				task[OSTCBCur].step = 4;
			}
			break;
		}
		case 4: {
			if (task0_level_count >= 7) task0_rx_buf_temp++; //����1.4ms��Ϊ��1
			task0_bit_level_s ^= 1;
			task0_dat_cur++;
			if (task0_dat_cur >= config_com_task0_lengh) //task0_dat_cur==8˵��[0:7]�����Ѿ��������
			{
				task0_level_count = 0;
				task[OSTCBCur].step = 5;
				task0_rx_buf = task0_rx_buf_temp;
			}
			else
			{
				task0_level_count = 0;
				task[OSTCBCur].step = 3; //������һλ����
			}
			break;
		}
		case 5: {
			COM_C = 1;
			if (COM == 0) //�ȴ�bit[8]
			{
				task0_level_count++;
				if (task0_level_count>20) //4ms
				{
					task0_level_count = 0;
					task0_wait_com_h_count = 0;
					task0_wait_com_l_count = 0;
					task[OSTCBCur].step = 255; //���,�����쳣��֧
				}
			}
			else
			{
				COM_C = 1;
				OSTimeDly(10); //��ʱ2ms��׼������
							   //��ʼ������
				task0_tx_buf_temp = task0_tx_buf;
				task0_dat_cur = 0;
				task[OSTCBCur].step = 6;
			}
			break;
		}

		case 6: {//���͵�0,2,4,6,8(������)
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
		case 7: {//���͵�1,3,5,7,9(������)
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
			if (task0_dat_cur>config_com_task0_lengh + 1) //task0_dat_cur==10, bit[0:9]������
			{
				task0_level_count = 0;
				task0_dat_cur = 0;
				COM_C = 1;
				OSTimeDly(5);//��ʱ1ms��׼����һ�ν���
				task[OSTCBCur].step = 0;
			}
			else
			{
				task[OSTCBCur].step = 6;
			}
			break;

		}
		case 255: { //�쳣��֧
			COM_C = 1;
			task0_rx_buf = 0;
			if (COM == 1)
			{
				task0_wait_com_l_count = 0;
				task0_wait_com_h_count++;
				task[OSTCBCur].step = 255;
				if (task0_wait_com_h_count>20) //������4ms,���½���
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
					task0_bit_com_err = 1; //���߱�����
					task0_wait_com_l_count = 0;
				}
			}
			break;
		}
		}
	}
}
#endif 