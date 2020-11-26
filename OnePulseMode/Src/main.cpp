#include "main.h"
#include "string.h" // for memset()
#include "stm32f4xx_ll_tim.h"


TIM_HandleTypeDef  htimer;
UART_HandleTypeDef huart2;


static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIMER_Init(void);
static void MX_USART2_UART_Init(void);
static void TimerCaptureCompare_Callback(void);


double timerClockFrequency = 10.0e6; // 10 MHz (100ns period)
double pulseDelay          = 10.0e-6; // in seconds
double pulsewidth          = 10.0e-6;// in seconds


/* Measured pulse delay (in us) */
__IO uint32_t uwMeasuredDelay = 0;
/* Measured pulse length (in us) */
__IO uint32_t uwMeasuredPulseLength = 0;


int
main(void) {
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init(); // Led on PA5 - CN10 --> Pin11
    MX_TIMER_Init();
    MX_USART2_UART_Init();

    HAL_Delay(1000);
    while(1) {
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
        /* Enable counter. Note that the counter will stop automatically at the     */
        /* next update event (UEV).                                                 */
        LL_TIM_EnableCounter(TIM2);
        HAL_Delay(100);
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
    }
}


//                                            ___________________________
//                                           |                           |
// CH1 ______________________________________|                           |____
//                             <---Delay----><------Pulse--------------->
//
void
MX_TIMER_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));

    __HAL_RCC_GPIOA_CLK_ENABLE();

    // TIM2 GPIO Configuration
    //    PA0  ------> TIM2_CH1 (Output CN7  - 28)
    //    PA1  ------> TIM2_CH1 (Output CN7  - 30)
    //    PB10 ------> TIM2_CH1 (Output CN10 - 25)
    GPIO_InitStruct.Pin       = GPIO_PIN_0;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    //*****************************************************+
    // Configure the NVIC to handle TIM2 Global Interrupt  |
    //*****************************************************+
    NVIC_SetPriority(TIM2_IRQn, 0);
    NVIC_EnableIRQ(TIM2_IRQn); // The Global Interrupt

    uint32_t uwPrescalerValue = (uint32_t) (SystemCoreClock/timerClockFrequency)-1;
    uint16_t PulseWidthNumber = pulsewidth*timerClockFrequency;
    uint16_t PulseDelayNumber = pulseDelay*timerClockFrequency;
    uint16_t period = PulseWidthNumber+PulseDelayNumber;

    __HAL_RCC_TIM2_CLK_ENABLE();

    memset(&htimer, 0, sizeof(htimer));
    htimer.Instance = TIM2;
    htimer.Init.Prescaler         = uwPrescalerValue;
    htimer.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htimer.Init.Period            = period; // Pulse Total
    htimer.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htimer.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if(HAL_TIM_OnePulse_Init(&htimer, TIM_OPMODE_SINGLE) != HAL_OK) {
        Error_Handler();
    }

    LL_TIM_OC_SetCompareCH1(TIM2, PulseDelayNumber); // Pulse Delay
    LL_TIM_OC_SetMode(TIM2,  LL_TIM_CHANNEL_CH1,  LL_TIM_OCMODE_PWM2);
    LL_TIM_OC_ConfigOutput(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH | LL_TIM_OCIDLESTATE_LOW);

    // Enable the capture/compare interrupt for channel 1
    LL_TIM_EnableIT_CC1(TIM2);

    //*************************+
    // Start pulse generation  |
    //*************************+
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1);
    /* Enable TIM3 outputs */
    LL_TIM_EnableAllOutputs(TIM2);
    /* Enable auto-reload register preload */
    LL_TIM_EnableARRPreload(TIM2);
    /* Force update generation */
    LL_TIM_GenerateEvent_UPDATE(TIM2);
}


void
MX_USART2_UART_Init(void) {
    huart2.Instance = USART2;
    huart2.Init.BaudRate     = 9600;
    huart2.Init.WordLength   = UART_WORDLENGTH_8B;
    huart2.Init.StopBits     = UART_STOPBITS_1;
    huart2.Init.Parity       = UART_PARITY_NONE;
    huart2.Init.Mode         = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        Error_Handler();
    }
}



void
MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = B1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LD2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);
}


void
HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base) {
    if(htim_base->Instance==TIM2) {
        __HAL_RCC_TIM2_CLK_DISABLE();
    }
}


void
Error_Handler(void) {

}


void
EXTI15_10_IRQHandler(void) { // defined in file "startup_stm32f411xe.s"
    // Non dovremmo controllare l'origine dell'interrupt ???
    HAL_GPIO_EXTI_IRQHandler(B1_Pin);
}


void
HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if(GPIO_Pin == B1_Pin) {

    }
}


void
TIM2_IRQHandler(void) {
    if(LL_TIM_IsActiveFlag_CC1(TIM2) == 1) {
        LL_TIM_ClearFlag_CC1(TIM2);
        TimerCaptureCompare_Callback();
    }
}


void
TimerCaptureCompare_Callback(void) {
    uint32_t CNT;
    uint32_t PSC;
    uint32_t ARR;

    CNT = LL_TIM_GetCounter(TIM2);
    PSC = LL_TIM_GetPrescaler(TIM2);
    ARR = LL_TIM_GetAutoReload(TIM2);

    uwMeasuredDelay = (CNT*timerClockFrequency)/(SystemCoreClock/(PSC+1));
    uwMeasuredPulseLength = ((ARR-CNT)*timerClockFrequency)/(SystemCoreClock/(PSC+1));
}


/* System Clock Configuration
//    The system Clock is configured as follow :
//        System Clock source            = PLL (HSI)
//        SYSCLK(Hz)                     = 100000000
//        HCLK(Hz)                       = 100000000
//        AHB Prescaler                  = 1
//        APB1 Prescaler                 = 2
//        APB2 Prescaler                 = 1
//        HSI Frequency(Hz)              = 16000000
//        PLL_M                          = 16
//        PLL_N                          = 400
//        PLL_P                          = 4
//        PLL_Q                          = 7
//        VDD(V)                         = 3.3
//        Main regulator output voltage  = Scale2 mode
//        Flash Latency(WS)              = 3
*/
void
SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    memset(&RCC_OscInitStruct, 0, sizeof(RCC_OscInitStruct));
    memset(&RCC_ClkInitStruct, 0, sizeof(RCC_ClkInitStruct));

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM            = 16;
    RCC_OscInitStruct.PLL.PLLN            = 400;
    RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ            = 7;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK   |
            RCC_CLOCKTYPE_SYSCLK |
            RCC_CLOCKTYPE_PCLK1  |
            RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}


#ifdef  USE_FULL_ASSERT
void
assert_failed(uint8_t *file, uint32_t line) {
    //  User can add his own implementation to report the file name and line number,
    //     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line)
}
#endif /* USE_FULL_ASSERT */
