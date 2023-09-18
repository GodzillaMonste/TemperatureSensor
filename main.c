#include <REGX52.H>
#include "LCD1602.h"
#include "DS18B20.h"
#include "Delay.h"
#include "AT24C02.h"
#include "Key.h"
#include "Timer0.h"
#include "MatrixKey.h"
#include "Buzzer.h"
#include <stdio.h>
#include "DS1302.h"
float T,TShow;
char temperature[10],Day[5];
char TLow,THigh;
unsigned char KeyNum, MatrixKeyNum;
unsigned char has_buzzer; 
unsigned char KeyNum,MODE,TimeSetSelect,TimeSetFlashFlag,Year = 20;

void TimeShow(void)//时间显示功能
{
	DS1302_ReadTime();//读取时间
  
    LCD_ShowNum(1,1,Year,2);//显示年
	LCD_ShowNum(1,3,DS1302_Time[0],2);//显示年
	LCD_ShowNum(1,6,DS1302_Time[1],2);//显示月
	LCD_ShowNum(1,9,DS1302_Time[2],2);//显示日
             
	LCD_ShowNum(2,1,DS1302_Time[3],2);//显示时
	LCD_ShowNum(2,4,DS1302_Time[4],2);//显示分
	LCD_ShowNum(2,7,DS1302_Time[5],2);//显示秒

    if(DS1302_Time[6] == 0)LCD_ShowString(1,14,"Mon");
    else if(DS1302_Time[6] == 1)LCD_ShowString(1,14,"Tue");
    else if(DS1302_Time[6] == 2)LCD_ShowString(1,14,"Wed");
    else if(DS1302_Time[6] == 3)LCD_ShowString(1,14,"Thu");
    else if(DS1302_Time[6] == 4)LCD_ShowString(1,14,"Fri");
    else if(DS1302_Time[6] == 5)LCD_ShowString(1,14,"Sat");
    else if(DS1302_Time[6] == 6)LCD_ShowString(1,14,"Sun"); //显示星期
}

void TimeSet(void)//时间设置功能
{
    
	if(MatrixKeyNum==2)//按键2按下
	{
		TimeSetSelect++;//设置选择位加1
		TimeSetSelect%=7;//越界清零
	}
	if(MatrixKeyNum==3)//按键3按下
	{
        
		DS1302_Time[TimeSetSelect]++;//时间设置位数值加1
		if(DS1302_Time[0]>99){DS1302_Time[0]=0;Year++;}//年越界判断
		if(DS1302_Time[1]>12){DS1302_Time[1]=1;}//月越界判断
		if( DS1302_Time[1]==1 || DS1302_Time[1]==3 || DS1302_Time[1]==5 || DS1302_Time[1]==7 || 
			DS1302_Time[1]==8 || DS1302_Time[1]==10 || DS1302_Time[1]==12)//日越界判断
		{
			if(DS1302_Time[2]>31){DS1302_Time[2]=1;}//大月
		}
		else if(DS1302_Time[1]==4 || DS1302_Time[1]==6 || DS1302_Time[1]==9 || DS1302_Time[1]==11)
		{
			if(DS1302_Time[2]>30){DS1302_Time[2]=1;}//小月
		}
		else if(DS1302_Time[1]==2)
		{
			if(DS1302_Time[0]%4==0)
			{
				if(DS1302_Time[2]>29){DS1302_Time[2]=1;}//闰年2月
			}
			else
			{
				if(DS1302_Time[2]>28){DS1302_Time[2]=1;}//平年2月
			}
		}
		if(DS1302_Time[3]>23){DS1302_Time[3]=0;}//时越界判断
		if(DS1302_Time[4]>59){DS1302_Time[4]=0;}//分越界判断
		if(DS1302_Time[5]>59){DS1302_Time[5]=0;}//秒越界判断
        if(DS1302_Time[6]>6){DS1302_Time[6]=0;}
	}
	if(MatrixKeyNum==4)//按键4按下
	{
		DS1302_Time[TimeSetSelect]--;//时间设置位数值减1
		if(DS1302_Time[0]<0){DS1302_Time[0]=99;Year--;}//年越界判断
		if(DS1302_Time[1]<1){DS1302_Time[1]=12;}//月越界判断
		if( DS1302_Time[1]==1 || DS1302_Time[1]==3 || DS1302_Time[1]==5 || DS1302_Time[1]==7 || 
			DS1302_Time[1]==8 || DS1302_Time[1]==10 || DS1302_Time[1]==12)//日越界判断
		{
			if(DS1302_Time[2]<1){DS1302_Time[2]=31;}//大月
			if(DS1302_Time[2]>31){DS1302_Time[2]=1;}
		}
		else if(DS1302_Time[1]==4 || DS1302_Time[1]==6 || DS1302_Time[1]==9 || DS1302_Time[1]==11)
		{
			if(DS1302_Time[2]<1){DS1302_Time[2]=30;}//小月
			if(DS1302_Time[2]>30){DS1302_Time[2]=1;}
		}
		else if(DS1302_Time[1]==2)
		{
			if(DS1302_Time[0]%4==0)
			{
				if(DS1302_Time[2]<1){DS1302_Time[2]=29;}//闰年2月
				if(DS1302_Time[2]>29){DS1302_Time[2]=1;}
			}
			else
			{
				if(DS1302_Time[2]<1){DS1302_Time[2]=28;}//平年2月
				if(DS1302_Time[2]>28){DS1302_Time[2]=1;}
			}
            
		}
		if(DS1302_Time[3]<0){DS1302_Time[3]=23;}//时越界判断
		if(DS1302_Time[4]<0){DS1302_Time[4]=59;}//分越界判断
		if(DS1302_Time[5]<0){DS1302_Time[5]=59;}//秒越界判断
        if(DS1302_Time[6]<0){DS1302_Time[6]=6;}
        
	}
	//更新显示，根据TimeSetSelect和TimeSetFlashFlag判断可完成闪烁功能
	if(TimeSetSelect==0 && TimeSetFlashFlag==1){LCD_ShowString(1,1,"    ");}
	else { 
        LCD_ShowNum(1,1,Year,2);
        LCD_ShowNum(1,3,DS1302_Time[0],2);
        }
	if(TimeSetSelect==1 && TimeSetFlashFlag==1){LCD_ShowString(1,6,"  ");}
	else {LCD_ShowNum(1,6,DS1302_Time[1],2);}
	if(TimeSetSelect==2 && TimeSetFlashFlag==1){LCD_ShowString(1,9,"  ");}
	else {LCD_ShowNum(1,9,DS1302_Time[2],2);}
	if(TimeSetSelect==3 && TimeSetFlashFlag==1){LCD_ShowString(2,1,"  ");}
	else {LCD_ShowNum(2,1,DS1302_Time[3],2);}
	if(TimeSetSelect==4 && TimeSetFlashFlag==1){LCD_ShowString(2,4,"  ");}
	else {LCD_ShowNum(2,4,DS1302_Time[4],2);}
	if(TimeSetSelect==5 && TimeSetFlashFlag==1){LCD_ShowString(2,7,"  ");}
	else {LCD_ShowNum(2,7,DS1302_Time[5],2);}
    if(TimeSetSelect==6 && TimeSetFlashFlag==1){LCD_ShowString(1,14,"   ");}
	else {
        if(DS1302_Time[6] == 0)LCD_ShowString(1,14,"Mon");
        else if(DS1302_Time[6] == 1)LCD_ShowString(1,14,"Tue");
        else if(DS1302_Time[6] == 2)LCD_ShowString(1,14,"Wed");
        else if(DS1302_Time[6] == 3)LCD_ShowString(1,14,"Thu");
        else if(DS1302_Time[6] == 4)LCD_ShowString(1,14,"Fri");
        else if(DS1302_Time[6] == 5)LCD_ShowString(1,14,"Sat");
        else if(DS1302_Time[6] == 6)LCD_ShowString(1,14,"Sun");
    }

   
}

void main()
{

    while(1)
    {
        
        Timer0_Init();
        LCD_Init();
        LCD_ShowString(1,1,"Welcome!");
        LCD_ShowString(2,1,"Model1 Model2");
        Delay(500);
        KeyNum=Key();
         
        if(KeyNum == 1)
        {
            
            DS18B20_ConvertT();		//上电先转换一次温度，防止第一次读数据错误
            Delay(1000);			//等待转换完成
            THigh=AT24C02_ReadByte(0);	//读取温度阈值数据
            TLow=AT24C02_ReadByte(1);
            if(THigh>125 || TLow<-55 || THigh<=TLow)
            {
                THigh=20;			//如果阈值非法，则设为默认值
                TLow=15;
            }
            LCD_Init();
            LCD_ShowString(1,1,"T:");
            LCD_ShowString(1,10,"B:");
            LCD_ShowString(2,1,"TH:");
            LCD_ShowString(2,9,"TL:");
            LCD_ShowSignedNum(2,4,THigh,3);
            LCD_ShowSignedNum(2,12,TLow,3);
           
            while(1)
            {
                MatrixKeyNum=MatrixKey();
                
                /*温度读取及显示*/
                DS18B20_ConvertT();	//转换温度
                T=DS18B20_ReadT();	//读取温度
                sprintf(temperature, "%.2f", T);
                LCD_ShowString(1,3,temperature);
                if(has_buzzer) LCD_ShowString(1,12,"P");
                else LCD_ShowString(1,12,"O");
                /*阈值判断及显示*/
                if(MatrixKeyNum)
                {
                    if(MatrixKeyNum==1)	//K1按键，THigh自增
                    {
                        THigh++;
                        if(THigh>125){THigh=125;}
                    }
                    if(MatrixKeyNum==2)	//K2按键，THigh自减
                    {
                        THigh--;
                        if(THigh<=TLow){THigh++;}
                    }
                    if(MatrixKeyNum==3)	//K3按键，TLow自增
                    {
                        TLow++;
                        if(TLow>=THigh){TLow--;}
                    }
                    if(MatrixKeyNum==4)	//K4按键，TLow自减
                    {
                        TLow--;
                        if(TLow<-55){TLow=-55;}
                    }
                    if(MatrixKeyNum==5){
                        has_buzzer = !has_buzzer;
                    }
                    if(MatrixKeyNum==6)break;
                    
                    LCD_ShowSignedNum(2,4,THigh,3);	//显示阈值数据
                    LCD_ShowSignedNum(2,12,TLow,3);
                    AT24C02_WriteByte(0,THigh);		//写入到At24C02中保存
                    Delay(5);
                    AT24C02_WriteByte(1,TLow);
                    Delay(5);
                }
                if(T>THigh)			//越界判断
                {
                    if(has_buzzer)Buzzer_Time(200);
                    LCD_ShowString(1,14,"!!!");
                }
                else if(T<TLow)
                {
                    if(has_buzzer)Buzzer_Time(100);
                    LCD_ShowString(1,14,"!!!");
                }
                else
                {
                    LCD_ShowString(1,14,"---");
                }
            }
        }
        
        if(KeyNum == 2)
        {
            LCD_Init();
            DS1302_Init();
           
            LCD_ShowString(1,1,"    -  -  ");//静态字符初始化显示
            LCD_ShowString(2,1,"  :  :  ");

            DS1302_Time[0] = AT24C02_ReadByte(2);
            DS1302_Time[1] = AT24C02_ReadByte(3);
            DS1302_Time[2] = AT24C02_ReadByte(4);
            DS1302_Time[3] = AT24C02_ReadByte(5);
            DS1302_Time[4] = AT24C02_ReadByte(6);
            DS1302_Time[6] = AT24C02_ReadByte(7);
            
            DS1302_SetTime();//设置时间
           

            
            while(1)
            {
                MatrixKeyNum=MatrixKey();//读取键码
                if(MatrixKeyNum==1)//按键1按下
                {
                    if(MODE==0){MODE=1;TimeSetSelect=0;}//功能切换
                    else if(MODE==1)
                    {
                        MODE=0;
                        // DS1302_WriteByte(0x8E,0x00);
                        DS1302_SetTime();
                        // DS1302_WriteByte(0x8E,0x80);
                    }
                    
                }
                else if(MatrixKeyNum == 6)break;
                switch(MODE)//根据不同的功能执行不同的函数
                {
                    case 0:
                        TimeShow();
                        
                        break;
                    case 1:TimeSet();break;
                }
            }
        }
        // else if(KeyNum == 3){

        //     while(1){
        //         LCD_Init();
                 
        //         LCD_ShowNum(1,1,AT24C02_ReadByte(3),3);
        //         LCD_ShowNum(1,6,AT24C02_ReadByte(2),3);
        //     }
        // }
    }

}


void Timer0_Routine() interrupt 1
{
	static unsigned int T0Count;
    static unsigned int T1Count;
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	T0Count++;
    T1Count++;
	if(T0Count>=20)
	{
		T0Count=0;
		Key_Loop();
        MatrixKey_Loop();	//每20ms调用一次按键驱动函数
       
	}
    if(T1Count>=500){
        T1Count=0;
        TimeSetFlashFlag=!TimeSetFlashFlag;//闪烁标志位取反
    }
}
