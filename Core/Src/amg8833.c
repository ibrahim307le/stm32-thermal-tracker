#include "amg8833.h"
#include <string.h>
#include <math.h>

uint8_t AMG8833_Init(void)
{
    uint8_t power_control = 0x00;
    uint8_t reset_cmd[2] = {0x01, 0x3F};  // Software reset

    // Reset device
    if (HAL_I2C_Master_Transmit(&hi2c1, AMG8833_ADDRESS, reset_cmd, 2, HAL_MAX_DELAY) != HAL_OK)
        return 0;

    HAL_Delay(100);

    // Set to normal mode
    power_control = 0x00;
    if (HAL_I2C_Mem_Write(&hi2c1, AMG8833_ADDRESS, 0x00, I2C_MEMADD_SIZE_8BIT, &power_control, 1, HAL_MAX_DELAY) != HAL_OK)
        return 0;

    HAL_Delay(100);
    return 1;
}

uint8_t AMG8833_ReadPixels(float *pixels)
{
    uint8_t raw[128];  // 2 bytes per pixel
    if (HAL_I2C_Mem_Read(&hi2c1, AMG8833_ADDRESS, 0x80, I2C_MEMADD_SIZE_8BIT, raw, 128, HAL_MAX_DELAY) != HAL_OK)
        return 0;

    for (uint8_t i = 0; i < AMG8833_PIXEL_ARRAY_SIZE; i++)
    {
        uint16_t temp = raw[i * 2] | (raw[i * 2 + 1] << 8);
        if (temp & 0x800)  // negative temperature
            temp |= 0xF000;

        pixels[i] = (int16_t)temp * 0.25f;
    }

    return 1;
}
