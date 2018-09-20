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

#define AT_AT 			0
#define AT_CREG			1
#define AT_COPS			2

#define AT_SET_CMGF		3
#define AT_GET_CMGF		4// 0 - UDP mode supported, 1 - Text mode supported

// from http://wiredlogic.io/ru/diy-gsm-sim900-stm32-open-source-ru/
#define AT_CPAS			5 //Ожидание готовности GSM-модуля и SIM-карты:
#define AT_GSN			6 //Чтение IMEI GSM-модуля:
#define AT_CSPN			7 //Чтение имени провайдера:
#define AT_CUSD			8 //Чтение баланса SIM-карты.
//from http://m2msupport.net/m2msupport/sim-at-commands-for-sim-presense-and-status/
#define AT_CPIN			9 //AT commands for SIM presense and status
//HTTP from http://2150692.ru/faq/62-gprs-svyaz-cherez-sim800l-i-arduino
#define AT_SAPBR_CONTYPE 10
#define AT_SAPBR_APN	11
#define AT_SAPBR_USER	12
#define AT_SAPBR_PWD	13
#define AT_SAPBR		14 //Устанавливаем GPRS соединение
#define AT_HTTPINIT		15 //Инициализация http сервиса
#define AT_HTTPPARA		16 //Установка CID параметра для http сессии
#define AT_HTTPPARA_DATA 17 //Процедура отправки данных на сервер
#define AT_HTTPACTION	18 //GET=0 POST=1 HEAD=2
//Отправка SMS-сообщения:
#define AT_CMGS			19
#define AT_CMGS_SMS_TEXT 20

#define RX_BUFFER_SIZE 1
#define OK 1
#define ERROR 0
#define INIT_STATE -1

char* queue[10];
typedef struct {
	bool busy;
	//bool transmitRequered;
	//bool waitForResponse;
	// char reserved:6;
	char rx_buff[RX_BUFFER_SIZE];
	char rxChar[1024];
	uint16_t i;
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

typedef struct {
	char* request;
	int8_t response;
	char* args;
} ATLine_Struct;
typedef struct {
	// int8_t AT;
	ATLine_Struct at;
	ATLine_Struct creg;
	ATLine_Struct cops;

	ATLine_Struct setCMGF;
	ATLine_Struct getCMGF; // 0 - UDP mode supported, 1 - Text mode supported

	ATLine_Struct cpas;
	ATLine_Struct gsn;
	ATLine_Struct cspn;
	ATLine_Struct cusd;
	ATLine_Struct cpin;
	//HTTP
	ATLine_Struct sapbrCONTYPE;
	ATLine_Struct sapbrAPN;
	ATLine_Struct sapbrUSER;
	ATLine_Struct sapbrPWD;
	ATLine_Struct sapbr;
	ATLine_Struct httpinit;
	ATLine_Struct httppara;


	ATLine_Struct cmgs;
	ATLine_Struct message;
} AT_Struct;
AT_Struct AT;

int strpos(char* hay, char* needle, int offset) {
	char haystack[strlen(hay)];
	strncpy(haystack, hay + offset, strlen(hay) - offset);
	char* p = strstr(haystack, needle);
	if (p)
		return p - haystack + offset;
	return -1;
}

void initAT() {
	gprs.TX = "";
	gprs.RX = "";
	//gprs.transmitRequered = false;
	//gprs.waitForResponse = false;
	gprs.busy = false;

	gprs.index = 0;
	gprs.i=0;
	//gprs.readIndex = 0;

	AT.at.request = "AT\r\n";
	AT.at.response = INIT_STATE;

	AT.cops.request = "AT+COPS?\r\n";
	AT.cops.response = INIT_STATE;

	AT.setCMGF.request = "AT+CMGF=1\r\n"; // Текстовый режим (не PDU)
	AT.setCMGF.response = INIT_STATE;

	AT.getCMGF.request = "AT+CMGF=?\r\n";
	AT.getCMGF.response = INIT_STATE;

	AT.cpas.request = "AT+CPAS\r\n"; // Ожидание готовности GSM-модуля и SIM-карты
	AT.cpas.response = INIT_STATE;

	AT.gsn.request = "AT+GSN\r\n"; //Чтение IMEI GSM-модуля:
	AT.gsn.response = INIT_STATE;

	AT.cspn.request = "AT+CSPN?\r\n"; // Чтение имени провайдера
	AT.cspn.response = INIT_STATE;

	AT.cusd.request = "AT+CUSD=1,'*100#'\r\n"; //, Request //Чтение баланса SIM-карты
	AT.cusd.response = INIT_STATE;

	AT.cpin.request = "AT+CPIN?\r\n"; //SIM not inserted
	AT.cpin.response = INIT_STATE;

	AT.sapbrCONTYPE.request =  "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n";
	AT.sapbrCONTYPE.response = INIT_STATE;

	AT.sapbrAPN.request = "";
	//"AT+CSQ\r\n" //качество сигнала

	queue[0] = "AT\r\n";
	queue[1] = "AT+CSQ\r\n";
	queue[2] = "AT+CMGF=1\r\n";
	queue[3] = "AT+CMGS=\"+79518926260\"\r\n";
	queue[4] = "hello\x1a\r\n";

	HAL_UART_Receive_IT(&huart1, gprs.rx_buff, RX_BUFFER_SIZE);
}
void sendQueue() {
	if (!gprs.busy) {
		gprs.busy = true;

		gprs.TX = "AT\r\n";
//		gprs.TX = "AT+CPIN?\r\n"; //..симка//5s
//		gprs.TX = "AT+CFUN?\r\n"; //1=полная работоспособность//10s
//		gprs.TX = "AT+CBC\r\n"; //напряжение питания
//		gprs.TX = "AT+CSQ\r\n"; //качество сигнала
//		gprs.TX = "AT+CPAS\r\n"; // Ожидание готовности GSM-модуля и SIM-карты //2 Unknown (MT is not guaranteed to respond totructions)
//		gprs.TX = "AT+CSPN?\r\n"; // Чтение имени провайдера
		gprs.TX = "AT+CSCS=\"PCCP\"\r\n";
		//gprs.TX = "AT+CMGF=1\r\n"; // Текстовый режим (не PDU)
//		gprs.TX = "AT+CREG?\r\n"; //Тип регистрации в сети
		gprs.TX = "AT+CUSD=1,\"*102#\"\r\n"; //, Request //Чтение баланса SIM-карты //20s

//		gprs.TX = "AT+CMGF=1\r\n"; // Текстовый режим (не PDU)
//		gprs.TX = "AT+CMGS=\"+79518926260\"\r\n";//sms
//		gprs.TX = "hello\x1a\r\n";

		//gprs.TX = "AT+CMGL=4\r\n";
		//gprs.TX = "AT+CMGR=3\r\n";
		//gprs.TX = "AT+CPMS=\"MT\"\r\n";

		//gprs.TX = "AT+CCID\r\n";
		//gprs.TX = AT.cops.request;

		//gprs.TX = "hello\x1a";
		//gprs.TX = queue[gprs.index];
		HAL_UART_Transmit_IT(&huart1, gprs.TX, strlen(gprs.TX));
	}
	if(HAL_GetTick()%5000==0 && gprs.index<4){
		gprs.index ++;
		//gprs.busy=false;

	}
	return;
}

void checkAT() {
	/*size_t len = strlen(gprs.TX); //namelen = strlen(name);
	//
	//har* copy;
	char* str2;
	if (len != 0) {
		str2 = (char*) malloc(len - 1);
		strncpy(str2, gprs.TX, len - 2);
		str2[len - 1] = 0;
		free(str2);
	}

	int txPosition = strpos(gprs.RX, str2, 0);
	// printf ( "tx position: %d\n\n", txPosition );

	if (len != 0)
		free(str2);

	int okPosition = strpos(gprs.RX, "OK", 0);
	// printf ( "ok position: %d\n\n", okPosition );

	int errorPosition = strpos(gprs.RX, "ERROR", 0);
	// printf ( "error position: %d\n\n", errorPosition );

	int8_t response = (okPosition > 0) ? OK :
						(errorPosition > 0) ? ERROR : INIT_STATE;
	if (txPosition >= 0) {
		if (gprs.TX == AT.at.request) {
			AT.at.response = response;
		} else if (gprs.TX == AT.cops.request) {
			AT.cops.response = response;
			char* found = strstr(gprs.RX, ": ");
			AT.cops.args = found + 2;
		}
		//gprs.RX = "";
		//gprs.busy=false;
	}*/
	return;
}

void checkUpdate() {
	for (int i = 0; i < RX_BUFFER_SIZE; i++) {
		/*if (gprs.rx_buff[i] <127 &&  gprs.rx_buff[i]>31){//!= '\0') {

			size_t len = strlen(gprs.RX);
			char* str2 = (char*) malloc(len + 1 + 1);
			strcpy(str2, gprs.RX);
			str2[len] = gprs.rx_buff[i];
			str2[len + 1] = '\0';

			gprs.RX = str2;
			//free(str2);
		}
		gprs.rx_buff[i] = '\0';*/
		gprs.rxChar[gprs.i] = gprs.rx_buff[i];
		gprs.i++;
	}
	HAL_UART_Receive_IT(&huart1, gprs.rx_buff, RX_BUFFER_SIZE);
}
void checkReceive() {
}
#endif /* SIM800L_H_ */
