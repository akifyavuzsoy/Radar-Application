#ifndef UART_H
#define UART_H

#include "pinmap.h"

static char test[25]="AKIF YAVUZSOY\r\n";
static uint16_t test_data=0;

class Uart
{
	private:
		uint8_t i;
	
	public:
		Uart::Uart(UART_HandleTypeDef *huart2, uint32_t baudRate);
	
		char CRC_check(char message[],unsigned int message_length);
		void GetCRC(char message[],unsigned int message_length);
		void GetCRC(unsigned char message[],unsigned int message_length);
		void Uart::sentDataToUART(UART_HandleTypeDef huart2, const uint8_t *pData, uint16_t Size, uint32_t Timeout);
		void uartTransmit(char *string);
	
};

#endif //UART_H
