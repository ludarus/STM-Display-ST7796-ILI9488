/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "display.h"
#include "commands.h"
//testing includes
#include "File_038_ObjNum_037_48x76_08_31_23.h"
#include "File_033_ObjNum_032_48x76_10_27_23.h"
#include "File_047_ObjNum_046_72x83_08_30_23.h"
#include "File_066_ObjNum_109_48x32_08_30_23.h"
#include "File_059_ObjNum_094_240x128_10_27_23.h"
#include "File_013_ObjNum_012_48x76_10_27_23.h"
#include "File_051_ObjNum_055_72x73_10_17_23.h"
#include "File_015_ObjNum_014_48x76_10_27_23.h"
#include "File_010_ObjNum_009_48x76_10_27_23.h"
#include "File_021_ObjNum_020_24x83_11_28_23.h"
#include "File_064_ObjNum_099_24x25_10_28_23.h"
#include "File_037_ObjNum_036_48x76_08_31_23.h"
#include "File_026_ObjNum_025_24x83_11_28_23.h"
#include "File_052_ObjNum_057_72x12_08_30_23.h"
#include "File_039_ObjNum_038_48x76_08_31_23.h"
#include "File_077_ObjNum_147_240x128_10_27_23.h"
#include "File_070_ObjNum_129_48x32_10_27_23.h"
#include "File_009_ObjNum_008_48x76_10_27_23.h"
#include "File_078_ObjNum_148_240x128_1_27_23.h"
#include "File_040_ObjNum_039_48x76_08_31_23.h"
#include "File_035_ObjNum_034_48x76_08_31_23.h"
#include "File_031_ObjNum_030_32x10_10_27_23.h"
#include "File_016_ObjNum_015_48x76_10_27_23.h"
#include "File_007_ObjNum_006_240x128.h"
#include "File_044_ObjNum_043_72x83_08_30_23.h"
#include "File_055_ObjNum_090_240x128_10_27_23.h"
#include "File_011_ObjNum_010_48x76_10_27_23.h"
#include "File_065_ObjNum_104_8x12_11_28_23.h"
#include "File_048_ObjNum_052_72x73_10_27_23.h"
#include "File_020_ObjNum_019_24x83_11_28_23.h"
#include "File_025_ObjNum_024_24x83_11_28_23.h"
#include "File_072_ObjNum_135_240x128_08_31_23.h"
#include "File_079_ObjNum_149_240x128_10_27_23.h"
#include "File_024_ObjNum_023_24x83_11_28_23.h"
#include "File_017_ObjNum_016_48x76_10_27_23.h"
#include "File_019_ObjNum_018_24x83_11_28_23.h"
#include "File_079_ObjNum_149_480x320_6_17_26__002_.h"
#include "File_042_ObjNum_041_48x76_08_31_23.h"
#include "File_062_ObjNum_097_240x16_10_27_23.h"
#include "File_075_ObjNum_140_184x16_10_27_23.h"
#include "File_056_ObjNum_091_240x128_10_27_23.h"
#include "File_008_ObjNum_007_48x76_10_27_23.h"
#include "File_068_ObjNum_119_32x41_11_28_23.h"
#include "File_067_ObjNum_114_32x41_11_28_23.h"
#include "File_005_ObjNum_004_240x128_10_27_23.h"
#include "File_043_ObjNum_042_72x83_08_30_23.h"
#include "File_073_ObjNum_136_240x128_10_27_23.h"
#include "File_069_ObjNum_124_32x41_11_28_23.h"
#include "File_030_ObjNum_029_32x10_10_27_23.h"
#include "File_029_ObjNum_028_24x83_11_28_23.h"
#include "File_053_ObjNum_077_24x12_10_27_23.h"
#include "File_071_ObjNum_134_88x75_10_27_23.h"
#include "File_002_ObjNum_001_NEW.h"
#include "File_074_ObjNum_138_24x57_10_27_2023.h"
#include "File_012_ObjNum_011_48x76_10_27_23.h"
#include "File_003_ObjNum_002_FONT.h"
#include "File_054_ObjNum_087_24x102_10_27_23.h"
#include "File_041_ObjNum_040_48x76_08_31_23.h"
#include "File_006_ObjNum_005_240x128_10_27_23_C.h"
#include "File_023_ObjNum_022_24x83_11_28_23.h"
#include "File_045_ObjNum_044_72x83_08_30_23.h"
#include "File_049_ObjNum_053_72x73_10_27_23.h"
#include "File_018_ObjNum_017_48x76_10_27_23.h"
#include "File_014_ObjNum_013_48x76_10_27_23.h"
#include "File_046_ObjNum_045_72x83_08_30_23.h"
#include "File_063_ObjNum_098_88x13_08_30_23.h"
#include "File_076_ObjNum_143_184x16_10_27_23.h"
#include "File_050_ObjNum_054_72x73_10_27_23.h"
#include "File_022_ObjNum_021_24x83_11_28_23.h"
#include "File_027_ObjNum_026_24x83_11_28_23.h"
#include "File_034_ObjNum_033_48x76_10_27_23.h"
#include "File_028_ObjNum_027_24x83_11_28_23.h"
#include "File_057_ObjNum_092_240x128_10_27_23.h"
#include "File_036_ObjNum_035_48x76_08_31_23.h"
#include "File_058_ObjNum_093_240x128_10_27_23.h"
#include "File_032_ObjNum_031_48x76_10_27_23.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

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
	MX_SPI1_Init();

	/* USER CODE BEGIN 2 */
	// DMA serial command recieve until IDLE
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2, getCommandBuffer(), getCommandSize());

	// initializing display
	DISPLAY_INIT(&hspi1);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
	RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
	PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void) {

	/* USER CODE BEGIN SPI1_Init 0 */

	/* USER CODE END SPI1_Init 0 */

	/* USER CODE BEGIN SPI1_Init 1 */

	/* USER CODE END SPI1_Init 1 */
	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 7;
	hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
	if (HAL_SPI_Init(&hspi1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN SPI1_Init 2 */

	/* USER CODE END SPI1_Init 2 */

}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void) {

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart2) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Ch1_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Ch1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Ch1_IRQn);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(DISPLAY_DC_GPIO_Port, DISPLAY_DC_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, DISPLAY_RESET_Pin | DISPLAY_LED_Pin,
			GPIO_PIN_RESET);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : DISPLAY_DC_Pin */
	GPIO_InitStruct.Pin = DISPLAY_DC_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DISPLAY_DC_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : DISPLAY_CS_Pin */
	GPIO_InitStruct.Pin = DISPLAY_CS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DISPLAY_CS_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : DISPLAY_RESET_Pin DISPLAY_LED_Pin */
	GPIO_InitStruct.Pin = DISPLAY_RESET_Pin | DISPLAY_LED_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* USER CODE BEGIN MX_GPIO_Init_2 */

	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
