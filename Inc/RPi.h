/*
 * RPi.h
 *
 *  Created on: 12 сент. 2018 г.
 *      Author: k.burdinov
 */

#ifndef RPI_H_
#define RPI_H_

#include "ADC.h"
#include  "OT.h"
#include "sim800l.h"

#include "stm32f1xx_hal.h"

extern UART_HandleTypeDef huart3;

#define RPI_RX_BUFFER_SIZE 60

#define RPI_BUFFER_SIZE 4
//toRPIlib
#define RPi_ECHO_UART_ADDRESS           1
#define RPi_BLINK_UART_ADDRESS			2
#define RPi_OT_UART_ADDRESS				3
#define RPi_DS18B20_UART_ADDRESS		4
#define RPi_SIM800L_UART_ADDRESS		5
#define RPi_ADC_UART_ADDRESS			6
#define RPi_OT_STATUS_UART_ADDRESS		7
#define RPi_MEM_UART_ADDRESS			8
#define RPi_SMS_UART_ADDRESS			9
//#define RPi

#define RPi_SET_TEMP_UART_ADDRESS       10
#define RPi_GET_HW_TEMP_UART_ADDRESS    11
//toRPIlib
typedef struct {
	uint16_t address :16;
	uint16_t data :16;
} RPiFrameStruct;

union RPiFrameUnion {
	uint8_t raw[RPI_BUFFER_SIZE];
	RPiFrameStruct frame;
};
union RPiFrameUnion rpiframe = { 0 };

typedef struct {
	uint16_t address;
	uint8_t data;
	uint8_t rx_buff[10];
	uint8_t tx_buff[10];
	char rxed;
} SPI_Struct;
SPI_Struct RPi_SPI;

typedef struct {
	bool transmitRequered;

	int len;
	char* pointer;

	uint16_t address;
	uint8_t data;
	uint8_t rx_buff[RPI_BUFFER_SIZE];
	uint8_t tx_buff[RPI_BUFFER_SIZE];
} UART_Struct;
UART_Struct RPi_UART;

void RPiInit(void);
void RPiRXRoute(void);
void RPiRoute(void);

void RPiInit(void) {
	HAL_UART_Receive_IT(&huart3, RPi_UART.rx_buff, RPI_BUFFER_SIZE);
	RPi_UART.transmitRequered = false;
	RPi_UART.len = -1;
	return;
}
void RPiRoute(void) {
	if (RPi_UART.transmitRequered) {
		if(RPi_UART.len<0){
			HAL_UART_Transmit_DMA(&huart3, RPi_UART.tx_buff, RPI_BUFFER_SIZE);
		}else{
			HAL_UART_Transmit_DMA(&huart3, RPi_UART.pointer, strlen(RPi_UART.pointer));
			RPi_UART.len = -1;
		}
		//HAL_UART_Transmit(&huart3, RPi_UART.tx_buff, RPI_BUFFER_SIZE, 1000);
		RPi_UART.transmitRequered = false;
	}
}
//toRPIlib
void RPiRXRoute(void) {
	int16_t tmp;
	uint32_t HWID = *(__IO uint32_t *) 0x08010004;






	for (int i = 0; i < RPI_BUFFER_SIZE; i++)
		rpiframe.raw[i] = RPi_UART.rx_buff[i];

	uint8_t subaddress = rpiframe.frame.address >> 8 & 0xFF;
	switch (rpiframe.frame.address & 0xff) {
	case RPi_ECHO_UART_ADDRESS:
		HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
		//frame.frame.address = 1;
		//if(!frame.frame.read)
		//for (int i=0;i<4;i++)
		//  RPi_UART.tx_buff[i] = rpiframe.raw[i];
		//else
		//	for (int i=0;i<4;i++)
		//		RPi_UART.tx_buff[i] = 0;
		RPi_UART.transmitRequered = true;
		break;
	case 2:
		ot.RPiRequestHI = rpiframe.frame.data;
		break;
	case RPi_OT_UART_ADDRESS:
		tmp = rpiframe.frame.address >> 8 & 0xEF;
		rpiframe.frame.data = OTDR.ID3.SlaveMemberID;
		RPi_UART.transmitRequered = true;
		break;
	case 4:

		HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
		//ot.RPiResponseHI = frame.frame.data;
		break;
	case RPi_SIM800L_UART_ADDRESS:
		rpiframe.frame.data = gprs.at[subaddress].response;
		if(subaddress==AT_CSQ)
			rpiframe.frame.data = gprs.quality;
		if(subaddress==AT_CBC)
			rpiframe.frame.data = gprs.voltage;
		if(subaddress==AT_CSPN){
			RPi_UART.pointer = gprs.operator;
			RPi_UART.len=1;
		}
		RPi_UART.transmitRequered = true;
		/*if (subaddress == 0){
			rpiframe.frame.data = AT.at.response;
		}*/
		HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
		//ot.RPiResponseLO= frame.frame.data;
		break;
	case RPi_ADC_UART_ADDRESS:
		tmp = round(adc.v[rpiframe.frame.address >> 8 & 0x0f] * 256.0);
		rpiframe.frame.data = tmp;
		RPi_UART.transmitRequered = true;
		//ot.RPiRequestHI = frame.frame.data;
		break;
	case RPi_OT_STATUS_UART_ADDRESS:
		if (subaddress == 0)
			rpiframe.frame.data = ot.timeout;
		RPi_UART.transmitRequered = true;
		break;
	case RPi_MEM_UART_ADDRESS:

		rpiframe.frame.data = HWID & 0xFFFF;
		RPi_UART.transmitRequered = true;
		break;
		/////////----------------///
	case RPi_SET_TEMP_UART_ADDRESS:
		OTCommon.targetTemp = rpiframe.frame.data;
		//rpiframe.frame.data = 1;
		//for (int i=0;i<4;i++)
		// RPi_UART.tx_buff[i] = rpiframe.raw[i];

		RPi_UART.transmitRequered = true;
		break;
	case RPi_GET_HW_TEMP_UART_ADDRESS:
		tmp = round(temp.out * 256.0);
		rpiframe.frame.data = tmp;
		//for (int i=0;i<4;i++)
		// RPi_UART.tx_buff[i] = rpiframe.raw[i];
		RPi_UART.transmitRequered = true;
		break;
	}

	if (RPi_UART.rx_buff[0] == 'H' && RPi_UART.rx_buff[1] == 'e'
			&& RPi_UART.rx_buff[2] == 'l') {
		HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
	}
	if (RPi_UART.rx_buff[0] == 't' && RPi_UART.rx_buff[1] == 'm'
			&& RPi_UART.rx_buff[2] == 'p') {
		HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
		//RPi_UART.tx_buff
	}
	for (int i = 0; i < RPI_BUFFER_SIZE; i++)
		RPi_UART.tx_buff[i] = rpiframe.raw[i];
	HAL_UART_Receive_IT(&huart3, RPi_UART.rx_buff, RPI_BUFFER_SIZE);
}

#endif /* RPI_H_ */
