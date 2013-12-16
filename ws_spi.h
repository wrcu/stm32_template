/*********************************************************************************************************
*
* File                : ws_spi.h
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

#ifndef __SPI_H
#define __SPI_H


#include <stm32f4xx.h>

#define CS_Enable 	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
#define CS_Disable 	GPIO_SetBits(GPIOA,GPIO_Pin_4);

static void SPI_Rcc_Configuration(void);

void SPI_Configuration(void);

//uint16_t SPI2_byte(uint16_t data);


void SPI2_Send_byte(u16 data);
u16 SPI2_Receive_byte(void);


#endif /*__SPI_H*/

