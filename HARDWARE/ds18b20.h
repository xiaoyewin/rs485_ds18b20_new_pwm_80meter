#ifndef __DS18B20_H
#define __DS18B20_H 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK MiniSTM32开发板
//DS18B20驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/3/12
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//IO方向设置

#include "stm32f0xx.h"



#define BIT32_SET(reg,pin,value) (reg = ((reg & ~(0x00000003 << (pin * 2))) | (value << (pin * 2))))//reg:寄存器  pin：管教号  value：参考手册上的设定值

#define DS18B20_IO_IN()  				BIT32_SET(GPIOA->MODER,3,0x00)//GPIOA_3模式：输人											00: 输入模式( 复位状态)	01: 通用输出模式	10: 复用功能模式	11: 模拟模式
#define DS18B20_IO_OUT() 				BIT32_SET(GPIOA->MODER,3,0x01)//GPIOA_3模式：输出	

////IO操作函数											   


#define BIT16_READ(reg,pin) ((reg & (0x0001 << pin)) >> pin)//reg:寄存器  pin：管教号  value：参考手册上的设定值

#define	DS18B20_DQ_IN  BIT16_READ(GPIOA->IDR,3)  //数据端口	PB9 
   	
uint8_t DS18B20_Init(void);	     //初始化DS18B20
short DS18B20_Get_Temp(void);	//获取温度
void DS18B20_Start(void);		//开始温度转换
void DS18B20_Write_Byte(uint8_t dat);//写入一个字节
uint8_t DS18B20_Read_Byte(void);		//读出一个字节
uint8_t DS18B20_Read_Bit(void);		//读出一个位
uint8_t DS18B20_Check(void);			//检测是否存在DS18B20
void DS18B20_Rst(void);			//复位DS18B20    
#endif















