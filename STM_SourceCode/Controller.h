#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "hcsr04.h"
#include "servo.h"
#include "uart.h"

#define BufSize 128

typedef enum {
	DIRECTION_INCREASE,
	DIRECTION_DECREASE
} ServoDirection;

typedef enum
{
  GET_DISTANCE,
	SET_SERVO_POSITION,
	SEND_MESSAGE
} SystemState;

typedef struct {
	uint8_t majorVersion;
	uint8_t minorVersion;
	
	
	uint32_t HCSR04_Distance;
	uint16_t Servo_Position;

	ServoDirection Direction;
	SystemState sysState;

	pinStruct_t trigPin;
	pinStruct_t echoPin;
	pinStruct_t servoPin;

	unsigned char UART_RX_BUF[BufSize];
	unsigned char UART_TX_BUF[BufSize];
	
} SysController;

void Init_System_Controller(SysController* controller);

void Servo_IncreasePosition(SysController* controller,Servo* servo, uint8_t increment);
void Assign_UartTxBuf(SysController* controller, Uart* uart);

void Error_Handler(void);
void HAL_MspInit(void);
void HAL_UART_MspInit(UART_HandleTypeDef* huart);
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart);

#endif // CONTROLLER_H
