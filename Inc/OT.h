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


//P MGS-TYPE SPARE DATA-ID  DATA-VALUE
//0 000      0000  00000000 00000000 00000000
unsigned long requests[] = {
  0x300, //0 get status
  //0x90014000, //1 set CH temp //64C
  //0x80190000, //25 Boiler water temperature
  0,
 ( (0x00000000) | ( ((long)3)<<16) ),
 ( (0x00000000) | ( ((long)5)<<16) ),
 ( (0x00000000) | ( ((long)6)<<16) ),
 ( (0x00000000) | ( ((long)9)<<16) ),
 ( (0x00000000) | ( ((long)10)<<16) ),
 ( (0x00000000) | ( ((long)12)<<16) ),
 ( (0x80000000) | ( ((long)13)<<16) ),//
 ( (0x00000000) | ( ((long)15)<<16) ),
 ( (0x00000000) | ( ((long)17)<<16) ),
 ( (0x00000000) | ( ((long)18)<<16) ),
 ( (0x80000000) | ( ((long)19)<<16) ),//
 ( (0x00000000) | ( ((long)24)<<16) ),//
  0x807c0000,
 ( (0x80000000) | ( ((long)127)<<16) ),//
};

uint8_t req[] = {
		3,5,6,9,10,12,13,15,17,18,19,24,124,127
};

//P MGS-TYPE SPARE DATA-ID  DATA-VALUE
//0 000      0000  00000000 00000000 00000000
typedef struct {
	//16
	uint16_t DATA_VALUE:16;
	//8
	uint8_t DATA_ID:8;
	//8
	uint8_t SPARE:4;
	uint8_t MSG_TYPE:3;
	bool PARITY: 1;
} OTFrameStruct;

union OTFrameUnion {
	uint32_t raw;
	OTFrameStruct frame;
} ;

typedef struct{
	union OTFrameUnion rx;
	union OTFrameUnion rx_ext;
	union OTFrameUnion tx;
	uint32_t errorCount;
	//OT_Frame_Struct reg[256];
	uint32_t reg[20];
}OT_Struct;
OT_Struct ot;

bool parity;
bool ext = false;

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
//OT
void initOT(void){

}
void setIdleState(void){
	HAL_GPIO_WritePin(OT_RXO_GPIO_Port, OT_RXO_Pin, GPIO_PIN_SET);
}
void setActiveState(void){
	HAL_GPIO_WritePin(OT_RXO_GPIO_Port, OT_RXO_Pin, GPIO_PIN_RESET);
}
void activateBoiler(void){
	setIdleState();
	HAL_Delay(1000);
}
void sendBit(bool high){
	  if (high) setActiveState(); else setIdleState();
	  delayMicros(500);
	  if (high) setIdleState(); else setActiveState();
	  delayMicros(500);
}
void sendFrame(uint32_t request){
	  sendBit(true); //start bit
	  for (int i = 31; i >= 0; i--) {
	    sendBit( (request>>i & 1) );//bitRead(request, i));
	  }
	  sendBit(true); //stop bit
	  setIdleState();
}
//void printBinary(uint32_t val);
uint32_t sendRequest(uint32_t request){
	//ot.tx.raw = request;
	sendFrame(request);

	  if (!waitForResponse()) return 0;

	  return readResponse();
}
bool waitForResponse(){
	uint32_t time_stamp = HAL_GetTick();
	  while (HAL_GPIO_ReadPin(OT_TXI_GPIO_Port,OT_TXI_Pin) == GPIO_PIN_RESET) { //start bit
	    if (HAL_GetTick() - time_stamp >= 1000) {
	      //Serial.println("Response timeout");
	      return false;
	    }
	  }
	  delayMicros(1000 * 1.25); //wait for first bit
	  return true;
}
uint32_t readResponse(){
	  unsigned long response = 0;
	  bool state;
	  ot.rx_ext.raw = 0;
	  for (int i = 0; i < 32; i++) {
	    response = (response << 1) | HAL_GPIO_ReadPin(OT_TXI_GPIO_Port,OT_TXI_Pin);
	    state = HAL_GPIO_ReadPin(OT_TXI_GPIO_Port,OT_TXI_Pin);
	    while (state == HAL_GPIO_ReadPin(OT_TXI_GPIO_Port,OT_TXI_Pin)){
	    }

	    delayMicros(500);
	  }

	  ot.rx.raw = response;
	  return response;
}
void delayMicros(uint32_t t){
	TIM4->ARR = t*2;
	HAL_TIM_Base_Start(&htim4);
	while (TIM4->CNT < TIM4->ARR){

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

#endif /* OT_H_ */
