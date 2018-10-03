/*
 * OT.h
 *
 *  Created on: 19 рту. 2018 у.
 *      Author: Sainquake
 */

#ifndef OT_H_
#define OT_H_

//#include "main.h"
#include "stm32f1xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

#define OT_QUEUE_LENGTH 18
//OT defenititons
#define OT_MSG_TYPE_SHIFT				28
#define OT_MSG_TYPE_M_READ_DATA			0
#define OT_MSG_TYPE_M_WRITE_DATA		1
#define OT_MSG_TYPE_M_INVALID_DATA		2
#define OT_MSG_TYPE_M_RESERVED			3
#define OT_MSG_TYPE_S_READ_ACK			4
#define OT_MSG_TYPE_S_WRITE_ACK			5
#define OT_MSG_TYPE_S_DATA_INVALID		6
#define OT_MSG_TYPE_S_UNKNOWN_DATAID	7

#define OT_DATA_ID_SHIFT		16
#define OT_DATA_ID_Status						0	//R-
#define OT_DATA_ID_TSet							1	//-W
#define OT_DATA_ID_MConfig_ID					2	//-W
#define OT_DATA_ID_SConfig_ID					3	//R-
#define OT_DATA_ID_Command						4	//
#define OT_DATA_ID_ASFFlags_OEM_fault_code		5	//R-
#define OT_DATA_ID_RBPFlags						6	//R-
#define OT_DATA_ID_CoolingControl				7	//-W

#define OT_DATA_ID_FHBIndex_FHBValue			13	//R-
#define OT_DATA_ID_DHWFlowRate					19	//R-
#define OT_DATA_ID_TBoiler						25	//R-

extern TIM_HandleTypeDef htim4;
//extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;

//P MGS-TYPE SPARE DATA-ID  DATA-VALUE
//0 000      0000  00000000 00000000 00000000
unsigned long requests[] = {
		0x300, //0 get status
		//0x90014000, //1 set CH temp //64C
		//0x80190000, //25 Boiler water temperature
		0,
		((0x90000000) | (((long) 1) << 16)),
		((0x00000000) | (((long) 3) << 16)),
		((0x00000000) | (((long) 5) << 16)),
		((0x00000000) | (((long) 6) << 16)),
		((0x00000000) | (((long) 9) << 16)),
		((0x00000000) | (((long) 10) << 16)),
		((0x00000000) | (((long) 12) << 16)),
		((0x80000000) | (((long) 13) << 16)),  //
		((0x00000000) | (((long) 15) << 16)),
		((0x00000000) | (((long) 17) << 16)),
		((0x00000000) | (((long) 18) << 16)),
		((0x80000000) | (((long) 19) << 16)),  //
		((0x00000000) | (((long) 24) << 16)),  //
		0x807c0000,
		((0x00000000) | (((long) 125) << 16)),  //
		((0x80000000) | (((long) 127) << 16)),  //
		};

uint8_t readReq[] = {0,5,15,115, 3,125,127, 17,18,19,25,26,27,28,31,32,33,116,117,118,119,120,121,122,123, 15 };
#define RRLEN 26

typedef struct {
	uint16_t targetTemp;
	//int index;
	//bool busy;
} OTCommonStruct;
OTCommonStruct OTCommon;

//P MGS-TYPE SPARE DATA-ID  DATA-VALUE
//0 000      0000  00000000 00000000 00000000
typedef struct {
	//16
	uint16_t DATA_VALUE :16;
	//8
	uint8_t DATA_ID :8;
	//8
	uint8_t SPARE :4;
	uint8_t MSG_TYPE :3;
	bool PARITY :1;
} OTFrameStruct;

union OTFrameUnion {
	uint32_t raw;
	OTFrameStruct frame;
};

typedef struct {
	uint16_t RPiRequestHI;
	uint16_t RPiRequestLO;
	uint16_t RPiResponseHI;
	uint16_t RPiResponseLO;
	union OTFrameUnion rx;
	union OTFrameUnion rx_ext;
	union OTFrameUnion tx;
	uint32_t errorCount;
	//OT_Frame_Struct reg[256];
	uint32_t reg[20];
	bool timeout;
	bool busy;
	bool complete;
	bool frameSendedAndStartWaitingACK;
	bool readingResponse;
	uint32_t dataRegisters[128];
	uint8_t index;
} OT_Struct;
OT_Struct ot;

bool parity;
bool ext = false;

typedef struct {
	//LB
	bool faultIndication :1;
	bool CHMode :1;
	bool DHWMode :1;
	bool FlameStatus :1;
	bool CoolingStatus :1;
	bool CH2Status :1;
	bool diagnosticIndication :1;
	bool reserved :1;
	//HB
	bool CHEnable :1;
	bool DHWEnable :1;
	bool CoolingEnable :1;
	bool OTCActive :1;
	bool CH2Enable :1;
	uint8_t reservedHB :3;
} OTDataValueID0Struct;

typedef struct {
	uint16_t controlSetpoint;
} OTDataValueF88Struct;

typedef struct {
	//LB
	uint8_t SlaveMemberID :8;
	//HB
	bool DHWPresent :1;
	bool ControlType :1;
	bool CoolingConfig :1;
	bool DHWConfig :1;
	bool MasterLowOff :1;
	bool CH2Present :1;
	uint8_t reservedHB :2;
} OTDataValueID3Struct;

typedef struct {
	//LB
	uint8_t OEMFaultCode :8;
	//HB
	bool ServiceRequered :1;
	bool LockoutReset :1;
	bool LowWaterPressure :1;
	bool GasFlameFault :1;
	bool AirPressureFault :1;
	bool WaterOverTemp :1;
	uint8_t reservedHB :2;
} OTDataValueID5Struct;
typedef struct {
	//LB
	uint8_t productVersion :8;
	//HB
	uint8_t productType :8;
} OTDataValueID126and127Struct;

union OTDataValueUnion {
	uint16_t raw;
	OTDataValueID0Struct ID0;
	OTDataValueF88Struct ID1;
	OTDataValueID3Struct ID3;
	OTDataValueID5Struct ID5;
	OTDataValueF88Struct ID8;
	OTDataValueF88Struct ID124;
	OTDataValueF88Struct ID125;
	OTDataValueID126and127Struct ID126;
	OTDataValueID126and127Struct ID127;
};
typedef struct {
	OTDataValueID0Struct ID0;
	OTDataValueF88Struct ID1;
	OTDataValueID3Struct ID3;
	OTDataValueID5Struct ID5;
	OTDataValueF88Struct ID8;
	OTDataValueF88Struct ID124;
	OTDataValueF88Struct ID125;
	OTDataValueID126and127Struct ID126;
	OTDataValueID126and127Struct ID127;
} OTDataRegistersStruct;
OTDataRegistersStruct OTDR;

union OTDataValueUnion dv = { 0 };
//OT
void initOT(void);
void setIdleState(void);
void setActiveState(void);
void activateBoiler(void);
void sendBit(bool high);
void sendFrame(uint32_t request);
void printBinary(uint32_t val);
uint32_t sendRequest(uint32_t request);
bool waitForResponse();
uint32_t readResponse();
void delayMicros(uint32_t t);
uint32_t checkParity(uint32_t val);
void startWaiting(void);
bool checkACK(void);
void startReadResponse(void);
bool checkTimerOVF(void);
uint32_t calculateResponse(void);
bool parityBit(uint32_t d);
//OT
void OTRoute(void) {
	if (!ot.busy) {
		ot.busy = true;
//		ot.complete = false;
//		ot.frameSendedAndStartWaitingACK = false;
//		ot.readingResponse = false;
		//OT
		// for (ot.index=0; ot.index < OT_QUEUE_LENGTH; ot.index++) {
		//HAL_Delay(300);

		/*if(ot.index==2){
			OTFrameStruct req1;
			req1.DATA_ID = 1;
			req1.MSG_TYPE = OT_MSG_TYPE_M_WRITE_DATA;
			req1.PARITY = 1;
			req1.DATA_VALUE = OTCommon.targetTemp;
			union OTFrameUnion reqU;
			reqU.frame = req1;
			requests[ot.index] = reqU.raw;//((0x90000000) | (((long) 1) << 16)) + OTCommon.targetTemp;
		}*/
		union OTFrameUnion reqU;
		reqU.frame.DATA_ID = readReq[ot.index];
		reqU.frame.MSG_TYPE = OT_MSG_TYPE_M_READ_DATA;
		reqU.frame.DATA_VALUE = 0;
		reqU.frame.PARITY = parityBit(reqU.raw);
		//reqU.raw = ot.dataRegisters[readReq[ot.index]];

							/*OTFrameStruct req1;
							req1.DATA_ID = readReq[ot.index];
							req1.MSG_TYPE = OT_MSG_TYPE_M_READ_DATA;
							//if(reqU.frame.MSG_TYPE == OT_MSG_TYPE_S_UNKNOWN_DATAID)
							//	req1.PARITY = 1;
							//else

							req1.DATA_VALUE = 0;
							req1.PARITY = parityBit();
							reqU.frame = req1;*/
							//requests[ot.index] = reqU.raw;

		ot.tx.raw = reqU.raw;//requests[ot.index];
		//ot.tx.raw = 0;
		//ot.tx.frame.DATA_ID = req[index];
		//ot.tx.frame.PARITY = checkParity( ot.tx.raw );
		sendFrame(ot.tx.raw);

		//waitForResponse();
		startWaiting();

		ot.frameSendedAndStartWaitingACK = true;
		/*while (!checkACK()) {
		 }*/

	}
	if (ot.frameSendedAndStartWaitingACK && checkACK()) {
		ot.frameSendedAndStartWaitingACK = false;

		if (!ot.timeout) {
			delayMicros(1000 * 1.25);
			startReadResponse();

			ot.readingResponse = true;
			while (!checkTimerOVF()) {
			}
			ot.rx.raw = calculateResponse();
			//ot.reg[ot.index] = ot.rx.raw;	//sendRequest( requests[ot.index] );
			//ot.dataRegisters[ot.rx.frame.DATA_ID] = ot.rx.raw;

			if ( (ot.rx.frame.MSG_TYPE == OT_MSG_TYPE_S_READ_ACK
							|| ot.rx.frame.MSG_TYPE == OT_MSG_TYPE_S_WRITE_ACK)) {

				ot.dataRegisters[ot.rx.frame.DATA_ID] = ot.rx.raw;
				dv.raw = ot.rx.frame.DATA_VALUE;
				if (ot.rx.frame.DATA_ID == 0) {
					OTDR.ID0 = dv.ID0;
				}
				if (ot.rx.frame.DATA_ID == 3) {
					OTDR.ID3 = dv.ID3;
				}
				if (ot.rx.frame.DATA_ID == 5) {
					OTDR.ID5 = dv.ID5;
				}
				if (ot.rx.frame.DATA_ID == 125)
					OTDR.ID125 = dv.ID125;
				if (ot.rx.frame.DATA_ID == 127)
					OTDR.ID127 = dv.ID127;

			}
			//ot.rx.frame.MSG_TYPE;


			if (ot.index > RRLEN){
				ot.index = 0;
			}else{
				ot.index++;
				HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
			}
			//OTCommon.busy = true;
			// }
			//HAL_Delay(300);
		}
		ot.complete = true;
	}
	if(ot.frameSendedAndStartWaitingACK && ot.timeout){
		ot.busy = false;
		ot.complete = false;
		ot.frameSendedAndStartWaitingACK = false;
		ot.readingResponse = false;
		ot.timeout = false;
//		if (ot.index > RRLEN){
//			ot.index = 0;
//		}else{
//			ot.index++;
//		}
//		HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_1);
//		HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_2);
	}
	if (ot.readingResponse && checkTimerOVF()) {
		ot.readingResponse = false;

	}
}
void initOT(void) {
	ot.busy = false;
	ot.complete = false;
	ot.frameSendedAndStartWaitingACK = false;
	ot.readingResponse = false;
	ot.timeout = false;
	ot.index = 0;
	//OTCommon.busy = false;
	OTCommon.targetTemp = 0;
}
void setIdleState(void) {
	HAL_GPIO_WritePin(OT_RXO_GPIO_Port, OT_RXO_Pin, GPIO_PIN_SET);
}
void setActiveState(void) {
	HAL_GPIO_WritePin(OT_RXO_GPIO_Port, OT_RXO_Pin, GPIO_PIN_RESET);
}
void activateBoiler(void) {
	setIdleState();
	HAL_Delay(1000);
}

bool tim1ovf = false;
bool setBIT;
void sendBit(bool high) {
	//TIM1->ARR = 1000*2;
	//TIM1->CCR1 = 500*2;
	/*//setBIT = high;
	 if (high) setActiveState(); else setIdleState();
	 //HAL_TIM_OC_Start_IT(&htim2,TIM_CHANNEL_1);

	 //while(HAL_TIM_OC_GetState(&htim1)==HAL_TIM_STATE_BUSY){

	 //}
	 //	  while (TIM1->CNT < TIM1->CCR1){
	 //
	 //	  		}

	 //delayMicros(500);
	 TIM1->CNT = 0;
	 TIM1->ARR = 500*2;
	 HAL_TIM_Base_Start(&htim1);
	 while (TIM1->CNT < TIM1->CCR1){

	 }

	 HAL_TIM_Base_Stop(&htim1);
	 //tim1ovf=false;
	 if (high) setIdleState(); else setActiveState();
	 //delayMicros(500);
	 TIM1->CNT=0;
	 TIM1->ARR = 500*2;
	 HAL_TIM_Base_Start(&htim1);
	 while (TIM1->CNT < TIM1->ARR){

	 }
	 HAL_TIM_Base_Stop(&htim1);*/
	//tim1ovf=false;
	//HAL_TIM_OC_Stop_IT(&htim2,TIM_CHANNEL_1);
	if (high)
		setActiveState();
	else
		setIdleState();
	delayMicros(500);
	if (high)
		setIdleState();
	else
		setActiveState();
	delayMicros(500);
}
void sendFrame(uint32_t request) {
	sendBit(true); //start bit
	for (int i = 31; i >= 0; i--) {
		sendBit((request >> i & 1)); //bitRead(request, i));
	}
	sendBit(true); //stop bit
	setIdleState();
}
//void printBinary(uint32_t val);
uint32_t sendRequest(uint32_t request) {
	//ot.tx.raw = request;
	sendFrame(request);

	if (!waitForResponse())
		return 0;

	return readResponse();
}
volatile bool rised = false;
volatile uint32_t risedCount;
void startWaiting() {
	rised = false;
	ot.timeout = false;
	TIM2->PSC = 64000;
	TIM2->ARR =1200;
	htim2.Init.Prescaler = 64000;
	htim2.Init.Period = 1200;
	HAL_TIM_Base_Init(&htim2);
	HAL_TIM_IC_Init(&htim2);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
}
bool checkACK(void) {
	if (TIM2->CNT >= TIM2->ARR) {
		HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_1);
		ot.timeout = true;
	}
	if (rised) {
		ot.timeout = false;
		HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_1);
	}
	return rised || TIM2->CNT >= TIM2->ARR;
}
bool waitForResponse() {
	//uint32_t time_stamp = HAL_GetTick();
	startWaiting();
	/*	while (!rised) { //HAL_GPIO_ReadPin(OT_TXI_GPIO_Port,OT_TXI_Pin) == GPIO_PIN_RESET) { //start bit
	 if (TIM2->CNT >= TIM2->ARR) {
	 HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_1);
	 //Serial.println("Response timeout");
	 ot.timeout = true;
	 return false;
	 }
	 }
	 ot.timeout = false;
	 HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_1);*/
	while (!checkACK()) {

	}
	delayMicros(1000 * 1.25); //wait for first bit
	return true;
}
volatile bool readResponseInProgress = false;
volatile uint32_t ttime[64];
volatile bool rf[64];
volatile swIndex = 0;
//uint32_t ddd=0;

void startReadResponse(void) {
	ot.rx_ext.raw = 0;
	swIndex = 0;
	TIM2->PSC = 64;
	TIM2->ARR = 40000;
	TIM2->CNT = 0;
	htim2.Init.Prescaler = 64;
	htim2.Init.Period = 40000;
	HAL_TIM_IC_Init(&htim2);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
}
bool checkTimerOVF(void) {
	return TIM2->CNT >= TIM2->ARR;
}
uint32_t calculateResponse(void) {
	HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_1);
	HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_2);
	int cnt = 0;
	uint32_t response = 0;
	if (!rf[0])
		response |= (1 << 0);
	uint32_t oldt = ttime[0];
	for (int i = 1; i < 64; i++) {
		if (ttime[i] > oldt + 900) {
			response = response << 1;
			if (!rf[i])
				response |= 1;
			oldt = ttime[i];
			cnt++;
			if (cnt == 31)
				break;
		}
	}
	return response;
}

uint32_t readResponse() {
	//readResponseInProgress = true;
	//unsigned long response = 0;
	//bool state;

	/*for(int i=0;i<32;i++){
	 arrR[i] = arrF[i] = 0;
	 }*/

	startReadResponse();
	/*ot.rx_ext.raw = 0;
	 swIndex = 0;
	 TIM2->PSC = 64;
	 TIM2->ARR = 40000;
	 TIM2->CNT = 0;
	 htim2.Init.Prescaler = 64;
	 htim2.Init.Period = 40000;
	 //HAL_TIM_Base_Init(&htim2);
	 HAL_TIM_IC_Init(&htim2);
	 //HAL_TIM_Base_Start_IT(&htim2);
	 HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
	 HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
	 */

	//for (int i = 0; i < 32; i++) {
	//delayMicros(100);
	// swIndex= i;
//	    response = (response << 1) | HAL_GPIO_ReadPin(OT_TXI_GPIO_Port,OT_TXI_Pin);
//	    state = HAL_GPIO_ReadPin(OT_TXI_GPIO_Port,OT_TXI_Pin);
	//TIM4->ARR = 500*2;
	//HAL_TIM_Base_Start(&htim4);
	//HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);
	//HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);!stateChanged){//
//	    while (state == HAL_GPIO_ReadPin(OT_TXI_GPIO_Port,OT_TXI_Pin) ){
//	    }
//	    stateChanged=false;
	/*if(state)
	 arrF[swIndex] = TIM2->CNT;
	 else
	 arrR[swIndex] = TIM2->CNT;*/
	// HAL_TIM_IC_Stop_IT(&htim2,TIM_CHANNEL_1);
	//HAL_TIM_IC_Stop_IT(&htim2,TIM_CHANNEL_2);
	//HAL_TIM_Base_Stop(&htim4);
	//delayMicros(1000);
	//}
	//delayMicros(1000);
	while (!checkTimerOVF()) {
	}
	//HAL_TIM_Base_Stop_IT(&htim2);
	/*HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_1);
	 HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_2);

	 readResponseInProgress = false;

	 int cnt = 0;
	 uint32_t response = 0;
	 if (!rf[0])
	 response |= (1 << 0);
	 uint32_t oldt = ttime[0];
	 for (int i = 1; i < 64; i++) {
	 if (ttime[i] > oldt + 900) {
	 response = response << 1;
	 if (!rf[i])
	 response |= 1;
	 oldt = ttime[i];
	 cnt++;
	 if (cnt == 31)
	 break;
	 }
	 }*/
	ot.rx.raw = calculateResponse();	  //response;
	return ot.rx.raw;
}
void delayMicros(uint32_t t) {
	TIM4->ARR = t * 2;
	HAL_TIM_Base_Start(&htim4);
	while (TIM4->CNT < TIM4->ARR) {

	}
	HAL_TIM_Base_Stop(&htim4);
}

uint32_t checkParity(uint32_t val) {
	val = val & 0x7FFFFFFF;
	val ^= val >> 16;
	val ^= val >> 8;
	val ^= val >> 4;
	val ^= val >> 2;
	val ^= val >> 1;
	return (val) & 1;
}
bool parityBit(uint32_t d){
    uint8_t x = 0;
    for(uint8_t i=1;i<31;i++)
        if( ((d>>i)&1))
            x++;
    return x%2;
}

#endif /* OT_H_ */
