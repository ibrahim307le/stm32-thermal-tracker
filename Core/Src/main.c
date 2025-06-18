/* main.c - STM32F407 + AMG8833 + USB CDC Output + Servo Pointing */

#include "main.h"
#include "usb_device.h"
#include "i2c.h"
#include "tim.h"
#include <stdio.h>
#include <string.h>
#include "usbd_cdc_if.h"
#include "amg8833.h"

float pixels[64];

char txBuf[512];

extern USBD_HandleTypeDef hUsbDeviceFS;
extern uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;

#define AMG8833_ADDR (0x69 << 1)
uint8_t amg_data[128];
char txBuffer[1024];

void AMG_ReadTempGrid() {
    HAL_I2C_Mem_Read(&hi2c1, AMG8833_ADDR, 0x80, I2C_MEMADD_SIZE_8BIT, amg_data, 128, HAL_MAX_DELAY);
}

float ConvertRawToTemp(uint8_t low, uint8_t high) {
    int16_t raw = (high << 8) | low;
    if (raw & 0x800) raw |= 0xF000; // sign extension
    return raw * 0.25;
}

void SetServoAngle(TIM_HandleTypeDef* htim, uint32_t channel, float angle) {
    uint32_t pulse = (uint32_t)(500 + (angle / 180.0f) * 2000); // 0-180° → 0.5ms–2.5ms pulse
    __HAL_TIM_SET_COMPARE(htim, channel, pulse);
}
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
    // Optional: blink LED or print debug info
  }
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_USB_DEVICE_Init();
    MX_TIM1_Init();
    MX_TIM2_Init();

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // PA8
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); // PA9

    HAL_Delay(2000);

    while (1)
    {
        if (AMG8833_ReadPixels(pixels))
        {
            int len = 0;
            len += sprintf(txBuf, "TEMP:");
            for (int i = 0; i < 64; i++)
            {
                len += sprintf(&txBuf[len], "%.2f,", pixels[i]);
            }
            txBuf[len - 1] = '\n';  // Replace last comma with newline

            CDC_Transmit_FS((uint8_t*)txBuf, strlen(txBuf));
            HAL_Delay(300); // limit data rate
        }
    }

    while (1) {
        AMG_ReadTempGrid();
        strcpy(txBuffer, "GRID:");

        int maxIdx = 0;
        float maxTemp = -100.0f;

        for (int i = 0; i < 64; ++i) {
            float temp = ConvertRawToTemp(amg_data[i * 2], amg_data[i * 2 + 1]);
            if (temp > maxTemp) {
                maxTemp = temp;
                maxIdx = i;
            }
            char tempStr[10];
            sprintf(tempStr, "%.2f", temp);
            strcat(txBuffer, tempStr);
            if (i != 63) strcat(txBuffer, ",");
        }

        // Calculate X,Y position from index
        int row = maxIdx / 8; // 0-7
        int col = maxIdx % 8; // 0-7

        // Map to servo angles (example: 0-180 degrees)
        float panAngle = (col / 7.0f) * 180.0f;
        float tiltAngle = (row / 7.0f) * 180.0f;

        SetServoAngle(&htim1, TIM_CHANNEL_1, panAngle);
        SetServoAngle(&htim2, TIM_CHANNEL_1, tiltAngle);

        strcat(txBuffer, "\r\n");
        CDC_Transmit_FS((uint8_t*)txBuffer, strlen(txBuffer));

        HAL_Delay(500);
    }
}
