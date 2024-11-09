#ifndef __DS18B20_H
#define __DS18B20_H 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK MiniSTM32������
//DS18B20��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/3/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//IO��������

#include "stm32f0xx.h"



#define BIT32_SET(reg,pin,value) (reg = ((reg & ~(0x00000003 << (pin * 2))) | (value << (pin * 2))))//reg:�Ĵ���  pin���̺ܽ�  value���ο��ֲ��ϵ��趨ֵ

#define DS18B20_IO_IN()  				BIT32_SET(GPIOA->MODER,3,0x00)//GPIOA_3ģʽ������											00: ����ģʽ( ��λ״̬)	01: ͨ�����ģʽ	10: ���ù���ģʽ	11: ģ��ģʽ
#define DS18B20_IO_OUT() 				BIT32_SET(GPIOA->MODER,3,0x01)//GPIOA_3ģʽ�����	

////IO��������											   


#define BIT16_READ(reg,pin) ((reg & (0x0001 << pin)) >> pin)//reg:�Ĵ���  pin���̺ܽ�  value���ο��ֲ��ϵ��趨ֵ

#define	DS18B20_DQ_IN  BIT16_READ(GPIOA->IDR,3)  //���ݶ˿�	PB9 
   	
uint8_t DS18B20_Init(void);	     //��ʼ��DS18B20
short DS18B20_Get_Temp(void);	//��ȡ�¶�
void DS18B20_Start(void);		//��ʼ�¶�ת��
void DS18B20_Write_Byte(uint8_t dat);//д��һ���ֽ�
uint8_t DS18B20_Read_Byte(void);		//����һ���ֽ�
uint8_t DS18B20_Read_Bit(void);		//����һ��λ
uint8_t DS18B20_Check(void);			//����Ƿ����DS18B20
void DS18B20_Rst(void);			//��λDS18B20    
#endif















