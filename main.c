/*********************************************************************************************************
*
* File                : main.c
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  : 
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "L3G4200D.h"
#include "ws_spi.h"
#include <string.h>
#include <stdio.h>
//#include "usart.h"
#include "usart3.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4_discovery.h"
//#include "memory.h"
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

float xyz[3]={0};
__IO float angle_x=0,angle_y=0,angle_z=0;
__IO unsigned char  f=0,t=0; 


/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);
//void USART_Configuration(void);
void USART3_Config(void);


/*******************************************************************************
* Function Name  : Delay
* Description    : Delay Time
* Input          : - nCount: Delay Time
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void  Delay (uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}


/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Timer_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	SystemInit();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	 
	//RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	TIM_DeInit(TIM2); 
	
	//TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseStructure.TIM_Period = 1000-1;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, & TIM_TimeBaseStructure);
	
  TIM_PrescalerConfig(TIM2,0x2cf,TIM_PSCReloadMode_Immediate);//時鐘分頻係數為719，所以定時器時鐘為100 000；
	TIM_ARRPreloadConfig(TIM2, DISABLE);
	TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
  TIM_Cmd(TIM2,ENABLE);
}
void NVIC_Configuration(void)

{
  NVIC_InitTypeDef NVIC_InitStructure;
	#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
 #else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
 #endif
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
void data_int(float *between)
{
  uint16_t i;
	float data[100][3]={0};
	for(i=0;i<100;i++)
	{ 
		//while(f)
		//{
		  READ_L3G4200D(data[i],data[i]+1,data[i]+2);
		//f=0;
		//}
		Delay_1ms(10);
		between[0]+=data[i][0];
		between[1]+=data[i][1];
		between[2]+=data[i][2];
  }
  for(i=0;i<3;i++)
	  between[i]=between[i]/100;
	

}
int main(void)
{
 float bet[3]={0};

 // GPIO_Configuration(); 
 //USART_Configuration();
 USART3_Config();
  SPI_Configuration();
		
	Init_L3G4200D();
	
	NVIC_Configuration();
	Timer_Configuration();
	printf("\rl3f4200D ID ..\r\n");

 printf("\r\n******In the initialization!  Do not move the module!!******\r\n");

  data_int(bet);


		printf("bet0=%f rad/s\r\n",bet[0]); // xyz[x]hi角速度。
		printf("bet1=%f rad/s\r\n",bet[1]);
		printf("bet2=%f rad/s\r\n",bet[2]);	

	
  printf("\r\n*************************************************\r\n");

  while (1)
  {

		if(f==1)
		{
		 READ_L3G4200D(xyz,xyz+1,xyz+2);
		
			xyz[0]=(int)(xyz[0]-bet[0]); 
			xyz[1]=(int)(xyz[1]-bet[1]); //
			xyz[2]=(int)(xyz[2]-bet[2]);
			
			xyz[0]=(int)(xyz[0]); 
			xyz[1]=(int)(xyz[1]); 
			xyz[2]=(int)(xyz[2]); 

	    xyz[0]/=100;
	    xyz[1]/=100;
	  	xyz[2]/=100;
			angle_x=(xyz[0])+angle_x;  if(angle_x>=360)angle_x-=360;
		  angle_y=(xyz[1])+angle_y;  if(angle_y>=360)angle_y-=360;
		  angle_z=(xyz[2])+angle_z;  if(angle_z>=360)angle_z-=360;
			//TIM_Cmd(TIM2,ENABLE);
	    f=0;
		t++;
			if(t==100)t=0;
		//if(test==10)test=0;
		}

		if(t==10){
	  printf("\r\n******  L3G4200D  ******\r\n");
			
		printf("x=%f rad/s\r\n",xyz[0]); // xyz[x]hi角速度。
		printf("y=%f rad/s\r\n",xyz[1]);
		printf("z=%f rad/s\r\n",xyz[2]);	
			
		printf("angle_x=%f Degree\r\n",angle_x); //angle_x 是角度。
		printf("angle_y=%f Degree\r\n",angle_y);
		printf("angle_z=%f Degree\r\n",angle_z);
		}
    

  }
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configure GPIO Pin
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
/*void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOF , ENABLE); 	
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE); 			
	

 *  LED1 -> PF6 , LED2 -> PF7 , LED3 -> PF8 , LED4 -> PF9
					 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}
*/
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

