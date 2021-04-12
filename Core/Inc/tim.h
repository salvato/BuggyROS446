#pragma once

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

void LeftEncoderTimerInit(void);
void SamplingTimerInit(uint32_t AHRSSamplingPeriod,
                       uint32_t motorSamplingPeriod,
                       uint32_t odometrySamplingPeriod);
void PwmTimerInit(void);
void RightEncoderTimerInit(void);
void SonarEchoTimerInit(void);
void SonarPulseTimerInit(void);
void SendingTimerInit(uint32_t dataSendingPeriod,
                      uint32_t sonarSamplingPeriod,
                      uint32_t mpuSamplingPeriod);

void initTim1GPIO();
void initTim3GPIO();
void initTim4GPIO();
void initTim5GPIO();
void initTim10GPIO();

#ifdef __cplusplus
}
#endif
