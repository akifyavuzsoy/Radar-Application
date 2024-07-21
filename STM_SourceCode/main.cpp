#include "Controller.h"

SysController syscontroller; 

int main(void)
{	
	syscontroller.majorVersion = 01;
	syscontroller.minorVersion = 01;
	
	HAL_Init();
	// For HCSR04 Sensor 
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_TIM1_CLK_ENABLE();
	// For Servo Motor
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_TIM4_CLK_ENABLE();
	
//	// Sensor pins
//	pinStruct_t trigPin = {GPIOA, GPIO_PIN_10};
//	pinStruct_t echoPin = {GPIOA, GPIO_PIN_8};
//	pinStruct_t servoPin = {GPIOB, GPIO_PIN_8};
	
	Init_System_Controller(&syscontroller);
	
	// For HCSR04 Sensor 
	HCSR04_TrigPinPWMInit(syscontroller.trigPin, TIM1, GPIO_AF1_TIM1, TIM_CHANNEL_3);
	static HCSR04 hcsr04(syscontroller.echoPin, TIM1, GPIO_AF1_TIM1);												// Created object for HCSR04 Sensor
	
	// For Servo Motor
	static Servo servo(syscontroller.servoPin, TIM4, GPIO_AF2_TIM4, TIM_CHANNEL_3);					// Created object for Servo Motor
	
	servo.TurnShaft((uint16_t)90);
	HAL_Delay(1000);
	
	
	while(1)
	{
		// TODO: System state için bir enum olusturulacak ve State Machine seklinde while çalistirilacak...
		switch(syscontroller.sysState)
		{
			case GET_DISTANCE:
			{
				syscontroller.HCSR04_Distance = hcsr04.GetDistance();
				syscontroller.sysState = SET_SERVO_POSITION;
				break;
			}
			case SET_SERVO_POSITION:
			{
				Servo_IncreasePosition(&syscontroller, &servo, 10);
				syscontroller.sysState = SEND_MESSAGE;
				break;
			}
			case SEND_MESSAGE:
			{
				// Send message via UART
				Assign_UartTxBuf(&syscontroller);
				syscontroller.sysState = GET_DISTANCE;
				break;
			}
		}
		HAL_Delay(250);
	}
}


extern "C" void SysTick_Handler(void)
{
	HAL_IncTick();
}
