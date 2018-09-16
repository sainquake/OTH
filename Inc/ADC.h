/*
 * ADC.h
 *
 *  Created on: 14 сент. 2018 г.
 *      Author: Sainquake
 */

#ifndef ADC_H_
#define ADC_H_

#include "stm32f1xx_hal.h"

extern ADC_HandleTypeDef hadc1;

#define V4_SENSE		ADC_CHANNEL_4
#define RPI_3V3_SENSE 	ADC_CHANNEL_5
#define USB_5V_SENSE	ADC_CHANNEL_6
#define VIN_SENSE		ADC_CHANNEL_7
#define A14				ADC_CHANNEL_14
#define A15				ADC_CHANNEL_15

ADC_ChannelConfTypeDef adcChannel;

typedef struct{
	int rpi_3v3;
	int in[16];
	float v[16];
}ADC_Struct;
ADC_Struct adc;

//ADC
void initADC(void);
void StartAnalogADC1( uint32_t ch );
int GetAnalogADC1( void );
int ReadAnalogADC1( uint32_t ch );


void initADC(void){
	adcChannel.Channel = ADC_CHANNEL_0;//B
	adcChannel.Rank = 1;
	adcChannel.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;



	  //ADC1
	  //HAL_ADC_MspInit(&hadc1);
	  HAL_ADC_Start(&hadc1);
	  //adc2
	  //HAL_ADC_MspInit(&hadc2);
	  //HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
}
//ADC
void StartAnalogADC1( uint32_t ch ){
	adcChannel.Channel = ch;//B
	adcChannel.Rank = 1;
	adcChannel.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;

	HAL_ADC_ConfigChannel(&hadc1, &adcChannel);
	HAL_ADC_Start(&hadc1);
}
int GetAnalogADC1( void ){
	while( __HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_EOC)==0 ){}
	return HAL_ADC_GetValue(&hadc1);
}
int ReadAnalogADC1( uint32_t ch ){
	adcChannel.Channel = ch;//B
	adcChannel.Rank = 1;
	adcChannel.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;

	HAL_ADC_ConfigChannel(&hadc1, &adcChannel);//A4 / B
	HAL_ADC_Start(&hadc1);
	while( __HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_EOC)==0 ){}
	return HAL_ADC_GetValue(&hadc1);
}

#endif /* ADC_H_ */
