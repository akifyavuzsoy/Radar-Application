#include "uart.h"

Uart::Uart(UART_HandleTypeDef *huart2, uint32_t baudRate)
{
	// UART Config
	
	huart2->Instance = USART2;
  huart2->Init.BaudRate = baudRate;
  huart2->Init.WordLength = UART_WORDLENGTH_8B;
  huart2->Init.StopBits = UART_STOPBITS_1;
  huart2->Init.Parity = UART_PARITY_NONE;
  huart2->Init.Mode = UART_MODE_TX_RX;
  huart2->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2->Init.OverSampling = UART_OVERSAMPLING_16;
	
	HAL_UART_Init(huart2);
	
}

void Uart::sentDataToUART(UART_HandleTypeDef huart2, const uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)pData, Size, Timeout);
}

char Uart::CRC_check(char message[],unsigned int message_length)
{
		volatile unsigned int crc[2];
		volatile unsigned int CRCFull = 0xFFFF;
		volatile unsigned int CRCHigh = 0xFF, CRCLow = 0xFF;
		volatile unsigned int CRCLSB;
		volatile unsigned int i=0;
		volatile unsigned int j=0;
		char CRC_OK=0;
    //Function expects a modbus message of any length as well as a 2 byte CRC array in which to 
    //return the CRC values:
    for (i = 0; i < message_length-2; i++)
    {
        CRCFull = (unsigned int)(CRCFull ^ message[i]);

        for (j = 0; j < 8; j++)
        {
            CRCLSB =  (unsigned int)( CRCFull & 0x0001);
            CRCFull = (unsigned int)((CRCFull >> 1) & 0x7FFF);

            if (CRCLSB == 1)
                CRCFull = (unsigned int)(CRCFull ^ 0xA001);
        }
    }
    crc[1] = CRCHigh = (unsigned int)((CRCFull >> 8) & 0xFF);
    crc[0] = CRCLow  = (unsigned int)( CRCFull & 0xFF);
    if((crc[0] == message[message_length-2]) && (crc[1] == message[message_length-1]))
    CRC_OK = 1;
    else
    CRC_OK = 0;
    return CRC_OK;		
}

void Uart::GetCRC(char message[],unsigned int message_length)
{
    volatile unsigned int crc[2];
		volatile unsigned int CRCFull = 0xFFFF;
		volatile unsigned int CRCHigh = 0xFF, CRCLow = 0xFF;
		volatile unsigned int CRCLSB;
		volatile unsigned int i=0;
		volatile unsigned int j=0;
		volatile char CRC_OK=0;
    //Function expects a modbus message of any length as well as a 2 byte CRC array in which to 
    //return the CRC values:

    for (i = 0; i < message_length-2; i++)
    {
        CRCFull = (unsigned int)(CRCFull ^ message[i]);

        for (j = 0; j < 8; j++)
        {
            CRCLSB = (unsigned int)(CRCFull & 0x0001);
            CRCFull = (unsigned int)((CRCFull >> 1) & 0x7FFF);

            if (CRCLSB == 1)
                CRCFull = (unsigned int)(CRCFull ^ 0xA001);
        }
    }
    crc[1] = CRCHigh = (unsigned int)((CRCFull >> 8) & 0xFF);
    crc[0] = CRCLow  = (unsigned int)( CRCFull & 0xFF);
    message[message_length-2] = crc[0];
    message[message_length-1] = crc[1];
}

void Uart::GetCRC(unsigned char message[],unsigned int message_length)
{
    volatile unsigned int crc[2];
		volatile unsigned int CRCFull = 0xFFFF;
		volatile unsigned int CRCHigh = 0xFF, CRCLow = 0xFF;
		volatile unsigned int CRCLSB;
		volatile unsigned int i=0;
		volatile unsigned int j=0;
		volatile char CRC_OK=0;
    //Function expects a modbus message of any length as well as a 2 byte CRC array in which to 
    //return the CRC values:

    for (i = 0; i < message_length-2; i++)
    {
        CRCFull = (unsigned int)(CRCFull ^ message[i]);

        for (j = 0; j < 8; j++)
        {
            CRCLSB = (unsigned int)(CRCFull & 0x0001);
            CRCFull = (unsigned int)((CRCFull >> 1) & 0x7FFF);

            if (CRCLSB == 1)
                CRCFull = (unsigned int)(CRCFull ^ 0xA001);
        }
    }
    crc[1] = CRCHigh = (unsigned int)((CRCFull >> 8) & 0xFF);
    crc[0] = CRCLow  = (unsigned int)( CRCFull & 0xFF);
    message[message_length-2] = crc[0];
    message[message_length-1] = crc[1];
}



