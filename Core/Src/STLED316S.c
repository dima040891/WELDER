#include "STLED316S.h"
//#include "stm32f103xe.h"



const uint8_t code_digit [] =						//Знакогенератор для семисегментного индикатора
{
	0x3f,	// 0
	0x06,	// 1
	0x5b,	// 2
	0x4f,	// 3
	0x66,	// 4
	0x6d,	// 5
	0x7d,	// 6
	0x07,	// 7
	0x7f,	// 8
	0x6f,	// 9
	0x00,	// Пустой разряд
	0x77,	// A
	0x00,	// C
	0x00,	// E
	0x00,	// F
	0x00,	// H
	0x00,	// L
	0x00,	// P
	0x00,	// S
	0x00,	// U
	0x10,	// LED_E (АВТО)
	0x20,	// LED_F (ПАРКОВКА)
	0x40,	// LED_G (ВНИЗ)
	0x80,	// LED_DP (ВВЕРХ)
};



void STLED316S_Ini (GPIO_TypeDef *STLED_PORT_STB, uint16_t STLED_PIN_STB)
{

	STLED316S_SetBrightness(7, STLED_PORT_STB, STLED_PIN_STB);


	STLED_TxData[0] = 0x0D; // Включить дисплей

	HAL_GPIO_WritePin(STLED_PORT_STB, STLED_PIN_STB, GPIO_PIN_RESET);
	DelayMicro(DELAY_US);
	HAL_SPI_Transmit(&hspi1, STLED_TxData, 1, 10000);
	DelayMicro(DELAY_US);
	HAL_GPIO_WritePin(STLED_PORT_STB, STLED_PIN_STB, GPIO_PIN_SET);
	DelayMicro(DELAY_US);

}


void STLED316S_OutData (uint8_t *data, uint8_t num, GPIO_TypeDef *STLED_PORT_STB, uint16_t STLED_PIN_STB)
{
	STLED_TxData[0] = 0x00; // Вывод с автоикрементом адреса

	STLED_TxData[1] = 0x00;
	STLED_TxData[2] = 0x00;
	STLED_TxData[3] = 0x00;
	STLED_TxData[4] = 0x00;
	STLED_TxData[5] = 0x00;
	STLED_TxData[6] = 0x00;

	for(uint8_t i = 0; i < num; i++)
	{
		STLED_TxData[i + 1] = code_digit[data[i]];
	}

	HAL_GPIO_WritePin(STLED_PORT_STB, STLED_PIN_STB, GPIO_PIN_RESET);
	DelayMicro(DELAY_US);
	HAL_SPI_Transmit(&hspi1, &STLED_TxData[0], num + 1, 10000);
	DelayMicro(DELAY_US);
	HAL_GPIO_WritePin(STLED_PORT_STB, STLED_PIN_STB, GPIO_PIN_SET);
	DelayMicro(DELAY_US);
}

void STLED316S_Single_Out (uint8_t *data, uint8_t num, GPIO_TypeDef *STLED_PORT_STB, uint16_t STLED_PIN_STB)
{
	STLED_TxData[0] = 0x00;
	STLED_TxData[1] = 0x00;

	STLED_TxData[0] |= 0x20; // Вывод значения без автоинкремента, в заданный разряд
	STLED_TxData[0] |= ((6-num) & 0x07); // В какой разряд выводить

	STLED_TxData[1] = *data; // Выводимое значение

	for(uint8_t i = 0; i < num; i++)
	{
		STLED_TxData[i + 1] = code_digit[data[i]];
	}

	HAL_GPIO_WritePin(STLED_PORT_STB, STLED_PIN_STB, GPIO_PIN_RESET);
	DelayMicro(DELAY_US);
	HAL_SPI_Transmit(&hspi1, &STLED_TxData[0], 2, 10000);
	DelayMicro(DELAY_US);
	HAL_GPIO_WritePin(STLED_PORT_STB, STLED_PIN_STB, GPIO_PIN_SET);
	DelayMicro(DELAY_US);
}

void STLED316S_Direct_Single_Out (uint8_t *data, uint8_t num, GPIO_TypeDef *STLED_PORT_STB, uint16_t STLED_PIN_STB)
{
	STLED_TxData[0] = 0x00;
	STLED_TxData[1] = 0x00;

	STLED_TxData[0] |= 0x20; // Вывод значения без автоинкремента, в заданный разряд
	STLED_TxData[0] |= ((6-num) & 0x07); // В какой разряд выводить

	STLED_TxData[1] = *data; // Выводимое значение

//	for(uint8_t i = 0; i < num; i++)
//	{
//		STLED_TxData[i + 1] = code_digit[data[i]];
//	}

	HAL_GPIO_WritePin(STLED_PORT_STB, STLED_PIN_STB, GPIO_PIN_RESET);
	DelayMicro(DELAY_US);
	HAL_SPI_Transmit(&hspi1, &STLED_TxData[0], 2, 10000);
	DelayMicro(DELAY_US);
	HAL_GPIO_WritePin(STLED_PORT_STB, STLED_PIN_STB, GPIO_PIN_SET);
	DelayMicro(DELAY_US);
}


void STLED316S_SetBrightness (uint8_t Bright, GPIO_TypeDef *STLED_PORT_STB, uint16_t STLED_PIN_STB)
{
	STLED_TxData[0] = 0x10;
	STLED_TxData[1] = 0xE5; // E5 - LSB2:0 - кол разряд (0b101 = 6 разрадам), LSB4:3 - режим яркости, LSB7:5 - яркость

	STLED_TxData[2] = 0x00;
	STLED_TxData[3] = 0x00;
	STLED_TxData[4] = 0x00;

	STLED_TxData[2] = (Bright<<4) | (Bright);
	STLED_TxData[3] = (Bright<<4) | (Bright);
	STLED_TxData[4] = (Bright<<4) | (Bright);


	HAL_GPIO_WritePin(STLED_PORT_STB, STLED_PIN_STB, GPIO_PIN_RESET);
	DelayMicro(DELAY_US);
	HAL_SPI_Transmit(&hspi1, &STLED_TxData[0], 5, 10000);
	DelayMicro(DELAY_US);
	HAL_GPIO_WritePin(STLED_PORT_STB, STLED_PIN_STB, GPIO_PIN_SET);
	DelayMicro(DELAY_US);

}

void STLED316S_Turn_ON_OFF (uint8_t OnOff, GPIO_TypeDef *STLED_PORT_STB, uint16_t STLED_PIN_STB)
{
	if (OnOff == 1)
	STLED_TxData[0] = 0x0D;

	if (OnOff == 0)
	STLED_TxData[0] = 0x0E;

	HAL_GPIO_WritePin(STLED_PORT_STB, STLED_PIN_STB, GPIO_PIN_RESET);
	DelayMicro(DELAY_US);
	HAL_SPI_Transmit(&hspi1, STLED_TxData, 1, 10000);
	DelayMicro(DELAY_US);
	HAL_GPIO_WritePin(STLED_PORT_STB, STLED_PIN_STB, GPIO_PIN_SET);
	DelayMicro(DELAY_US);
}

void STLED316S_ReciveData (uint8_t *data, GPIO_TypeDef *STLED_PORT_STB, uint16_t STLED_PIN_STB)
{
	STLED_TxData[0] = 0x49;
	STLED_TxData[1] = 0x00;
	STLED_TxData[2] = 0x00;
	STLED_TxData[3] = 0x00;

	STLED_Rx[0] = 0x49;
	STLED_Rx[1] = 0x02;
	STLED_Rx[2] = 0x02;
	STLED_Rx[3] = 0x02;

	for(uint8_t i = 0; i < 8; i++)
	{
		STLED_Rx[i] = 0;
	}

	HAL_GPIO_WritePin(STLED_PORT_STB, STLED_PIN_STB, GPIO_PIN_RESET);
	DelayMicro(DELAY_US);
	//SPI1->CR1 |= SPI_CR1_BIDIOE; // Перевод SPI в режим передачи
	HAL_SPI_Transmit(&hspi1, STLED_TxData, 1, 1000);
	//SPI1->CR1 &= ~SPI_CR1_BIDIOE; // // Перевод SPI в режим приема
	//HAL_SPI_TransmitReceive(&hspi1, STLED_TxData, STLED_Rx, 1, 1000);
	HAL_SPI_Receive(&hspi1, STLED_Rx, 2, 1000);
	HAL_GPIO_WritePin(STLED_PORT_STB, STLED_PIN_STB, GPIO_PIN_SET);
	DelayMicro(DELAY_US);

	data[0] = STLED_Rx[0];
	data[1]= STLED_Rx[1];

}

 void DelayMicro(uint32_t __IO micros)
{
  micros *=(SystemCoreClock/1000000)/5;
  while(micros--);
}

