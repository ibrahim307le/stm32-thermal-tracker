/* tim.h - Header file for timer initialization */

#ifndef __TIM_H__
#define __TIM_H__

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;

void MX_TIM1_Init(void);
void MX_TIM2_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */
