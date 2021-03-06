/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "SDcardFun.h"
#include "ExSuppFun.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */



#define I2C_mode
#define UART_mode

#define SD_mode
#define Led_mode

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

#define DELAY_100HZ	10
#define DELAY_1HZ	1000
#define I2C_RECEIVE_CNT	(uint16_t)380
#define I2C_RECEIVE_TMT	10

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

ETH_HandleTypeDef heth;

I2C_HandleTypeDef hi2c2;

SD_HandleTypeDef hsd1;
DMA_HandleTypeDef hdma_sdmmc1_rx;
DMA_HandleTypeDef hdma_sdmmc1_tx;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* USER CODE BEGIN PV */

#ifdef SD_mode
SDcardFile_HandleTypeDef sdcfhtd;
				  //01234567890123456789


#endif
				  //0123456789012345678
TCHAR FileName[] = "dtss/ss0000.txt";
uint8_t BufferTMP[10];

__IO uint8_t TargetI2Cdevice = 0xff;
__IO uint8_t I2Cflag = 0x00;
uint8_t ptI2Cbuffer2transmit[] = {0,2,0,0};
uint8_t ptI2Cbuffer4receive[I2C_RECEIVE_CNT] = {0};




//FATFS SDFatFs = {0};
//char SDPath[4];
//uint8_t workBuffer[_MAX_SS];
//FIL MyFile;

__IO uint8_t flgBtnPress = 0;
typedef enum{
	Mode_Start,
	Mode_Blinked
}BaseMode_TypeDef;
__IO uint8_t BaseCurMode = Mode_Start;

__IO uint8_t MaxCountDelayPerOneBlink = 100;
__IO uint8_t CountDPB = 0;
__IO uint32_t BaseDelay = DELAY_100HZ;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ETH_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_SDMMC1_SD_Init(void);
/* USER CODE BEGIN PFP */
#ifdef UART_mode
static void UartsendMessage(uint8_t * msg, uint16_t cnt);
#endif
#ifdef Led_mode
static void LedSignalOn(void);
#endif

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
#ifdef SD_mode
	UINT I2CgetDTcnt;
#endif

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
  MX_ETH_Init();
  MX_I2C2_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_SDMMC1_SD_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */


  /* здесь происходит опрос портов*/
#ifdef UART_mode
  ReportMsg((uint8_t*)"Hello, user!\n");
#endif

#ifdef I2C_mode
  uint16_t possibleIndex;
  for ( possibleIndex = 0x01; possibleIndex < 127; possibleIndex++)
  {
	  if(HAL_I2C_IsDeviceReady(&hi2c2,(possibleIndex<<1),1,100) == HAL_OK)
	  {
		  TargetI2Cdevice = possibleIndex;
//		  break;
	  }
	  else
		  __NOP();
  }
  if(TargetI2Cdevice != 0xff)
  {
	  UartsendMessage((uint8_t*)"Get trg\n", 8);


	  __NOP();
  }
#endif

#ifdef SD_mode
  if(SDcardSelfTest(&sdcfhtd) == SDcard_success)
  {

#ifdef Led_mode
	  LedSignalOn();
#endif
//	  SDcardOpenDir(&sdcfhtd, "data");
//	  SDcardOpenFile2write(&sdcfhtd, "data/newfile2.txt");

	  UINT getmsglen;
//	  SDcardWrite2file(&sdcfhtd, (uint8_t*)"hello", (UINT)5, &getmsglen);
//	  SDcardCloseFile(&sdcfhtd);
	  if(TargetI2Cdevice != 0xff)
	  {
		  //check file names
		  uint16_t iterator = 1;
		  SDcardOpenDir(&sdcfhtd, "dtss");
		  while(SDcardTryOpen(&sdcfhtd, FileName) == SDcard_success)
		  {
			  uint8_t order = Dec_Convert(&BufferTMP[0],(int)iterator++);
			  for(uint8_t i = 0; i < order; i++)	FileName[11 - order + i] = BufferTMP[10 - order + i];
		  }
//		  TCHAR trgpath_tmp[] = "dtss/ss0000.txt";
		  SDcardOpenFile2write(&sdcfhtd, FileName);
		  SDcardWrite2file(&sdcfhtd, (uint8_t*)"new session\n", (UINT)sizeof("new session\n"), &getmsglen);

		  uint8_t i = 0;
		  while(i++ < 1)
		  {
			  ptI2Cbuffer2transmit[3] = 4;
			  HAL_I2C_Master_Transmit(&hi2c2, (TargetI2Cdevice<<1), ptI2Cbuffer2transmit, 4, 10);
			  HAL_Delay(100);
			  uint8_t j = 0;
			  while(j++ < 10)
			  {
					  HAL_I2C_Master_Receive(&hi2c2, (TargetI2Cdevice<<1), ptI2Cbuffer4receive, I2C_RECEIVE_CNT, I2C_RECEIVE_TMT);
					HAL_Delay(10);
			  }
			  HAL_Delay(100);
			  ptI2Cbuffer2transmit[3] = 0;
			  HAL_I2C_Master_Transmit(&hi2c2, (TargetI2Cdevice<<1), ptI2Cbuffer2transmit, 4, 10);
		  }

		  SDcardCloseFile(&sdcfhtd);
		  LedSignalOn();
	  }
	  __NOP();
  }
  __NOP();
#endif

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
#ifdef I2C_mode
  if(TargetI2Cdevice != 0xff)
  {
	  ptI2Cbuffer2transmit[3] = 0x01;
	  //HAL_I2C_Master_Receive(&hi2c2, (TargetI2Cdevice<<1), ptI2Cbuffer4receive, I2C_RECEIVE_CNT, I2C_RECEIVE_TMT);
	  if(HAL_I2C_Master_Transmit(&hi2c2, (TargetI2Cdevice<<1), ptI2Cbuffer2transmit, 4, 100) == HAL_OK)
	  {
		  I2Cflag = 0x01;
		  UartsendMessage((uint8_t*)"Set str\n", 8);
	  }
  }
#endif
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
#ifdef I2C_mode
	  if(I2Cflag)
	  {
		  uint32_t time = HAL_GetTick();
		  HAL_I2C_Master_Receive(&hi2c2, (TargetI2Cdevice<<1), ptI2Cbuffer4receive, I2C_RECEIVE_CNT, 10);
		  UartsendMessage(ptI2Cbuffer4receive, I2C_RECEIVE_CNT);
		  UartsendMessage((uint8_t*)"\n", 1);
		  time = HAL_GetTick() - time;
		  HAL_Delay(1000 - time);
	  }
#endif
	  if(flgBtnPress)
	  {
		  switch(BaseCurMode)
		  {
		  case Mode_Start:
			  BaseCurMode = Mode_Blinked;
#ifdef SD_mode
			  if(SDcardIsFileOpened(&sdcfhtd) != SDcard_FileOpen2write)
				  SDcardOpenFile2write(&sdcfhtd, FileName);
#endif
			  break;
		  case Mode_Blinked:
			  BaseCurMode = Mode_Start;
#ifdef SD_mode
			  if(SDcardIsFileOpened(&sdcfhtd) == SDcard_FileOpen2write)
				  SDcardCloseFile(&sdcfhtd);
#endif
			  break;
		  }
		  flgBtnPress = 0;
	  }
	  else
	  {
		  switch(BaseCurMode)
		  {
		  case Mode_Start:
			  break;
		  case Mode_Blinked:
#ifdef I2C_mode
			  HAL_I2C_Master_Receive(&hi2c2, (TargetI2Cdevice<<1), ptI2Cbuffer4receive, I2C_RECEIVE_CNT, I2C_RECEIVE_TMT);
#endif
#ifdef SD_mode
			  SDcardWrite2file(&sdcfhtd, ptI2Cbuffer4receive, I2C_RECEIVE_CNT, &I2CgetDTcnt);
#endif
			  if(CountDPB == MaxCountDelayPerOneBlink)
			  {
				  LedTurn(0);
				  CountDPB = 0;
			  }
			  else if(CountDPB == (MaxCountDelayPerOneBlink - 1))
			  {
				  LedTurn(1);
				  CountDPB++;
			  }
			  else
			  {
				  CountDPB++;
			  }
			  break;
		  }
	  }
	  HAL_Delay(BaseDelay);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure LSE Drive Capability 
  */
  HAL_PWR_EnableBkUpAccess();
  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode 
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_USART3
                              |RCC_PERIPHCLK_I2C2|RCC_PERIPHCLK_SDMMC1
                              |RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
  PeriphClkInitStruct.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_CLK48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ETH Initialization Function
  * @param None
  * @retval None
  */
static void MX_ETH_Init(void)
{

  /* USER CODE BEGIN ETH_Init 0 */

  /* USER CODE END ETH_Init 0 */

   uint8_t MACAddr[6] ;

  /* USER CODE BEGIN ETH_Init 1 */

  /* USER CODE END ETH_Init 1 */
  heth.Instance = ETH;
  heth.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
  heth.Init.PhyAddress = LAN8742A_PHY_ADDRESS;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE1;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x00;
  MACAddr[5] = 0x00;
  heth.Init.MACAddr = &MACAddr[0];
  heth.Init.RxMode = ETH_RXPOLLING_MODE;
  heth.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
  heth.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;

  /* USER CODE BEGIN MACADDRESS */
    
  /* USER CODE END MACADDRESS */

  if (HAL_ETH_Init(&heth) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ETH_Init 2 */

  /* USER CODE END ETH_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x6000030D;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief SDMMC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDMMC1_SD_Init(void)
{

  /* USER CODE BEGIN SDMMC1_Init 0 */

  /* USER CODE END SDMMC1_Init 0 */

  /* USER CODE BEGIN SDMMC1_Init 1 */

  /* USER CODE END SDMMC1_Init 1 */
  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockBypass = SDMMC_CLOCK_BYPASS_DISABLE;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_1B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 2;
  /* USER CODE BEGIN SDMMC1_Init 2 */

  HAL_SD_MspInit(&hsd1);

  //BSP_SD_Init();

  HAL_Delay(200);

  __NOP();


  /* USER CODE END SDMMC1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

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
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 6;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
  /* DMA2_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
#ifdef UART_mode
static void UartsendMessage(uint8_t * msg, uint16_t cnt)
{
	HAL_UART_Transmit(&huart2, msg, cnt, 10);
}
#endif
#ifdef Led_mode
static void LedSignalOn(void)
{
	HAL_GPIO_WritePin(GPIOB, LD1_Pin,GPIO_PIN_SET);
}
#endif

void BlinkLed(uint32_t delay)
{
	HAL_GPIO_TogglePin(GPIOB, LD2_Pin);
	HAL_Delay(delay);
	HAL_GPIO_TogglePin(GPIOB, LD2_Pin);
}
void LedTurn(uint8_t mode)
{
	if(mode)	HAL_GPIO_WritePin(GPIOB, 	LD2_Pin,	GPIO_PIN_SET);
	else		HAL_GPIO_WritePin(GPIOB, 	LD2_Pin,	GPIO_PIN_RESET);
}
void BtnPress_Callback(void)
{
	flgBtnPress = 1;
}
void ReportMsg(uint8_t * msg)
{
	uint16_t count = 0;
	while(*msg++ != '\0') count++;
#ifdef UART_mode
	HAL_UART_Transmit(&huart2, msg, count, 10);
#endif

#ifdef SD_mode
	if(sdcfhtd.fileIsOpened == SDcard_FileOpen2write)
	{
		UINT getmsglen = 0;
		SDcardWrite2file(&sdcfhtd, msg, count, &getmsglen);
		if(count != getmsglen)
		{
			//some error
		}
	}
#endif
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
