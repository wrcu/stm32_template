#include "stm32f4_discovery.h"
#include <stdio.h>

int main(void)
{
	int i;
	USART3_Config();
	//USART3_printf(USART3, "\r\n This is a USART3_printf demo \r\n");
	//USART3_printf(USART3, "\r\n ("__DATE__" - "__TIME__") \r\n");
	
	for(;;)
	{
		USART3_printf("Hello\n");
		for(i=0; i<30000000; i++);
		printf("\r\n this is a printf demo \r\n");
		printf("\r\n ("__DATE__" - "__TIME__") \r\n");
	}
}

