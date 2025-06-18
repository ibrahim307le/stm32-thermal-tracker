#ifndef __AMG8833_H__
#define __AMG8833_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>

#define AMG8833_ADDRESS          (0x68 << 1)  // 7-bit address shifted for HAL
#define AMG8833_PIXEL_ARRAY_SIZE 64

extern I2C_HandleTypeDef hi2c1;

uint8_t AMG8833_Init(void);
uint8_t AMG8833_ReadPixels(float *pixels);

#endif
