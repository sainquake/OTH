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

typedef enum {
	AT_AT = 0, AT_CREG = 1, AT_COPS = 2,

	AT_SET_CMGF = 3, AT_GET_CMGF = 4, // 0 - UDP mode supported, 1 - Text mode supported

// from http://wiredlogic.io/ru/diy-gsm-sim900-stm32-open-source-ru/
	AT_CPAS = 5, //Ожидание готовности GSM-модуля и SIM-карты:
	AT_GSN = 6, //Чтение IMEI GSM-модуля:
	AT_CSPN = 7, //Чтение имени провайдера:
	AT_CUSD = 8, //Чтение баланса SIM-карты.
//from http://m2msupport.net/m2msupport/sim-at-commands-for-sim-presense-and-status/
	AT_CPIN = 9, //AT commands for SIM presense and status
//HTTP from http://2150692.ru/faq/62-gprs-svyaz-cherez-sim800l-i-arduino
	AT_SAPBR_CONTYPE = 10,
	AT_SAPBR_APN = 11,
	AT_SAPBR_USER = 12,
	AT_SAPBR_PWD = 13,
	AT_SAPBR = 14, //Устанавливаем GPRS соединение
	AT_HTTPINIT = 15, //Инициализация http сервиса
	AT_HTTPPARA = 16, //Установка CID параметра для http сессии
	AT_HTTPPARA_DATA = 17, //Процедура отправки данных на сервер
	AT_HTTPACTION = 18, //GET=0 POST=1 HEAD=2
//Отправка SMS-сообщения:
	AT_CMGS = 19,
	AT_CMGS_SMS_TEXT = 20,
	AT_CSQ = 21,
	AT_CMGF = 22,
	AT_CBC = 23,
	AT_CMGR = 24, //read sms from mem
	AT_CPMS = 25, //count of sms
	AT_CMGDA = 26,
	AT_RESPONSE_TIMEOUT = 255
} AT_Enum;

/*#define AT_AT 			0
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
 #define AT_CMGS_SMS_TEXT 20*/

#define RX_BUFFER_SIZE 32
#define OK 1
#define ERROR 0
#define INIT_STATE -1

typedef struct {
	char* request;
	int8_t response;
	char* args;
} ATLine_Struct;
uint8_t queue[10];
typedef struct {
	uint8_t quality;
	uint8_t charge;
	uint16_t voltage;
	char *operator;
	bool timeoutOccured;
	bool balanceRequered;
	bool balanceReceived;
	float balance;
	uint8_t smsCount;
	uint8_t smsToRead;

	uint8_t subaddress;

	//perform specific request
	uint8_t subds;
	bool specificRequest;
	//
	bool busy;
	//bool transmitRequered;
	//bool waitForResponse;
	// char reserved:6;
	char rx_buff[RX_BUFFER_SIZE];
	//char rxChar[1024];
	uint16_t i;
	// char tx_buff[64];
	// char RXEcho[128];
	// char RXResponse[128];
	uint8_t index;
	//uint8_t readIndex;
	// char reserved2;
	int txPosition;
	int okPosition;
	int errorPosition;
	int8_t response;
	uint32_t timeout;
	bool RXOvf;

	ATLine_Struct at[256];

	char* TX;
	char* RXPointer;
	char* RXFinal;
	char RX[512];
} SIM800_Struct; // gprs = {0,0,0,0,"",""};

SIM800_Struct gprs;

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
int hex_to_int(char c);
int hex_to_ascii(char c, char d);
int strpos(char* hay, char* needle, int offset);
char* atCMGRPrepare(uint8_t smsToRead_);

void initAT() {
	gprs.TX = "";
	//gprs.RX ;
	gprs.RXOvf = false;
	gprs.RXPointer = &gprs.RX;

	gprs.txPosition = -1;
	gprs.okPosition = -1;
	gprs.errorPosition = -1;
	gprs.response = INIT_STATE;
	gprs.timeout = HAL_GetTick();
	gprs.timeoutOccured = false;
	gprs.balanceRequered = false;
	gprs.balanceReceived = false;
	gprs.voltage =0;
	gprs.charge = 0;
	gprs.balance = 0;
	gprs.subaddress = 0;
	gprs.smsCount=0;
	gprs.smsToRead=2;
	gprs.operator = "NO DATA";
	gprs.smsCount = 0;
	gprs.smsToRead = 2;
	gprs.specificRequest = false;
	gprs.subds = 0;
	//gprs.transmitRequered = false;
	//gprs.waitForResponse = false;
	gprs.busy = false;

	gprs.index = 0;
	gprs.i = 0;
	//gprs.readIndex = 0;

	gprs.at[AT_AT].request = "AT\r\n";
	gprs.at[AT_AT].response = INIT_STATE;

	gprs.at[AT_CPIN].request = "AT+CPIN?\r\n"; //..симка//5s
	gprs.at[AT_CPIN].response = INIT_STATE;

	gprs.at[AT_CSQ].request = "AT+CSQ\r\n"; //качество сигнала
	gprs.at[AT_CSQ].response = INIT_STATE;

	gprs.at[AT_CMGF].request = "AT+CMGF=1\r\n"; // Текстовый режим (не PDU)
	gprs.at[AT_CMGF].response = INIT_STATE;

	gprs.at[AT_CBC].request = "AT+CBC\r\n"; //напряжение питания
	gprs.at[AT_CBC].response = INIT_STATE;

	gprs.at[AT_CSPN].request = "AT+CSPN?\r\n"; // Чтение имени провайдера
	gprs.at[AT_CSPN].response = INIT_STATE;

	gprs.at[AT_RESPONSE_TIMEOUT].response = gprs.timeoutOccured;

	gprs.at[AT_CUSD].request = "AT+CUSD=1,\"#102#\"\r\n"; //, Request //Чтение баланса SIM-карты
	gprs.at[AT_CUSD].response = INIT_STATE;

	gprs.at[AT_CMGR].request = "AT+CMGR=2\r\n"; //read sms from mem
	gprs.at[AT_CMGR].response = INIT_STATE;
	gprs.at[AT_CMGR].args = "NO DATA";

	gprs.at[AT_CPMS].request = "AT+CPMS?\r\n"; //count of sms
	gprs.at[AT_CPMS].response = INIT_STATE;

	gprs.at[AT_CMGDA].request = "AT+CMGDA=\"DEL ALL\"\r\n"; //count of sms
	gprs.at[AT_CMGDA].response = INIT_STATE;
	/*AT.cops.request = "AT+COPS?\r\n";
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

	 AT.sapbrCONTYPE.request = "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n";
	 AT.sapbrCONTYPE.response = INIT_STATE;*/

	//AT.sapbrAPN.request = "";*/
	//"AT+CSQ\r\n" //качество сигнала
	/*queue[0] = "AT\r\n";
	 queue[1] = "AT+CSQ\r\n";
	 queue[2] = "AT+CMGF=1\r\n";
	 queue[3] = "AT+CMGS=\"+79518926260\"\r\n";
	 queue[4] = "hello\x1a\r\n";*/
	queue[0] = AT_AT;
	queue[1] = AT_CSQ;
	queue[2] = AT_CPIN;
	queue[3] = AT_CMGF;
	queue[4] = AT_CBC;
	queue[5] = AT_CSPN;
	queue[6] = AT_CPMS;
	queue[7] = AT_CMGR;

	HAL_UART_Receive_DMA(&huart1, gprs.rx_buff, RX_BUFFER_SIZE);
}
void sendQueue() {
	if (!gprs.busy) {
		gprs.busy = true;

		gprs.timeout = HAL_GetTick() + 5000;
//		gprs.TX = "AT\r\n";
//		gprs.TX = "AT+CPIN?\r\n"; //..симка//5s
//		gprs.TX = "AT+CFUN?\r\n"; //1=полная работоспособность//10s
//		gprs.TX = "AT+CBC\r\n"; //напряжение питания
//		gprs.TX = "AT+CSQ\r\n"; //качество сигнала
//		gprs.TX = "AT+CPAS\r\n"; // Ожидание готовности GSM-модуля и SIM-карты //2 Unknown (MT is not guaranteed to respond totructions)
//		//gprs.TX = "AT+CSPN?\r\n"; // Чтение имени провайдера
//		//gprs.TX = "AT+CSCS=\"GSM\"\r\n";
//		//gprs.TX = "AT+CREG=1\r\n";
//		//gprs.TX = "AT+CREG?\r\n"; //Тип регистрации в сети
//
//		//gprs.balanceRequered = true;
//		gprs.TX = "AT+CMGF=1\r\n"; // Текстовый режим (не PDU)
		//gprs.TX = "AT+CUSD=1,\"#102#\"\r\n"; //, Request //Чтение баланса SIM-карты //20s
		//gprs.subaddress = AT_CUSD;
		//gprs.balanceRequered = true;

		//gprs.TX = "AT+CMGF=1\r\n"; // Текстовый режим (не PDU)
		/*if(gprs.index==0)
		 gprs.TX = "AT+CMGS=\"+79063280423\"\r\n";//sms
		 if(gprs.index==1)
		 gprs.TX = "hello\x1a\r\n";*/

		//gprs.TX = "AT+CMGL=\"ALL\"\r\n";
		//gprs.TX = "AT+CMGR=2\r\n";
		//gprs.TX = "AT+CPMS?\r\n";//сколько смсок
		//gprs.TX = "AT+CPMS=\"MT\"\r\n";
		//gprs.TX = "AT+CCID\r\n";
		//gprs.TX = AT.cops.request;
		//gprs.TX = "hello\x1a";
		gprs.subaddress = queue[gprs.index];
		if (gprs.balanceRequered && !gprs.balanceReceived) {
			gprs.subaddress = AT_CUSD;
		}
		if(gprs.specificRequest)
			gprs.subaddress = gprs.subds;
		if (gprs.subaddress == AT_CMGR) {
			gprs.at[gprs.subaddress].request = atCMGRPrepare(gprs.smsToRead);
			//sprintf(gprs.at[gprs.subaddress].request, "AT+CMGR=%d\r\n", gprs.smsToRead);
			/*gprs.at[gprs.subaddress].request = "AT+CMGR=";
			 char* num;
			 itoa(gprs.smsToRead, num, 10);
			 strcat(gprs.at[gprs.subaddress].request, num);
			 strcat(gprs.at[gprs.subaddress].request, "\r\n");*/
		}
		gprs.TX = gprs.at[gprs.subaddress].request;
		HAL_UART_Transmit_DMA(&huart1, gprs.TX, strlen(gprs.TX));
	}
	/*if (HAL_GetTick() % 1000 == 0 && gprs.index < 2) {
	 gprs.index++;
	 gprs.busy=false;
	 }*/
	for (int i = 0; i < RX_BUFFER_SIZE; i++) {
		if (gprs.rx_buff[i] != '\0') {
			gprs.RX[gprs.i] = gprs.rx_buff[i];
			gprs.i++;
			if (gprs.i == 512)
				gprs.i = 0;
		}
		gprs.rx_buff[i] = '\0';
	}
	return;
}

void checkAT() {
	size_t len = strlen(gprs.TX); //namelen = strlen(name);
	//
	//har* copy;
	char* str2;
	if (len != 0) {
		str2 = (char*) malloc(len - 1);
		strncpy(str2, gprs.TX, len - 2);
		str2[len - 1] = 0;
		//free(str2);
	}

	gprs.txPosition = strpos(gprs.RXPointer, str2, 0);
	// printf ( "tx position: %d\n\n", txPosition );

	if (len != 0)
		free(str2);

	gprs.okPosition = strpos(gprs.RXPointer, "OK", 0);
	// printf ( "ok position: %d\n\n", okPosition );

	gprs.errorPosition = strpos(gprs.RXPointer, "ERROR", 0);
	// printf ( "error position: %d\n\n", errorPosition );

	if ((gprs.okPosition > 0 || gprs.errorPosition > 0)
			|| HAL_GetTick() > gprs.timeout) {
		gprs.timeoutOccured = false;
		//AT.at.response = gprs.response;
		gprs.response = (gprs.okPosition > 0) ? OK :
						(gprs.errorPosition > 0) ? ERROR : INIT_STATE;

		gprs.at[gprs.subaddress].response = gprs.response;

		if (gprs.response != ERROR) {
			if (gprs.subaddress == AT_CSQ) {
				char* found = strstr(gprs.RXPointer, "+CSQ: ") + 6;
				/*size_t len = strlen(found);
				 char* str2 = (char*) malloc(len + 1);
				 strcpy(str2, found);
				 gprs.at[gprs.subaddress].args = str2;*/
				gprs.quality = atoi(found);
			}
			if (gprs.subaddress == AT_CBC) {
				char* found = strstr(gprs.RXPointer, "+CBC: ") + 6;
				found = strstr(found, ",") + 1;
				gprs.charge = atoi(found);
				found = strstr(found, ",") + 1;
				gprs.voltage = atoi(found);
				/*size_t len = strlen(found);
				 char* str2 = (char*) malloc(len + 1);
				 strcpy(str2, found);
				 gprs.at[gprs.subaddress].args = str2;*/
			}
			if (gprs.subaddress == AT_CSPN) {
				char* found = strstr(gprs.RXPointer, "+CSPN: \"") + 8;
				//size_t quote = strstr(found, "\"") - found;
				size_t len = strlen(found);
				char* str2 = (char*) malloc(len + 1);
				strcpy(str2, found);
				//str2[len]=0;
				gprs.at[gprs.subaddress].args = str2;
				gprs.operator = str2;
			}
			if (gprs.subaddress == AT_CPMS) {
				char* found = strstr(gprs.RXPointer, "\"MT\",") + 5;
				gprs.smsCount = atoi(found);
			}
			if (gprs.subaddress == AT_CMGR) {
				char* found = strstr(gprs.RXPointer, "+CMGR: ") + 7;
				size_t len = strlen(found);
				char* str2 = (char*) malloc(len + 1);
				strcpy(str2, found);
				gprs.at[gprs.subaddress].args = str2;
			}
			if (gprs.subaddress == AT_CUSD) {
				/*char* found = strstr(gprs.RXPointer, "+CUSD: ") + 7;
				 found = strstr(found, ",\"") + 2;
				 //size_t quote = strstr(found, "\"") - found;
				 size_t len = strlen(found);
				 char* str2 = (char*) malloc(len + 1);
				 strcpy(str2, found);
				 //str2[len]=0;
				 gprs.at[gprs.subaddress].args = str2;

				 int length = strlen(str2);
				 char buffer[length / 2];
				 int i;
				 char buf = 0;
				 for (i = 0; i < length; i++) {
				 if (i % 2 != 0) {
				 buffer[i / 2] = hex_to_ascii(buf, str2[i]);
				 } else {
				 buf = str2[i];
				 }
				 }
				 gprs.balance = atof(&buffer);*/

				/*size_t len = strlen(gprs.RXPointer);
				 char* str2 = (char*) malloc(len + 1);
				 strcpy(str2, gprs.RXPointer);
				 gprs.at[gprs.subaddress].args = str2;*/

				//gprs.balanceRequered = false;
				//gprs.balanceReceived = true;
			}
		}
		if(gprs.specificRequest)
			gprs.specificRequest = false;
		if (gprs.balanceRequered && gprs.at[AT_CUSD].response == 1
				&& strpos(gprs.RXPointer, " r.", 0) > 0) {				// " p"
				//check +CUSD and calc balance
				//gprs.txPosition = -1;
			char* found = strstr(gprs.RXPointer, "balans ") + 7;
			//found = strstr(found + 7, "\" ")+2;
			gprs.balance = atof(found);
			/*size_t len = strpos(found, " r.", 0);
			 char* str2 = (char*) malloc(len + 1);
			 strcpy(str2, found);*/
			//gprs.at[gprs.subaddress].args = str2;
			gprs.balanceReceived = true;
			gprs.balanceRequered = false;
		}
		gprs.txPosition = -1;
		gprs.okPosition = -1;
		gprs.errorPosition = -1;
		gprs.response = INIT_STATE;
		if (!gprs.balanceRequered) {
			for (int i = 0; i < gprs.i; i++)
				gprs.RX[i] = 0;
			gprs.i = 0;
			gprs.busy = false;
			gprs.index++;
			if (gprs.index == 8)
				gprs.index = 0;
		}
	}
	if (HAL_GetTick() > gprs.timeout) {
		gprs.timeoutOccured = true;
		gprs.balanceRequered = false;
	}
	gprs.at[AT_RESPONSE_TIMEOUT].response = gprs.timeoutOccured;
	/*if (txPosition >= 0) {
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
		if (gprs.rx_buff[i] != '\0') {

			/*size_t len = strlen(gprs.RX);
			 if (len < 127) {
			 char* str2 = (char*) malloc(len + 1 + 1);
			 strcpy(str2, gprs.RX);
			 str2[len] = gprs.rx_buff[i];
			 str2[len + 1] = '\0';

			 gprs.RX = str2;
			 } else {
			 gprs.RX="";
			 gprs.RXOvf = true;
			 }*/
			gprs.RX[gprs.i] = gprs.rx_buff[i];
			gprs.i++;
			if (gprs.i == 128) {
				gprs.i = 0;
			}
			//free(str2);
		}
		gprs.rx_buff[i] = '\0';
		//gprs.rxChar[gprs.i] = gprs.rx_buff[i];
		//gprs.i++;
	}

	HAL_UART_Receive_DMA(&huart1, gprs.rx_buff, RX_BUFFER_SIZE);
}
void checkReceive() {
}

int hex_to_int(char c) {
	int first = c / 16 - 3;
	int second = c % 16;
	int result = first * 10 + second;
	if (result > 9)
		result--;
	return result;
}

int hex_to_ascii(char c, char d) {
	int high = hex_to_int(c) * 16;
	int low = hex_to_int(d);
	return high + low;
}
int strpos(char* hay, char* needle, int offset) {
	char haystack[strlen(hay)];
	strncpy(haystack, hay + offset, strlen(hay) - offset);
	char* p = strstr(haystack, needle);
	if (p)
		return p - haystack + offset;
	return -1;
}
char* atCMGRPrepare(uint8_t smsToRead_) {
	char num[10];
	itoa(smsToRead_, num, 10);
	unsigned int len = strlen(num);
	char* req = (char*) malloc(8 + len + 2 + 1);
	strcpy(req, "AT+CMGR=");
	strcat(req, num);
	strcat(req, "\r\n");
	req[8 + len + 2] = 0;
	return req;
}
#endif /* SIM800L_H_ */
