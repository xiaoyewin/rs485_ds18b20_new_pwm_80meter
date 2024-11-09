#include "ds18b20.h"	
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK miniSTM32������
//DS18B20��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
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
//��λDS18B20
void DS18B20_Rst(void)	   
{                 
	  DS18B20_IO_OUT(); //SET PA0 OUTPUT
    DS18B20_DQ_OUTPUT_L; //����DQ
    ds_delay_us(750);    //����750us
    DS18B20_DQ_OUTPUT_H; //DQ=1 
	  ds_delay_us(15);     //15US
}
//�ȴ�DS18B20�Ļ�Ӧ
//����1:δ��⵽DS18B20�Ĵ���
//����0:����
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
//��DS18B20��ȡһ��λ
//����ֵ��1/0
uint8_t DS18B20_Read_Bit(void) 			 // read one bit
{
    uint8_t data;
	DS18B20_IO_OUT();//SET PA0 OUTPUT
    DS18B20_DQ_OUTPUT_L; 
	ds_delay_us(2);
    DS18B20_DQ_OUTPUT_H; 
	DS18B20_IO_IN();//SET PA0 INPUT
	ds_delay_us(5); //���15ns��
	if(DS18B20_DQ_IN)data=1;
    else data=0;	 
    ds_delay_us(50);           
    return data;
}
//��DS18B20��ȡһ���ֽ�
//����ֵ������������
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
//дһ���ֽڵ�DS18B20
//dat��Ҫд����ֽ�
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
//��ʼ�¶�ת��
void DS18B20_Start(void)// ds1820 start convert
{   						               
    DS18B20_Rst();	   
	  DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
} 
//��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
//����1:������
//����0:����    	 
uint8_t DS18B20_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* ʹ��GPIOAʱ�� */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  /* ����LED��Ӧ����PB1*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;  // ��©
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOA,GPIO_Pin_3);    //���A3

	DS18B20_Rst();

	return DS18B20_Check();
}  


uint8_t DS18B20_Crc(uint8_t *addr, uint8_t len)
{
    uint8_t crc = 0, inbyte, i, mix;
 
    while (len--)
    {
        // inbyte �洢��ǰ�����������ֽ�
        inbyte = *addr;
				addr++;
 
        for (i = 8; i; i--) 
        {
            // �����ֽ���CRC�ӵ�λ����λ, �������������, ÿ��������CRC����һλ
            // ���������ֵΪ1, ��CRC�� 1000 1100 �����
            // ��3,4λ��������ͼ�е��������, ��7λ��ʵ���������������1
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


//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-550~1250�� 
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
        temp=0;//�¶�Ϊ��  
    }
		else 
			temp=1;//�¶�Ϊ��	  	

		
    tem=TH; //��ø߰�λ
    tem<<=8;    
    tem+=TL;//��õװ�λ
    tem=(float)tem*6.25;//ת��     
	if(temp)return tem; //�����¶�ֵ
	else return -tem;    
} 
 
