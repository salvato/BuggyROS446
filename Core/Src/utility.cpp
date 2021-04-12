#include "utility.h"
#include "stm32f4xx_hal.h"
#include "string.h" // for memset()

// System Clock Configuration
//    The system Clock is configured as follow :
//        System Clock source            = PLL (HSI)
//        SYSCLK(Hz)                     = 100000000
//        HCLK(Hz)                       = 100000000
//        AHB Prescaler                  = 1
//        APB1 Prescaler                 = 2
//        APB2 Prescaler                 = 1
//        HSI Frequency(Hz)              = 16000000
//        PLL_M                          = 16
//        PLL_N                          = 336
//        PLL_P                          = 4
//        PLL_Q                          = 7
//        VDD(V)                         = 3.3
//        Main regulator output voltage  = Scale2 mode
//        Flash Latency(WS)              = 3

void
Error_Handler(int error) {
    while(true) {
        HAL_GPIO_TogglePin(Alarm_GPIO_Port, Alarm_GPIO_Pin);
        HAL_Delay(500+error);
    }
}


void
GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

    // Push Button
    GPIO_InitStruct.Pin  = B1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);

    // Led On Board
    GPIO_InitStruct.Pin   = LD2_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

    // Left Motor IN1
    GPIO_InitStruct.Pin   = LMOTOR_IN1;
    HAL_GPIO_Init(LM_IN1_PORT, &GPIO_InitStruct);

    // Left Motor IN2
    GPIO_InitStruct.Pin   = LMOTOR_IN2;
    HAL_GPIO_Init(LM_IN2_PORT, &GPIO_InitStruct);

    // Right Motor IN3
    GPIO_InitStruct.Pin   = LMOTOR_IN3;
    HAL_GPIO_Init(RM_IN3_PORT, &GPIO_InitStruct);

    // Right Motor IN4
    GPIO_InitStruct.Pin   = LMOTOR_IN4;
    HAL_GPIO_Init(RM_IN4_PORT, &GPIO_InitStruct);

    // Alarm Pin
    GPIO_InitStruct.Pin   = Alarm_GPIO_Pin;
    HAL_GPIO_Init(Alarm_GPIO_Port, &GPIO_InitStruct);
}


// System Clock Configuration
// The system Clock is configured as follow :
//   System Clock source            = PLL (HSE)
//   SYSCLK(Hz)                     = 180000000
//   HCLK(Hz)                       = 180000000
//   AHB Prescaler                  = 1
//   APB1 Prescaler                 = 4
//   APB2 Prescaler                 = 2
//   HSE Frequency(Hz)              = 8000000
//   PLL_M                          = 8
//   PLL_N                          = 360
//   PLL_P                          = 2
//   PLL_Q                          = 7
//   PLL_R                          = 2
//   VDD(V)                         = 3.3
//   Main regulator output voltage  = Scale1 mode
//   Flash Latency(WS)              = 5


void
SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    memset(&RCC_OscInitStruct, 0, sizeof(RCC_OscInitStruct));
    memset(&RCC_ClkInitStruct, 0, sizeof(RCC_ClkInitStruct));  
    HAL_StatusTypeDef ret = HAL_OK;

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_BYPASS;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM       = 8;
    RCC_OscInitStruct.PLL.PLLN       = 360;
    RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ       = 7;
    RCC_OscInitStruct.PLL.PLLR       = 2;

    ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
    if(ret != HAL_OK) {
        Error_Handler();
    }
    /* Activate the OverDrive to reach the 180 MHz Frequency */
    ret = HAL_PWREx_EnableOverDrive();
    if(ret != HAL_OK) {
        Error_Handler();
    }
    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
    RCC_ClkInitStruct.ClockType      = (RCC_CLOCKTYPE_SYSCLK |
                                        RCC_CLOCKTYPE_HCLK   |
                                        RCC_CLOCKTYPE_PCLK1  |
                                        RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
    if(ret != HAL_OK) {
        Error_Handler();
    }
}

