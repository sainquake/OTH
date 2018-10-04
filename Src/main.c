/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "OT.h"
#include "DS18B20.h"
#include "RPi.h"
#include "sim800l.h"
#include "ADC.h"
#include <string.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

CAN_HandleTypeDef hcan;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart3_tx;
DMA_HandleTypeDef hdma_usart3_rx;

/* USER CODE BEGIN PV */
//#define NO_RPI
#define NO_GSM
//#define OT_ONLY
uint32_t micros;

//uint8_t sim_tx[10];
//uint8_t sim_rx[10];
/* Private variables ---------------------------------------------------------*/

/*int quality = 0;
 const char *a[10] = {
 "AT",
 "AT+CSQ",
 "ATI",
 "AT+CBC"
 };*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_CAN_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM2_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
/*if(GPIO_Pin== GPIO_PIN_15 && ext){
 //HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
 ot.rx_ext.raw = (ot.rx_ext.raw << 1) | ~HAL_GPIO_ReadPin(OT_TXI_GPIO_Port,OT_TXI_Pin);
 }*/
//}
/*void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
 {
 if (hspi->Instance==SPI2){
 //HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);

 //RPi_SPI.rxed =1;

 //HAL_SPI_
 HAL_SPI_Receive_IT(&hspi2, RPi_SPI.rx_buff, 3);
 //HAL_SPI_Transmit_IT(&hspi2, &RPi_SPI.tx_buff,  2);
 }
 }

 void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
 {
 if (hspi->Instance==SPI2){
 HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);

 //RPi_SPI.rxed =1;

 //HAL_SPI_
 HAL_SPI_Receive_IT(&hspi2, RPi_SPI.rx_buff, 3);

 }
 }*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM4) //check if the interrupt comes from TIM4
	{
		micros++;
	}
	if (htim->Instance == TIM2) //check if the interrupt comes from TIM1
	{
		rised = true;
		//stateChanged=true;
		//readResponseInProgress = false;
	}
}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2) //check if the interrupt comes from TIM1
	{
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
			risedCount = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); //TIM2->CCR1;
			//HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
			rised = true;
			//stateChanged = true;
			ttime[swIndex] = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			rf[swIndex] = 1;
			//arrF[swIndex] = 0;
		}
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
			//stateChanged = true;
			ttime[swIndex] = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
			rf[swIndex] = 0;
			//arrR[swIndex] = 0;
		}
		swIndex++;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART3)
		RPiRXRoute();
	if (huart->Instance == USART1)
		checkUpdate();
}
//void HAL_DMA

void HAL_SYSTICK_Callback(void) {

	//hdma_usart1_rx.State==HAL_DMA_STATE_READY;
	//int index=strstr(gprs.RX,gprs.TX)-gprs.RX;
	/*char* str2;
	 strcpy (str2,gprs.TX);
	 if(strpos(gprs.RX,str2,0)>=0)
	 HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);*/

//	if(strlen(gprs.RX)>0){
//		char * found = strstr(gprs.RX,"OK"); /* should return 0x18 */
//		if (found != NULL)                     /* strstr returns NULL if item not found */
//		{
//		  int index = found - gprs.RX;          /* index is 8 */
//											   /* source[8] gets you "i" */
//		  HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
//		  gprs.waitForResponse = false;
//
//		  //gprs.RX = "";
//		  gprs.index++;
//		  if(gprs.index>5)
//			  gprs.index=0;
//
//		  if(strpos(gprs.RX,"+COPS?",0)>0){
//			  HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
//			  //printf("+CSQ");
//			  //char *token = strtok(gprs.RX, "+CSQ: ");
//			 // quality = strtol(gprs.RX, ',', 10);
//		  }
//		  gprs.RX = "";
//		  //strcpy(gprs.TX,a[gprs.index]);
//		  switch(gprs.index){
//		  case 0:
//			  gprs.TX = "AT\r\n";
//			  break;
//		  case 1:
//			  gprs.TX = "AT+CFUN=1\r\n";
//			  break;
//		  case 2:
//			  gprs.TX = "AT+CSQ\r\n";
//			  break;
//		  case 3:
//			  gprs.TX = "AT+CREG=1\r\n";
//			  break;
//		  case 4:
//			  gprs.TX = "AT+CREG?\r\n";
//			  break;
//		  case 5:
//			  gprs.TX = "AT+COPS?\r\n";
//			  break;
//		  }
//		  gprs.transmitRequered = true;
//		}
//	}
	//strstr( gprs.RX, "in" );
	//if(index>=0)
#ifndef NO_GSM
	checkAT();
#endif
	if (HAL_GetTick() % 5000 == 0)
		OWTick();
	if ( (HAL_GetTick() % 300 == 0) && ot.complete && ot.granted) {
		ot.complete = false;
		ot.busy = false;
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	//uint8_t rx[16];
	//uint8_t i = 0;
	//uint16_t tt=0;
	//int Temp;
	//uint32_t re;
	//int index;
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_ADC1_Init();
  MX_CAN_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM4_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

	micros = 0;

	//HAL_TIM_Base_Stop_IT(&htim4);

	initADC();
#ifndef NO_RPI
	RPiInit();
#endif
	OWInit();
#ifndef NO_GSM
	initAT();
#endif

	initOT();
	activateBoiler();

	//HAL_SPI_Receive_IT(&hspi2, RPi_SPI.rx_buff, 3);

//  HAL_UART_Receive_IT(&huart1,&gprs.rx_buff,1);
	/* gprs.RX = "";
	 gprs.transmitRequered = false;
	 gprs.waitForResponse = false;*/
	/*RPi_SPI.rxed = 0;

	 RPi_SPI.tx_buff[0] = 0x0A;b
	 RPi_SPI.tx_buff[1] = 0x55;
	 RPi_SPI.tx_buff[2] = 0x0F;*/
	//HAL_SPI_
	//temp.TransmitReceive = true;
	//if( temp.TransmitReceive ){
	/* temp.busy = true;
	 OWTransmit();//HAL_UART_Transmit(&huart2,&convert_T,16,5000);
	 temp.TransmitReceive = false;
	 // }else{
	 HAL_Delay(200);

	 OWReceive();*/

	//temp.TransmitReceive = true;
	//}
	//OpenTherm init
	//FLASH MEMORY Write/Read
	/* HAL_FLASH_Unlock();
	 HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,0x08010000,0xAAAAAAAA);
	 HAL_FLASH_Lock();*/
	//uint32_t counter2 = *(__IO uint32_t *) 0x08010004;
	//counter2;
	HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);

	/*frame.raw[0] = 2;
	 frame.raw[2] = 0xAA;
	 frame.frame.data[0];
	 frame.frame.address;*/
//  gprs.index=0;
//  gprs.readIndex=0;
//
//  gprs.TX = "AT\r\n";
//  gprs.transmitRequered = true;
//  gprs.waitForResponse = true;
	/*while(gprs.waitForResponse){

	 }

	 gprs.TX = "AT\r\n";
	 gprs.transmitRequered = true;
	 gprs.waitForResponse = true;
	 while(gprs.waitForResponse){

	 }*/
//  HAL_UART_Transmit(&huart1,gprs.TX,strlen(gprs.TX),5000);
//  HAL_Delay(3000);
	/*gprs.TX = "AT+CREG?\r";
	 HAL_UART_Transmit(&huart1,gprs.TX,strlen(gprs.TX),5000);
	 HAL_Delay(3000);*/

	/* gprs.TX = "AT+CMGF=1\r\n";
	 HAL_UART_Transmit(&huart1,gprs.TX,strlen(gprs.TX),5000);
	 HAL_Delay(3000);

	 gprs.TX = "AT+CMGS=\"+79063280423\"\r\n";
	 HAL_UART_Transmit(&huart1,gprs.TX,strlen(gprs.TX),5000);
	 HAL_Delay(1000);

	 gprs.TX = "msg\x1a";
	 HAL_UART_Transmit(&huart1,gprs.TX,strlen(gprs.TX),5000);
	 HAL_Delay(1000);*/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
#ifndef NO_GSM
		sendQueue();
#endif
		OWRoute();
#ifndef NO_RPI
		RPiRoute();
#endif
		routeADC();
		if(ot.granted)
			OTRoute();
		/*OWTransmit();//HAL_UART_Transmit(&huart2,&convert_T,16,5000);
		 temp.TransmitReceive = false;
		 // }else{
		 HAL_Delay(200);

		 OWReceive();*/

		/*if(gprs.transmitRequered){
		 //gprs.TX = "AT\r\n";

		 HAL_Delay(1000);
		 HAL_UART_Transmit(&huart1,gprs.TX,strlen(gprs.TX),5000);
		 gprs.transmitRequered = false;
		 gprs.waitForResponse = true;
		 }*/

		//HAL_Delay(1000);
		//RPi_SPI.tx_buff[2] = temp.raw;
		//RPi_SPI.tx_buff[0] = (uint8_t)(adc.rpi_3v3>>8);
		//RPi_SPI.tx_buff[1] = (uint8_t)adc.rpi_3v3;
		/*if(RPi_SPI.rxed == 1){
		 RPi_SPI.tx_buff[0] = 0x0A;
		 RPi_SPI.tx_buff[1] = 0x55;
		 RPi_SPI.tx_buff[2] = 0xF0;

		 //HAL_SPI_Receive_IT(&hspi2, , 3);
		 //RPi_SPI.rxed=0;

		 //HAL_SPI_Transmit_IT(&hspi2, &RPi_SPI.tx_buff,  3);

		 //HAL_SPI_Receive_IT(&hspi2, &RPi_SPI.rx_buff, 3);
		 //HAL_SPI_TransmitReceive_IT(&hspi2, &RPi_SPI.tx_buff,&RPi_SPI.rx_buff, 3);
		 }*/

		//parity = checkParity( ot.tx.raw ) == ot.tx.frame.PARITY;
		//if(parity)
		//HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
		// HAL_Delay(300);
		//HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
		//}
//	    OTCommon.index++;//}
//	    if(OTCommon.index>=OT_QUEUE_LENGTH)
//	    	OTCommon.index=0;
		//HAL_Delay(300);
		//if(htim4.==0)
		//HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
		//
		/*while(1){
		 for (int index = 0; index < 500; index++)
		 delayMicros(1000);
		 HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
		 for (int index = 0; index < 500; index++)
		 delayMicros(1000);
		 HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
		 }*/
	}
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Common config 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* CAN init function */
static void MX_CAN_Init(void)
{

  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 16;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SJW = CAN_SJW_1TQ;
  hcan.Init.BS1 = CAN_BS1_1TQ;
  hcan.Init.BS2 = CAN_BS2_1TQ;
  hcan.Init.TTCM = DISABLE;
  hcan.Init.ABOM = DISABLE;
  hcan.Init.AWUM = DISABLE;
  hcan.Init.NART = DISABLE;
  hcan.Init.RFLM = DISABLE;
  hcan.Init.TXFP = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* SPI2 init function */
static void MX_SPI2_Init(void)
{

  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_SLAVE;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_HARD_INPUT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM2 init function */
static void MX_TIM2_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_IC_InitTypeDef sConfigIC;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 64000;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 900;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM4 init function */
static void MX_TIM4_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 32;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65000;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART3 init function */
static void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OT_RXO_GPIO_Port, OT_RXO_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(NSS1_GPIO_Port, NSS1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_R_Pin */
  GPIO_InitStruct.Pin = LED_R_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_R_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : OT_RXO_Pin */
  GPIO_InitStruct.Pin = OT_RXO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OT_RXO_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : NSS1_Pin */
  GPIO_InitStruct.Pin = NSS1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(NSS1_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while (1) {
	}
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	 tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
