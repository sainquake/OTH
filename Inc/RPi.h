/*
 * RPi.h
 *
 *  Created on: 12 сент. 2018 г.
 *      Author: k.burdinov
 */

#ifndef RPI_H_
#define RPI_H_


#include "stm32f1xx_hal.h"

extern UART_HandleTypeDef huart3;

#define RPI_BUFFER_SIZE 4
//toRPIlib
#define RPi_ECHO_UART_ADDRESS           1
#define RPi_BLINK_UART_ADDRESS			2
#define RPi_OT_UART_ADDRESS				3
#define RPi_DS18B20_UART_ADDRESS		4
#define RPi_SIM800L_UART_ADDRESS		5
#define RPi_ADC_UART_ADDRESS			6
//#define RPi

#define RPi_SET_TEMP_UART_ADDRESS       10
#define RPi_GET_HW_TEMP_UART_ADDRESS    11
//toRPIlib
typedef struct {
	uint16_t address:16;
	uint16_t data:16;
} RPiFrameStruct;

union RPiFrameUnion {
	uint8_t raw[RPI_BUFFER_SIZE];
	RPiFrameStruct frame;
} ;
union RPiFrameUnion rpiframe = {0};

typedef struct{
	uint16_t address;
	uint8_t data;
	uint8_t rx_buff[10];
	uint8_t tx_buff[10];
	char 	rxed;
}SPI_Struct;
SPI_Struct RPi_SPI;

typedef struct{
	bool transmitRequered;
	uint16_t address;
	uint8_t data;
	uint8_t rx_buff[RPI_BUFFER_SIZE];
	uint8_t tx_buff[RPI_BUFFER_SIZE];
}UART_Struct;
UART_Struct RPi_UART;

void RPiInit(void);
void RPiRXRoute(void);
void RPiRoute(void);

void RPiInit(void){
	  HAL_UART_Receive_IT(&huart3,RPi_UART.rx_buff,RPI_BUFFER_SIZE);
	  RPi_UART.transmitRequered = false;
	return;
}
void RPiRoute(void){
	if(RPi_UART.transmitRequered){
			  HAL_UART_Transmit(&huart3,RPi_UART.tx_buff,RPI_BUFFER_SIZE,1000);
			  RPi_UART.transmitRequered = false;
		  }
}
//toRPIlib
void RPiRXRoute(void){
    for (int i=0;i<RPI_BUFFER_SIZE;i++)
        rpiframe.raw[i] = RPi_UART.rx_buff[i];

    switch(rpiframe.frame.address){
    case RPi_ECHO_UART_ADDRESS:
        HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
        //frame.frame.address = 1;
        //if(!frame.frame.read)
            for (int i=0;i<4;i++)
                RPi_UART.tx_buff[i] = rpiframe.raw[i];
        //else
        //	for (int i=0;i<4;i++)
        //		RPi_UART.tx_buff[i] = 0;
        RPi_UART.transmitRequered = true;
        break;
    case 2:
        ot.RPiRequestHI = rpiframe.frame.data;
        break;
    case 3:
        ot.RPiRequestLO = rpiframe.frame.data;
        break;
    case 4:

        HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
        //ot.RPiResponseHI = frame.frame.data;
        break;
    case 5:

        HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
        //ot.RPiResponseLO= frame.frame.data;
        break;
    case 6:
        //ot.RPiRequestHI = frame.frame.data;
        break;
    case RPi_SET_TEMP_UART_ADDRESS:
        OTCommon.targetTemp = rpiframe.frame.data;
        //rpiframe.frame.data = 1;
        for (int i=0;i<4;i++)
            RPi_UART.tx_buff[i] = rpiframe.raw[i];

        RPi_UART.transmitRequered = true;
        break;
    case RPi_GET_HW_TEMP_UART_ADDRESS:
        rpiframe.frame.data = temp.out;
        for (int i=0;i<4;i++)
            RPi_UART.tx_buff[i] = rpiframe.raw[i];
        RPi_UART.transmitRequered = true;
        break;
    }

    if(RPi_UART.rx_buff[0]=='H' && RPi_UART.rx_buff[1]=='e' && RPi_UART.rx_buff[2]=='l'){
        HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
    }
    if(RPi_UART.rx_buff[0]=='t' && RPi_UART.rx_buff[1]=='m' && RPi_UART.rx_buff[2]=='p'){
        HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
        //RPi_UART.tx_buff
    }
    HAL_UART_Receive_IT(&huart3,RPi_UART.rx_buff,RPI_BUFFER_SIZE);
}

#endif /* RPI_H_ */
