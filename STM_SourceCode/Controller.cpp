#include "Controller.h"
#include <string.h>

extern UART_HandleTypeDef huart2;

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
void Assign_UartTxBuf(SysController* controller, Uart* uart)
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
	
	uart->GetCRC(controller->UART_TX_BUF, 128);
	
}

void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);

}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(huart->Instance==USART2)
  {
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  }

}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART2)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);
  }

}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
