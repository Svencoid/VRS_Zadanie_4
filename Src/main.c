/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define STATUS_SEND_PERIOD_MS   2000U
#define FRAME_MAX_LEN           35U
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint32_t elapsed_ms = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void proccesDmaData(const uint8_t* data, uint16_t len);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  USART2_RegisterCallback(proccesDmaData);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    USART2_CheckDmaReception();

    elapsed_ms += 10;
    if (elapsed_ms >= STATUS_SEND_PERIOD_MS)
    {
        char statusMsg[48];
        uint16_t used = USART2_GetRxBufferUsage();
        int len = snprintf(statusMsg, sizeof(statusMsg),
                            "RX buffer: %u/%u B\r\n", used, DMA_USART2_BUFFER_SIZE);
        USART2_PutBuffer((uint8_t*)statusMsg, (uint8_t)len);
        elapsed_ms = 0;
    }

    LL_mDelay(10);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
  {
    Error_Handler();
  }
  LL_RCC_HSI_Enable();

  while(LL_RCC_HSI_IsReady() != 1)
  {
  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {
  }
  LL_Init1msTick(8000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(8000000);
}

/* USER CODE BEGIN 4 */

static uint8_t  frame_active = 0;
static uint8_t  frame_char_count = 0;
static uint16_t lower_count = 0;
static uint16_t upper_count = 0;


void proccesDmaData(const uint8_t* data, uint16_t len)
{
	for (uint16_t i = 0; i < len; i++)
	{
		uint8_t c = data[i];

		if (!frame_active)
		{
			if (c == '#')
			{
				frame_active = 1;
				frame_char_count = 0;
				lower_count = 0;
				upper_count = 0;
			}
			/* znaky mimo rámca sa ignorujú */
			continue;
		}

		if (c == '$')
		{
			char msg[40];
			int msgLen = snprintf(msg, sizeof(msg),
			                       "malych: %u, velkych: %u\r\n",
			                       lower_count, upper_count);
			USART2_PutBuffer((uint8_t*)msg, (uint8_t)msgLen);

			frame_active = 0;
			continue;
		}

		if (c >= 'a' && c <= 'z')      lower_count++;
		else if (c >= 'A' && c <= 'Z') upper_count++;

		frame_char_count++;
		if (frame_char_count >= FRAME_MAX_LEN)
		{
			/* ukončovací znak neprišiel včas - zahoď a čakaj na nový '#' */
			frame_active = 0;
		}
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
void assert_failed(char *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
