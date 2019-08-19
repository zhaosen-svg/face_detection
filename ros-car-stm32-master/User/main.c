/***********************************************  ËµÃ÷  *****************************************************************
*
*   1.´®¿Ú½ÓÊÕ
*    £¨1£©ÄÚÈİ£ºĞ¡³µ×óÓÒÂÖËÙ¶È,µ¥Î»:mm/s£¨ËùÓĞÊı¾İ¶¼ÎªfloatĞÍ£¬floatĞÍÕ¼4×Ö½Ú£©
*    £¨2£©¸ñÊ½£º10×Ö½Ú [ÓÒÂÖËÙ¶È4×Ö½Ú][×óÂÖËÙ¶È4×Ö½Ú][½áÊø·û"\r\n"2×Ö½Ú]
*
*   2.´®¿Ú·¢ËÍ
*    £¨1£©ÄÚÈİ£ºÀï³Ì¼Æ£¨x,y×ø±ê¡¢ÏßËÙ¶È¡¢½ÇËÙ¶ÈºÍ·½Ïò½Ç£¬µ¥Î»ÒÀ´ÎÎª£ºmm,mm,mm/s,rad/s,rad£¬ËùÓĞÊı¾İ¶¼ÎªfloatĞÍ£¬floatĞÍÕ¼4×Ö½Ú£©
*    £¨2£©¸ñÊ½£º21×Ö½Ú [x×ø±ê4×Ö½Ú][y×ø±ê4×Ö½Ú][·½Ïò½Ç4×Ö½Ú][ÏßËÙ¶È4×Ö½Ú][½ÇËÙ¶È4×Ö½Ú][½áÊø·û"\n"1×Ö½Ú]
*
************************************************************************************************************************/
#include "stm32f10x.h"
#include "stm32f10x_it.h"

#include "delay.h"
#include "nvic_conf.h"
#include "rcc_conf.h"

#include "usart.h"
#include "encoder.h"
#include "contact.h"
#include "gpio_conf.h"
#include "tim2_5_6.h"
#include "odometry.h"
#include "tim2_5_6.h"

#include "stdbool.h"
#include <stdio.h>
#include "string.h"
#include "math.h"
/***********************************************  Êä³ö  *****************************************************************/

char odometry_data[21]={0};   //·¢ËÍ¸ø´®¿ÚµÄÀï³Ì¼ÆÊı¾İÊı×é

float odometry_right=0,odometry_left=0;//´®¿ÚµÃµ½µÄ×óÓÒÂÖËÙ¶È

/***********************************************  ÊäÈë  *****************************************************************/

extern float position_x,position_y,oriention,velocity_linear,velocity_angular;         //¼ÆËãµÃµ½µÄÀï³Ì¼ÆÊıÖµ

extern u8 USART_RX_BUF[USART_REC_LEN];     //´®¿Ú½ÓÊÕ»º³å,×î´óUSART_REC_LEN¸ö×Ö½Ú.
extern u16 USART_RX_STA;                   //´®¿Ú½ÓÊÕ×´Ì¬±ê¼Ç	

extern float Milemeter_L_Motor,Milemeter_R_Motor;     //dtÊ±¼äÄÚµÄ×óÓÒÂÖËÙ¶È,ÓÃÓÚÀï³Ì¼Æ¼ÆËã

/***********************************************  ±äÁ¿  *****************************************************************/

u8 main_sta=0; //ÓÃ×÷´¦ÀíÖ÷º¯Êı¸÷ÖÖif£¬È¥µô¶àÓàµÄflag£¨1´òÓ¡Àï³Ì¼Æ£©£¨2µ÷ÓÃ¼ÆËãÀï³Ì¼ÆÊı¾İº¯Êı£©£¨3´®¿Ú½ÓÊÕ³É¹¦£©£¨4´®¿Ú½ÓÊÕÊ§°Ü£©

union recieveData  //½ÓÊÕµ½µÄÊı¾İ
{
	float d;    //×óÓÒÂÖËÙ¶È
	unsigned char data[4];
}leftdata,rightdata;       //½ÓÊÕµÄ×óÓÒÂÖÊı¾İ

union odometry  //Àï³Ì¼ÆÊı¾İ¹²ÓÃÌå
{
	float odoemtry_float;
	unsigned char odometry_char[4];
}x_data,y_data,theta_data,vel_linear,vel_angular;     //Òª·¢²¼µÄÀï³Ì¼ÆÊı¾İ£¬·Ö±ğÎª£ºX£¬Y·½ÏòÒÆ¶¯µÄ¾àÀë£¬µ±Ç°½Ç¶È£¬ÏßËÙ¶È£¬½ÇËÙ¶È

/****************************************************************************************************************/	
void led_init(void)//stm32 °å×Ó×Ô´øledµÆµÄ³õÊ¼»¯
{
    GPIO_InitTypeDef GPIO_InitStructure;   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOB, ENABLE); 	                                               
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;  																
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 	 
    GPIO_Init(GPIOE, &GPIO_InitStructure);	
    GPIO_SetBits(GPIOE,GPIO_Pin_5);	
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;			
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 
    GPIO_SetBits(GPIOB,GPIO_Pin_5);
}
int main(void)
{		
	u8 t=0;
	u8 i=0,j=0,m=0;

  RCC_Configuration();      //ÏµÍ³Ê±ÖÓÅäÖÃ		
	NVIC_Configuration();     //ÖĞ¶ÏÓÅÏÈ¼¶ÅäÖÃ
	GPIO_Configuration(); 	  //µç»ú·½Ïò¿ØÖÆÒı½ÅÅäÖÃ
	USART1_Config();	      //´®¿Ú1ÅäÖÃ
    
  TIM2_PWM_Init();	      //PWMÊä³ö³õÊ¼»¯
	ENC_Init();               //µç»ú´¦Àí³õÊ¼»¯
	TIM5_Configuration();     //ËÙ¶È¼ÆËã¶¨Ê±Æ÷³õÊ¼»¯
	TIM1_Configuration();     //Àï³Ì¼Æ·¢²¼¶¨Ê±Æ÷³õÊ¼»¯
	led_init();
	while (1)
	{		
		if(main_sta&0x01)//Ö´ĞĞ·¢ËÍÀï³Ì¼ÆÊı¾İ²½Öè
		{
    //Àï³Ì¼ÆÊı¾İ»ñÈ¡
			x_data.odoemtry_float=position_x;//µ¥Î»mm
			y_data.odoemtry_float=position_y;//µ¥Î»mm
			theta_data.odoemtry_float=oriention;//µ¥Î»rad
			vel_linear.odoemtry_float=velocity_linear;//µ¥Î»mm/s
			vel_angular.odoemtry_float=velocity_angular;//µ¥Î»rad/s
            
     //½«ËùÓĞÀï³Ì¼ÆÊı¾İ´æµ½Òª·¢ËÍµÄÊı×é
			for(j=0;j<4;j++)
			{
				odometry_data[j]=x_data.odometry_char[j];
				odometry_data[j+4]=y_data.odometry_char[j];
				odometry_data[j+8]=theta_data.odometry_char[j];
				odometry_data[j+12]=vel_linear.odometry_char[j];
				odometry_data[j+16]=vel_angular.odometry_char[j];
			}
			odometry_data[20]='\n';//Ìí¼Ó½áÊø·û
			//·¢ËÍÊı¾İÒª´®¿Ú
			for(i=0;i<21;i++)
			{
				USART_ClearFlag(USART1,USART_FLAG_TC);  //ÔÚ·¢ËÍµÚÒ»¸öÊı¾İÇ°¼Ó´Ë¾ä£¬½â¾öµÚÒ»¸öÊı¾İ²»ÄÜÕı³£·¢ËÍµÄÎÊÌâ				
				USART_SendData(USART1,odometry_data[i]);//·¢ËÍÒ»¸ö×Ö½Úµ½´®¿
				GPIO_ResetBits(GPIOB,GPIO_Pin_5);
				while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	//µÈ´ı·¢ËÍ½áÊø			
			}
			main_sta&=0xFE;//Ö´ĞĞ¼ÆËãÀï³Ì¼ÆÊı¾İ²½Öè
		}
		if(main_sta&0x02)//Ö´ĞĞ¼ÆËãÀï³Ì¼ÆÊı¾İ²½Öè
		{
			odometry(Milemeter_R_Motor,Milemeter_L_Motor);//¼ÆËãÀï³Ì¼Æ
			main_sta&=0xFD;//Ö´ĞĞ·¢ËÍÀï³Ì¼ÆÊı¾İ²½Öè
		} 
		if(main_sta&0x08)        //µ±·¢ËÍÖ¸ÁîÃ»ÓĞÕıÈ·½ÓÊÕÊ±
		{
			USART_ClearFlag(USART1,USART_FLAG_TC);  //ÔÚ·¢ËÍµÚÒ»¸öÊı¾İÇ°¼Ó´Ë¾ä£¬½â¾öµÚÒ»¸öÊı¾İ²»ÄÜÕı³£·¢ËÍµÄÎÊÌâ
            for(m=0;m<3;m++)
            {
                USART_SendData(USART1,0x00);	
                while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
            }		
        USART_SendData(USART1,'\n');	
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	
        main_sta&=0xF7;
		}
		if(USART_RX_STA&0x8000)  // ´®¿Ú1½ÓÊÕº¯Êı
		{			
			 GPIO_ResetBits(GPIOE,GPIO_Pin_5);
			  
            //½ÓÊÕ×óÓÒÂÖËÙ¶È
            for(t=0;t<4;t++)
            {
                rightdata.data[t]=USART_RX_BUF[t];
                leftdata.data[t]=USART_RX_BUF[t+4];
            }
            //´¢´æ×óÓÒÂÖËÙ¶È
            odometry_right=rightdata.d;//µ¥Î»mm/s
            odometry_left=leftdata.d;//µ¥Î»mm/s
			       USART_RX_STA=0;//Çå³ş½ÓÊÕ±êÖ¾Î»
		}
        car_control(rightdata.d,leftdata.d);	 //½«½ÓÊÕµ½µÄ×óÓÒÂÖËÙ¶È¸³¸øĞ¡³µ	
	}//end_while
}//end main
/*********************************************END OF FILE**************************************************/
