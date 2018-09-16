/*
 * DS18B20.h
 *
 *  Created on: 19 рту. 2018 у.
 *      Author: Sainquake
 */

#ifndef DS18B20_H_
#define DS18B20_H_

#include "stm32f1xx_hal.h"

extern UART_HandleTypeDef huart2;

#define OW_Period 200

#define OW_0    0x00
#define OW_1    0xff
#define OW_R    0xff

const uint8_t convert_T[] = {
                OW_0, OW_0, OW_1, OW_1, OW_0, OW_0, OW_1, OW_1, // 0xcc SKIP ROM
                OW_0, OW_0, OW_1, OW_0, OW_0, OW_0, OW_1, OW_0  // 0x44 CONVERT
};

const uint8_t read_scratch[] = {
                OW_0, OW_0, OW_1, OW_1, OW_0, OW_0, OW_1, OW_1, // 0xcc SKIP ROM
                OW_0, OW_1, OW_1, OW_1, OW_1, OW_1, OW_0, OW_1, // 0xbe READ SCRATCH
                OW_R, OW_R, OW_R, OW_R, OW_R, OW_R, OW_R, OW_R,
                OW_R, OW_R, OW_R, OW_R, OW_R, OW_R, OW_R, OW_R
};

uint8_t scratch[sizeof(read_scratch)];

//DS18B20
typedef struct{
	uint16_t raw;
	bool reset;
	float out;
	bool TransmitReceive;
    bool busy;
    bool transmitInProgress;
    bool receiveInProgress;
}TEMP_Struct;
TEMP_Struct temp;

// one wire
void OWInit(void);
bool OWReset(void);
void OWTransmit(void);
void OWReceive(void);

//1 wire
void OWInit(void){
temp.TransmitReceive = false;
temp.busy = true;
temp.transmitInProgress = false;
temp.receiveInProgress = false;
//OWTransmit();
//temp.TransmitReceive = false;
//HAL_Delay(200);
//OWReceive();
}
void OWTick(void){
    if(!temp.transmitInProgress && !temp.receiveInProgress){
        temp.TransmitReceive = ! temp.TransmitReceive;
        temp.busy = false;
    }
}
void OWRoute(void){
    if(!temp.busy){
        temp.busy = true;
        if(temp.TransmitReceive)
            OWTransmit();
        else
            OWReceive();
    }
}
bool OWReset(void){
	uint8_t tx = 0xf0;
	uint8_t rx = 0;

	huart2.Instance = USART2;
	huart2.Init.BaudRate = 9600;
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

	huart2.Instance->DR = tx;
	HAL_Delay(10);
	rx = huart2.Instance->DR;


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
	if(rx==0xf0)
		return false;

	return true;
}

void OWTransmit(void){
    temp.transmitInProgress = true;
    //temp.busy = true;
	temp.reset = OWReset();
	if(temp.reset) HAL_UART_Transmit(&huart2,&convert_T,16,5000);
    //temp.TransmitReceive = false;
    temp.transmitInProgress = false;
}
void OWReceive(void){
    temp.receiveInProgress = true;
    //temp.busy = true;
	uint8_t rx=0;
	uint8_t i=0;

	temp.reset = OWReset();
	if( temp.reset ){
		temp.raw = 0;
				HAL_UART_Transmit(&huart2,&read_scratch,16,5000);

				for(i=0;i<16;i++){
					huart2.Instance->DR = 0xff;
					HAL_Delay(10);
					rx = huart2.Instance->DR;

					if (rx == 0xff) {
						temp.raw = (temp.raw>>1) | 0x8000;
					} else {
						temp.raw = temp.raw>>1;
					}
				}
				//HAL_UART_Receive(&huart2,&scratch,16,2000);
				//HAL_UART_*/
				HAL_Delay(1);
				temp.out = (temp.out + (int)(temp.raw>>4) )/2;
			}
    //temp.TransmitReceive = true;
    temp.receiveInProgress = false;
}




#endif /* DS18B20_H_ */
