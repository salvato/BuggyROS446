#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

void Error_Handler(void);
void TIM3_IRQHandler(void);
// Onboard Push Button
#define B1_Pin       GPIO_PIN_13
#define B1_GPIO_Port GPIOC

// USART connected to USB
#define USART_TX_Pin       GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin       GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA

// Onboard Led
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA

// Timer Pins
#define leftPWM_Pin GPIO_PIN_6
#define leftPWM_GPIO_Port GPIOA
#define rightPWM_Pin GPIO_PIN_7
#define rightPWM_GPIO_Port GPIOA

// JTAG Pins
#define TMS_Pin       GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin       GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin       GPIO_PIN_3
#define SWO_GPIO_Port GPIOB


#ifdef __cplusplus
}
#endif
