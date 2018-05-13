/*
*********************************************************************************************************


*********************************************************************************************************
* File : Function.h
* By : Minglie
* Date :
*********************************************************************************************************
*/

/* Function header file, all the function be used in program */

#ifndef __Function_H
#define __Function_H
//-------------------Main and Common Subprogram routine-----
void mcu_init();
void variable_init();
void get_keystatus();
void key_scan();
void key_serve();
void dis_refresh();
void hc595_drive(unsigned char dat_h/*,unsigned char dat_l*/);
void ad_convert(unsigned char addres); /*ad test routine */
void tb0_counter(); /*system counter routine base time 1ms */
void delay_5ms(unsigned char delay_parameter); /*standard delay time function */
void peripheral_init();
void tm2_counter();
void sleep();
void sound_process(void);
#endif
