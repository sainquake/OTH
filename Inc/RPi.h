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

#define RPI_BUFFER_SIZE 5
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
#define RPi_OT_HEADER_UART_ADDRESS		32
//#define RPi

#define RPi_SET_TEMP_UART_ADDRESS       10
#define RPi_GET_HW_TEMP_UART_ADDRESS    11

#define RPi_RESET_MCU					254
#define RPi_CRC_ERROR					255
//toRPIlib
typedef struct {
	uint16_t address :16;
	uint16_t data :16;
	uint8_t crc :8;
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
	bool crcChecked;

	int len;
	char* pointer;

	uint16_t address;
	uint8_t data;
	//uint8_t rx_buff[RPI_BUFFER_SIZE];
	uint8_t rx_buff[RPI_RX_BUFFER_SIZE];
	uint8_t tx_buff[RPI_BUFFER_SIZE];
} UART_Struct;
UART_Struct RPi_UART;

void RPiInit(void);
void RPiRXRoute(void);
void RPiRoute(void);
void makeResponse(void);

void RPiInit(void) {
	//HAL_UART_Receive_DMA(&huart3, RPi_UART.rx_buff, RPI_BUFFER_SIZE);
	HAL_UART_Receive_DMA(&huart3, RPi_UART.rx_buff, RPI_RX_BUFFER_SIZE);
	RPi_UART.transmitRequered = false;
	RPi_UART.len = -1;
	RPi_UART.crcChecked = false;
	return;
}
void RPiRoute(void) {
	if (!RPi_UART.transmitRequered) {
		for (int i = 0; i < RPI_RX_BUFFER_SIZE; i++) {
			char crc_ = 0;
			for (int j = 0; j < 4; j++) {
				crc_ += RPi_UART.rx_buff[
						((i + j) > RPI_RX_BUFFER_SIZE - 1) ?
								(i + j - RPI_RX_BUFFER_SIZE) : (i + j)];
			}
			if (crc_
					== RPi_UART.rx_buff[
							((i + 4) > RPI_RX_BUFFER_SIZE - 1) ?
									(i + 4 - RPI_RX_BUFFER_SIZE) : (i + 4)]
					&& RPi_UART.rx_buff[i] != 0) {
				for (int k = 0; k < 5; k++) {
					rpiframe.raw[k] = RPi_UART.rx_buff[
							((i + k) > RPI_RX_BUFFER_SIZE - 1) ?
									(i + k - RPI_RX_BUFFER_SIZE) : (i + k)];
					RPi_UART.rx_buff[
							((i + k) > RPI_RX_BUFFER_SIZE - 1) ?
									(i + k - RPI_RX_BUFFER_SIZE) : (i + k)] = 0;
				}
				RPi_UART.crcChecked = true;
			}
		}
	}
	if (RPi_UART.crcChecked)
		makeResponse();
	if (RPi_UART.transmitRequered) {
		if (RPi_UART.len < 0) {
			HAL_UART_Transmit_DMA(&huart3, RPi_UART.tx_buff, RPI_BUFFER_SIZE);
		} else {
			HAL_UART_Transmit_DMA(&huart3, RPi_UART.pointer,
					strlen(RPi_UART.pointer));
			RPi_UART.len = -1;
		}
		//HAL_UART_Transmit(&huart3, RPi_UART.tx_buff, RPI_BUFFER_SIZE, 1000);
		RPi_UART.transmitRequered = false;
	}
}
//toRPIlib
void makeResponse(void) {

	int16_t tmp;
	uint32_t HWID = *(__IO uint32_t *) 0x08010004;
	RPi_UART.crcChecked = false;
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
		ot.granted = false;
		tmp = (rpiframe.frame.address >> 8) & 0x7F;
		if((rpiframe.frame.address >> 15) & 1){
			ot.special_tx.frame.DATA_VALUE = rpiframe.frame.data;
			ot.special_tx.frame.DATA_ID = tmp;
			ot.special_tx.frame.MSG_TYPE = OT_MSG_TYPE_M_WRITE_DATA;
			ot.special_tx.frame.PARITY = parityBit(ot.special_tx.raw);
			ot.specialRequest = true;
			ot.specialRequestComplete = false;
		}
		rpiframe.frame.data = ot.dataRegisters[tmp] & 0xFFFF;//OTDR.ID3.SlaveMemberID;
		RPi_UART.transmitRequered = true;
		ot.granted = true;
		break;
	case RPi_OT_HEADER_UART_ADDRESS:
		ot.granted = false;
		tmp = (rpiframe.frame.address >> 8) & 0x7F;
		rpiframe.frame.data = (ot.dataRegisters[tmp] >> 16) & 0xFFFF;//OTDR.ID3.SlaveMemberID;
		RPi_UART.transmitRequered = true;
		ot.granted = true;
		break;
	case 4:

		HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
		//ot.RPiResponseHI = frame.frame.data;
		break;
	case RPi_SIM800L_UART_ADDRESS: {
		uint16_t d = rpiframe.frame.data;
		rpiframe.frame.data = gprs.at[subaddress].response;
		if (subaddress == AT_CSQ)
			rpiframe.frame.data = gprs.quality;
		if (subaddress == AT_CBC)
			rpiframe.frame.data = gprs.voltage;
		if (subaddress == AT_CSPN) {
			RPi_UART.pointer = gprs.operator;
			RPi_UART.len = 1;
		}
		if (subaddress == AT_CUSD) {
			if (d == 1) {
				rpiframe.frame.data = 1;
				gprs.balanceRequered = true;
				gprs.balanceReceived = false;
			} else if (d == 2) {
				rpiframe.frame.data = gprs.balanceReceived;
			} else {
				rpiframe.frame.data = round(gprs.balance * 256.0);
			}
			//rpiframe.frame.data = 1;
			//rpiframe.frame.data = d;
		}
		if (subaddress == AT_CPMS) {
			rpiframe.frame.data = gprs.smsCount;
		}
		if (subaddress == AT_CMGR) {
			if (d != 0) {
				gprs.smsToRead = d;
			} else {
				RPi_UART.pointer = gprs.at[AT_CMGR].args;
				RPi_UART.len = 1;
			}
		}
		RPi_UART.transmitRequered = true;
		/*if (subaddress == 0){
		 rpiframe.frame.data = AT.at.response;
		 }*/
		//HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
		//ot.RPiResponseLO= frame.frame.data;
	}
		break;
	case RPi_ADC_UART_ADDRESS:
		tmp = round(adc.v[rpiframe.frame.address >> 8 & 0x0f] * 256.0);
		rpiframe.frame.data = tmp;
		RPi_UART.transmitRequered = true;
		//ot.RPiRequestHI = frame.frame.data;
		break;
	case RPi_OT_STATUS_UART_ADDRESS:
		ot.granted = false;
		if (subaddress == 0)
			rpiframe.frame.data = ot.timeout;
		if (subaddress == 1)
			rpiframe.frame.data = ot.index;
		if (subaddress == 2)
			rpiframe.frame.data = ot.busy;
		if (subaddress == 3)
			rpiframe.frame.data = ot.complete;
		if (subaddress == 4)
			rpiframe.frame.data = ot.frameSendedAndStartWaitingACK;
		if (subaddress == 5)
			rpiframe.frame.data = ot.readingResponse;
		if (subaddress == 6) {
			rpiframe.frame.data = (ot.timeout)
					+ (ot.busy << 1)
					+ (ot.complete << 2)
					+ (ot.frameSendedAndStartWaitingACK << 3)
					+ (ot.readingResponse << 4)
					+ (ot.specialRequest <<5)
					+ (ot.specialRequestComplete <<6)
					+ ((ot.noResponseCount>5)<<7)
					+ (ot.index << 8);
		}
		RPi_UART.transmitRequered = true;
		ot.granted = true;
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
	case RPi_RESET_MCU:
		//NVIC_SystemReset();
		break;
	}

	/*if (RPi_UART.rx_buff[0] == 'H' && RPi_UART.rx_buff[1] == 'e'
	 && RPi_UART.rx_buff[2] == 'l') {
	 HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
	 }
	 if (RPi_UART.rx_buff[0] == 't' && RPi_UART.rx_buff[1] == 'm'
	 && RPi_UART.rx_buff[2] == 'p') {
	 HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
	 //RPi_UART.tx_buff
	 }*/

	for (int i = 0; i < RPI_BUFFER_SIZE; i++)
		RPi_UART.tx_buff[i] = rpiframe.raw[i];
}
void RPiRXRoute(void) {
	HAL_UART_Receive_DMA(&huart3, RPi_UART.rx_buff, RPI_RX_BUFFER_SIZE);

	/*for (int i = 0; i < RPI_BUFFER_SIZE; i++)
	 rpiframe.raw[i] = RPi_UART.rx_buff[i];
	 if (rpiframe.frame.crc
	 == rpiframe.raw[0] + rpiframe.raw[1] + rpiframe.raw[2]
	 + rpiframe.raw[3]) {
	 RPi_UART.crcChecked = true;
	 } else {
	 rpiframe.frame.address = RPi_CRC_ERROR;
	 RPi_UART.transmitRequered = true;
	 for (int i = 0; i < RPI_BUFFER_SIZE; i++)
	 RPi_UART.tx_buff[i] = rpiframe.raw[i];
	 }

	 HAL_UART_Receive_DMA(&huart3, RPi_UART.rx_buff, RPI_BUFFER_SIZE);*/
}

#endif /* RPI_H_ */
