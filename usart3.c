#include "stm32f4xx_conf.h"
#include <stdarg.h>

/*
 * function name: 	itoa
 * description:		integer to ascii
 * input:			-radix = 10
					-value
					-buf
 * output:			none
 * refrence from:	stm32庫開發實踐指南
*/
static char *itoa(int value, char *string, int radix)
{
	int     i, d;
	int     flag = 0;
	char    *ptr = string;
	
	/* This implementation only works for decimal numbers. */
	if (radix != 10)
	{
	    *ptr = 0;
	    return string;
	}
	
	if (!value)
	{
	    *ptr++ = 0x30;
	    *ptr = 0;
	    return string;
	}
	
	/* if this is a negative value insert the minus sign. */
	if (value < 0)
	{
	    *ptr++ = '-';
	
	    /* Make the value positive. */
	    value *= -1;
	}
	
	for (i = 10000; i > 0; i /= 10)
	{
	    d = value / i;
	
	    if (d || flag)
	    {
	        *ptr++ = (char)(d + 0x30);
			value -= (d * i);
			flag = 1;
		}
	}

	/* Null terminate the string. */
	*ptr = 0;
	
	return string;

} /* NCL_Itoa */


/*
 * function name: 	USART3_Config
 * description:		USART3 GPIO configure, 9600 8-N-1
 * input:			none
 * output:			none
 * refrence from:	http://lolikitty.pixnet.net/blog/post/143466068
*/
void USART3_Config(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

   /******** 啟用 GPIOC、USART3 的 RCC 時鐘 ********/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  
	/******** 將 PC10、PC11 連接至 USART3 ********/
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);

	/******** 設定 PC10 為 Tx 覆用  ********/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 使用推挽式輸出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; // 使用上拉電阻
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // 設置為覆用
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // 設定第 10 腳
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 設定 GPIO 速度為 50 MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure); // 套用以上 GPIO 設置，並初始化 GPIOC

	/******** 設定 PC11 為 Rx 覆用  ********/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // 設置為覆用
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; // 設定第 11 腳
	GPIO_Init(GPIOC, &GPIO_InitStructure); // 套用以上 GPIO 設置，並初始化 GPIOC

	/******** USART 基本參數設定 ********/
	USART_InitStructure.USART_BaudRate = 9600; // 設定 USART 包率 (每秒位元數) 為 9600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 設定 USART 傳輸的資料位元為 8
	USART_InitStructure.USART_StopBits = USART_StopBits_1; // 設定 USART 停止位元為 1
	USART_InitStructure.USART_Parity = USART_Parity_No; // 不使用同位元檢查
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 不使用流量控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  // 設定 USART 模式為 Rx (接收) 、 Tx (傳送)
	USART_Init(USART3, &USART_InitStructure); // 套用以上 USART 設置，並初始化UART3

	/******** 啟用 USART3 ********/
	USART_Cmd(USART3, ENABLE);
}

char USART3_printf(char *string){
    while(*string){
        /* 傳送訊息至 USART3 */
        USART_SendData(USART3, (unsigned short int) *string++);
 
        /* 等待訊息傳送完畢 */
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    }
}

/*
 * function name: 	printf
 * description:		format and print data
 * input:			none
 * output:			none
 * refrence from:	http://lolikitty.pixnet.net/blog/post/143466068
*/
void printf(uint8_t *Data,...)
{
	const char *s;
	int d;   
	char buf[16];
	
	va_list ap;
	va_start(ap, Data);
	
	while ( *Data != 0)     // 瓚剿岆瘁善湛趼睫揹賦旰睫
	{				                          
		if ( *Data == 0x5c )  //'\'
	{									  
	switch ( *++Data )
	{
		case 'r':							          //隙陬睫
			USART_SendData(USART3, 0x0d);
			Data ++;
		break;
		
		case 'n':							          //遙俴睫
			USART_SendData(USART3, 0x0a);	
			Data ++;
		break;
		
		default:
			Data ++;
		break;
	}			 
	}
	else if ( *Data == '%')
	{									  //
	switch ( *++Data )
	{				
		case 's':										  //趼睫揹
			s = va_arg(ap, const char *);
	for ( ; *s; s++) 
	{
		USART_SendData(USART3,*s);
		while( USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET );
	}
		Data++;
		break;
	
	case 'd':										//坋輛秶
	d = va_arg(ap, int);
	itoa(d, buf, 10);
	for (s = buf; *s; s++) 
	{
		USART_SendData(USART3,*s);
		while( USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET );
	}
	Data++;
	break;
		 default:
				Data++;
		    break;
	}		 
	} /* end of else if */
	else USART_SendData(USART3, *Data++);
	while( USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET );
	}
}


