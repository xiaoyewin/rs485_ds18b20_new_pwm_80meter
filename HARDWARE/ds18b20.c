#include "ds18b20.h"	
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK miniSTM32开发板
//DS18B20驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/12
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
  #include "modbus_rtu.h"
#define DS18B20_DQ_OUTPUT_L     GPIO_ResetBits(GPIOA, GPIO_Pin_3);
#define DS18B20_DQ_OUTPUT_H     GPIO_SetBits(GPIOA, GPIO_Pin_3);

void ds_delay_us(uint16_t time)
{    
   uint16_t i=0; 
   
   while(time--)
   {
       
   }
}
//复位DS18B20
void DS18B20_Rst(void)	   
{                 
	  DS18B20_IO_OUT(); //SET PA0 OUTPUT
    DS18B20_DQ_OUTPUT_L; //拉低DQ
    ds_delay_us(750);    //拉低750us
    DS18B20_DQ_OUTPUT_H; //DQ=1 
	  ds_delay_us(15);     //15US
}
//等待DS18B20的回应
//返回1:未检测到DS18B20的存在
//返回0:存在
uint8_t DS18B20_Check(void) 	   
{   
	uint8_t retry=0;
	DS18B20_IO_IN();//SET PA0 INPUT	 
   while (DS18B20_DQ_IN&&retry<200)
	{
		retry++;
		ds_delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
    while (!DS18B20_DQ_IN&&retry<240)
	{
		retry++;
		ds_delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}
//从DS18B20读取一个位
//返回值：1/0
uint8_t DS18B20_Read_Bit(void) 			 // read one bit
{
    uint8_t data;
	DS18B20_IO_OUT();//SET PA0 OUTPUT
    DS18B20_DQ_OUTPUT_L; 
	ds_delay_us(2);
    DS18B20_DQ_OUTPUT_H; 
	DS18B20_IO_IN();//SET PA0 INPUT
	ds_delay_us(5); //最多15ns内
	if(DS18B20_DQ_IN)data=1;
    else data=0;	 
    ds_delay_us(50);           
    return data;
}
//从DS18B20读取一个字节
//返回值：读到的数据
uint8_t DS18B20_Read_Byte(void)    // read one byte
{        
    uint8_t i,j,dat;
    dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}
//写一个字节到DS18B20
//dat：要写入的字节
void DS18B20_Write_Byte(uint8_t dat)     
 {             
    uint8_t j;
    uint8_t testb;
	DS18B20_IO_OUT();//SET PA0 OUTPUT;
  for (j=1;j<=8;j++) {
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
			{
            DS18B20_DQ_OUTPUT_L;// Write 1
            ds_delay_us(3);  //2                          
            DS18B20_DQ_OUTPUT_H;
            ds_delay_us(60);    //60          
        }
        else 
        {
            DS18B20_DQ_OUTPUT_L;// Write 0
            ds_delay_us(60);             
            DS18B20_DQ_OUTPUT_H;
            ds_delay_us(3);                          
        }
    }
}
//开始温度转换
void DS18B20_Start(void)// ds1820 start convert
{   						               
    DS18B20_Rst();	   
	  DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
} 
//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在    	 
uint8_t DS18B20_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* 使能GPIOA时钟 */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  /* 配置LED相应引脚PB1*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;  // 开漏
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOA,GPIO_Pin_3);    //输出A3

	DS18B20_Rst();

	return DS18B20_Check();
}  


uint8_t DS18B20_Crc(uint8_t *addr, uint8_t len)
{
    uint8_t crc = 0, inbyte, i, mix;
 
    while (len--)
    {
        // inbyte 存储当前参与计算的新字节
        inbyte = *addr;
				addr++;
 
        for (i = 8; i; i--) 
        {
            // 将新字节与CRC从低位到高位, 依次做异或运算, 每次运算完CRC右移一位
            // 如果运算结果值为1, 则将CRC与 1000 1100 作异或
            // 第3,4位代表流程图中的异或运算, 第7位其实就是运算结果移入的1
            mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix) 
            {
                crc ^= 0x8C;
            }
            inbyte >>= 1;
        }
    }
    return crc;
}


//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
short DS18B20_Get_Temp(void)
{
    uint8_t temp;
    uint8_t TL,TH;
	  short tem;
    DS18B20_Start ();                    // ds1820 start convert
    modbusPoll();
    DS18B20_Rst();
		if(DS18B20_Check()){
			return -10000;
		}
    
    modbusPoll();    
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0xbe);// convert	    
    modbusPoll();
		
		
		uint8_t src_data[9];
		for(int i=0;i<9;i++){
			src_data[i]=DS18B20_Read_Byte();
		}
	
		if(src_data[8]!=DS18B20_Crc(src_data,8)){
			return -10000;
		}
	
	
    TL=src_data[0]; // LSB   
    TH=src_data[1]; // MSB  
	    	  
    if(TH>7){
        TH=~TH;
        TL=~TL; 
        temp=0;//温度为负  
    }
		else 
			temp=1;//温度为正	  	

		
    tem=TH; //获得高八位
    tem<<=8;    
    tem+=TL;//获得底八位
    tem=(float)tem*6.25;//转换     
	if(temp)return tem; //返回温度值
	else return -tem;    
} 
 
