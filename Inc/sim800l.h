#ifndef SIM800L_H_
#define SIM800L_H_

// for sim800
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f1xx_hal.h"

extern UART_HandleTypeDef huart1;

#define RX_BUFFER_SIZE 1
#define OK 1
#define ERROR 0
#define INIT_STATE -1

char* queue[10];
typedef struct
{
  bool busy;
  //bool transmitRequered;
  //bool waitForResponse;
  // char reserved:6;
  char rx_buff[RX_BUFFER_SIZE];
  // char tx_buff[64];
  // char RXEcho[128];
  // char RXResponse[128];
  uint8_t index;
  //uint8_t readIndex;
  // char reserved2;

  char* TX;
  char* RX;
} SIM800_Struct; // gprs = {0,0,0,0,"",""};

SIM800_Struct gprs;

typedef struct
{
  char* request;
  int8_t response;
  char* args;
} ATLine_Struct;
typedef struct
{
  // int8_t AT;
  ATLine_Struct at;
  ATLine_Struct creg;
  ATLine_Struct cops;

  ATLine_Struct setCMGF;
  ATLine_Struct getCMGF; // 0 - UDP mode supported, 1 - Text mode supported

  ATLine_Struct cmgs;
  ATLine_Struct message;
} AT_Struct;
AT_Struct AT;

int
strpos(char* hay, char* needle, int offset)
{
  char haystack[strlen(hay)];
  strncpy(haystack, hay + offset, strlen(hay) - offset);
  char* p = strstr(haystack, needle);
  if (p)
    return p - haystack + offset;
  return -1;
}

void
initAT()
{
  gprs.TX = "";
  gprs.RX = "";
  //gprs.transmitRequered = false;
  //gprs.waitForResponse = false;
  gprs.busy = false;

  gprs.index = 0;
  //gprs.readIndex = 0;

  AT.at.request = "AT\r\n";
  AT.at.response = INIT_STATE;

  AT.cops.request = "AT+COPS?\r\n";
  AT.cops.response = INIT_STATE;

  AT.setCMGF.request = "AT+CMGF=1\r\n";
  AT.setCMGF.response = INIT_STATE;

  AT.getCMGF.request = "AT+CMGF=?\r\n";
  AT.getCMGF.response = INIT_STATE;

  queue[0] = AT.at.request;
  queue[1] = AT.cops.request;

  HAL_UART_Receive_IT(&huart1,gprs.rx_buff,RX_BUFFER_SIZE);
}
void
sendQueue()
{
  if (!gprs.busy) {
    gprs.busy = true;
    gprs.TX = queue[gprs.index];
    HAL_UART_Transmit_IT(&huart1,gprs.TX,strlen(gprs.TX));
  }
  return;
}

void
checkAT()
{
	size_t len = strlen(gprs.TX); //namelen = strlen(name);
	//
	//har* copy;
	char* str2;
	if(len!=0){
		str2 = (char*)malloc(len-1);
		strncpy(str2, gprs.TX,len-2);
		str2[len-1] =0;
		free(str2);
	}

  int txPosition = strpos(gprs.RX, str2, 0);
  // printf ( "tx position: %d\n\n", txPosition );

  if(len!=0)
  	free(str2);

  int okPosition = strpos(gprs.RX, "OK", 0);
  // printf ( "ok position: %d\n\n", okPosition );

  int errorPosition = strpos(gprs.RX, "ERROR", 0);
  // printf ( "error position: %d\n\n", errorPosition );

  int8_t response =
    (okPosition > 0) ? OK : (errorPosition > 0) ? ERROR : INIT_STATE;
  if (txPosition >= 0) {
    if (gprs.TX == AT.at.request) {
      AT.at.response = response;
    } else if (gprs.TX == AT.cops.request) {
      AT.cops.response = response;
      char* found = strstr(gprs.RX, ": ");
      AT.cops.args = found + 2;
    }
  }
  return;
}

void
checkUpdate()
{
	for(int i=0;i<RX_BUFFER_SIZE;i++){
	  if (gprs.rx_buff[i] != '\0') {

		size_t len = strlen(gprs.RX);
		char* str2 = (char*)malloc(len + 1 + 1);
		strcpy(str2, gprs.RX);
		str2[len] = gprs.rx_buff[i];
		str2[len + 1] = '\0';

		gprs.RX = str2;
		//free(str2);
	  }
	  gprs.rx_buff[i] = '\0';
	}
	  HAL_UART_Receive_IT(&huart1, gprs.rx_buff, RX_BUFFER_SIZE);
}
void
checkReceive()
{}
#endif /* SIM800L_H_ */
