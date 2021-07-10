#ifndef STLED316S_H
#define STLED316S_H

#include "main.h"

#define DELAY_US 5




//#define STLED316S_ENABLE1 HAL_GPIO_WritePin(GPIOC, oSTLED316S_STB1_Pin, GPIO_PIN_RESET);
//#define STLED316S_DISABLE1 HAL_GPIO_WritePin(GPIOC, oSTLED316S_STB1_Pin, GPIO_PIN_SET);
//
//#define STLED316S_ENABLE2 HAL_GPIO_WritePin(GPIOC, oSTLED316S_STB2_Pin, GPIO_PIN_RESET);
//#define STLED316S_DISABLE2 HAL_GPIO_WritePin(GPIOC, oSTLED316S_STB2_Pin, GPIO_PIN_SET);
//
//#define STLED316S_ENABLE3 HAL_GPIO_WritePin(GPIOB, oSTLED316S_STB3_Pin, GPIO_PIN_RESET);
//#define STLED316S_DISABLE3 HAL_GPIO_WritePin(GPIOB, oSTLED316S_STB3_Pin, GPIO_PIN_SET);
//
//#define STLED316S_ENABLE4 HAL_GPIO_WritePin(GPIOB, oSTLED316S_STB4_Pin, GPIO_PIN_RESET);
//#define STLED316S_DISABLE4 HAL_GPIO_WritePin(GPIOB, oSTLED316S_STB4_Pin, GPIO_PIN_SET);
//
//#define STLED316S_ENABLE5 HAL_GPIO_WritePin(GPIOA, oSTLED316S_STB5_Pin, GPIO_PIN_RESET);
//#define STLED316S_DISABLE5 HAL_GPIO_WritePin(GPIOA, oSTLED316S_STB5_Pin, GPIO_PIN_SET);

extern SPI_HandleTypeDef hspi1;

uint8_t STLED_TxData[8]; // Данные непосредственно отправляемые STLED316S по SPI

uint8_t STLED_Rx[8]; // Данные непосредственно принимаемые от STLED316S по SPI

void STLED316S_Ini (GPIO_TypeDef *STLED_PORT_STB, uint16_t STLED_PIN_STB); // Инициализация STLED316S, параметры это порт и пин МК для подачи сигнала  STB

void STLED316S_OutData (uint8_t *data, uint8_t num, GPIO_TypeDef *STLED_PORT_STB, uint16_t STLED_PIN_STB); // Вывод значения на семисегментные индикаторы

void STLED316S_SetBrightness (uint8_t Bright, GPIO_TypeDef *STLED_PORT_STB, uint16_t STLED_PIN_STB); // Установка яркости свечения семисегментных индикаторов (от 0 до 7)

void STLED316S_Turn_ON_OFF (uint8_t OnOff, GPIO_TypeDef *STLED_PORT_STB, uint16_t STLED_PIN_STB); // Включенеи/Отключение семисегментных индикаторов

void STLED316S_ReciveData (uint8_t *data, GPIO_TypeDef *STLED_PORT_STB, uint16_t STLED_PIN_STB); // Опрос состояния кнопок

void DelayMicro(uint32_t __IO micros);

void STLED316S_Single_Out (uint8_t *data, uint8_t num, uint8_t DP, GPIO_TypeDef *STLED_PORT_STB, uint16_t STLED_PIN_STB); // Вывод одночного числа в заддный разряд

void STLED316S_Direct_Single_Out (uint8_t *data, uint8_t num, GPIO_TypeDef *STLED_PORT_STB, uint16_t STLED_PIN_STB); // Прямой вывод значения на индкаторы (без таблицы знакогенератора)


#endif
