#include "Controller.h"
#include <string.h>

void Init_System_Controller(SysController* controller)
{
	controller->HCSR04_Distance = 0;
	controller->Servo_Position = 90;
	controller->Direction = DIRECTION_INCREASE;
	controller->sysState = GET_DISTANCE;

	// Initialize pins
	controller->trigPin.port = GPIOA;
	controller->trigPin.selectedPin = GPIO_PIN_10;
	controller->echoPin.port = GPIOA;
	controller->echoPin.selectedPin = GPIO_PIN_8;
	
	controller->servoPin.port = GPIOB;
	controller->servoPin.selectedPin = GPIO_PIN_8;

	// Clear UART buffers
	memset(controller->UART_RX_BUF, 0, BufSize);
	memset(controller->UART_TX_BUF, 0, BufSize);
}

// TODO: void yerine uint8_tt kullanarak hata aldiginda -1 döndürecek try-catch kullanilacak...
void Servo_IncreasePosition(SysController* controller,Servo* servo, uint8_t increment)
{
	if (controller->Direction == DIRECTION_INCREASE) {
			controller->Servo_Position += increment;
			if (controller->Servo_Position >= 180) {
					controller->Servo_Position = 180;
					controller->Direction = DIRECTION_DECREASE;
			}
	} else {
			controller->Servo_Position -= increment;
			if (controller->Servo_Position <= 0) {
					controller->Servo_Position = 0;
					controller->Direction = DIRECTION_INCREASE;
			}
	}
	
	servo->TurnShaft(controller->Servo_Position);
}

// 
void Assign_UartTxBuf(SysController* controller)
{
	memset(controller->UART_TX_BUF, 0, sizeof(controller->UART_TX_BUF));
	
	controller->UART_TX_BUF[0] = 0xFA;
	controller->UART_TX_BUF[1] = 0xFB;
	controller->UART_TX_BUF[2] = controller->majorVersion;
	controller->UART_TX_BUF[3] = controller->minorVersion;
	
	// Servo_Position (16 bit) degerini diziye yerlestir
	controller->UART_TX_BUF[8] = (controller->Servo_Position >> 8) & 0xFF;  // MSB
	controller->UART_TX_BUF[9] = controller->Servo_Position & 0xFF;         // LSB
	
	// HCSR04_Distance (32 bit) degerini diziye yerlestir
	controller->UART_TX_BUF[10] = (controller->HCSR04_Distance >> 24) & 0xFF; // En yüksek byte
	controller->UART_TX_BUF[11] = (controller->HCSR04_Distance >> 16) & 0xFF;
	controller->UART_TX_BUF[12] = (controller->HCSR04_Distance >> 8) & 0xFF;
	controller->UART_TX_BUF[13] = controller->HCSR04_Distance & 0xFF;         // En düsük byte
	
	controller->UART_TX_BUF[124] = 0xFB;
	controller->UART_TX_BUF[125] = 0xFA;
	
	GetCRC(controller->UART_TX_BUF, 128);
	
}

// TODO: Send To UART_TX_BUF array via Uart....



char CRC_check(char message[],unsigned int message_length)
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


void GetCRC(char message[],unsigned int message_length)
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

void GetCRC(unsigned char message[],unsigned int message_length)
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
