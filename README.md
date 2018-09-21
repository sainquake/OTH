# features
* AT COMMANDS SIM800C/L https://www.elecrow.com/download/SIM800%20Series_AT%20Command%20Manual_V1.09.pdf
* SIM800C/L http://codius.ru/articles/GSM_%D0%BC%D0%BE%D0%B4%D1%83%D0%BB%D1%8C_SIM800L_%D1%87%D0%B0%D1%81%D1%82%D1%8C_1
* TIM4 for microseconds delay 
* TIM2 InputCapture Mode  for OpenTerm receive data 
* HUART3 for RPi dialog; Useing DMA for TX data
* HUART2 for 1-Wire DS18B20 temperature sensor

# todo

* PA9 add devider 3.3/(3.3+1)

* make openterm lib
* make coverstion with RPi
* make battery control

* f8.8 covert to float
* parity calc

* UNION usage for UART STM-RPi Protocol

* separate power for sim800l module

* HUART1  אשך SIM800L use DMA for TX

# electronics issue
* make R4 700 Ohm
* STM UART3 - RPi UART0

* PA8 Output
* PA15 Input

* Vin to SIM800L Vcc (4V)

* strhex to ascii
```
    const char* st = "00370037002E0030003200200440002E000A041E0431044904300439044204350441044C0020044100200434044004430433043E043C0020043F043E00200053004D00530020043104350441043F043B04";
            int length = strlen(st);
            int i;
            char buf = 0;
            for(i = 0; i < length; i++){
                    if(i % 2 != 0){
                            printf("%c", hex_to_ascii(buf, st[i]));
                    }else{
                            buf = st[i];
                    }
            }
```



* https://github.com/ihormelnyk/arduino_opentherm_controller - opentherm arduino example
*https://github.com/jpraus/arduino-opentherm/blob/master/src/opentherm.cpp - Parity Bit Function


# EEPROM MAP

* http://www.mcu.by/%D1%81%D1%82%D0%B0%D1%80%D1%82-arm-%D1%8D%D1%82%D0%B0-%D1%83%D0%B6%D0%BD%D0%B0%D1%81%D0%BD%D0%B0%D1%8F-flah-%D0%BE%D1%82-stm32-hal/
* https://stackoverflow.com/questions/45574246/hal-drivers-erase-read-write-flash-on-stm32f4-nucleo


# RPi bluetooth issue for Wi-Fi login password enter

* https://www.cnet.com/how-to/how-to-setup-bluetooth-on-a-raspberry-pi-3/
* https://github.com/EnableTech/raspberry-bluetooth-demo