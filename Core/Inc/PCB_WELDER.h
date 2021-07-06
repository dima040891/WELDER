#ifndef PCB_WELDER_H
#define PCB_WELDER_H

#include "main.h"
#include "WELDER_tasks.h"
#include "STLED316S.h"
#include "MicrostepDriver.h"



#define STRIDE_ANGLE 1.8 // Угловой шаг ШД
#define STEPS_PER_ROTATE (360 / STRIDE_ANGLE) // Шагов на поворот ШД
//#define GEAR_TRAIN 27 // Передаточное отношение от вала ШД к приводу каретки

#define DISTANCE_PER_ROTATE 60 // Смещение каретки на 1 оборот ШД, мм


#define TIME_KEY_SCAN 20 // Период опроса клавиатуры, мС
#define TIME_KEY_SHORT_PRESS 80 // Время короткого нажатия на кнопку, мС
#define TIME_KEY_LONG_PRESS 1000 // Время короткого нажатия на кнопку, мС

#define CNT_KEY_SHORT (TIME_KEY_SHORT_PRESS / TIME_KEY_SCAN) // Для счетика определения короткого нажатия
#define CNT_KEY_LONG (TIME_KEY_LONG_PRESS / TIME_KEY_SCAN) // // Для счетика определения длинного нажатия
#define CNT_LONG_PRESS 37 // Определяет на сколько быстро будет увеличиваться значения при долгом удержании (до какой величины сбрасывается счетчик), у.е.

#define BUZZER_ON HAL_GPIO_WritePin(oBuzer_GPIO_Port, oBuzer_Pin, GPIO_PIN_SET);
#define BUZZER_OFF HAL_GPIO_WritePin(oBuzer_GPIO_Port, oBuzer_Pin, GPIO_PIN_RESET);





//#define Valve2_OPEN HAL_GPIO_WritePin(GPIOB, oSolenoidValve2_Pin, GPIO_PIN_SET);
//#define Valve2_CLOSE HAL_GPIO_WritePin(GPIOB, oSolenoidValve2_Pin, GPIO_PIN_RESET);

#define XS_MIN 10 // Мимальное значение значение хода каретки, мм
#define XS_MAX 1600 // Максимальное значение значение хода каретки, мм

#define XF_MIN 20 // Мимальное значение значение хода каретки, мм
#define XF_MAX 1650 // Максимальное значение значение хода каретки, мм

#define DELAY_S_MIN 1  // Мимальное значение значение временной задержки Delay_s, мС * 100
#define DELAY_S_MAX 50 // Мимальное значение значение временной задержки Delay_s, мС * 100

#define DELAY_F_MIN 2  // Мимальное значение значение временной задержки Delay_f, мС * 100
#define DELAY_F_MAX 70 // Мимальное значение значение временной задержки Delay_f, мС * 100

#define SPEED_MIN 1 // Минимальное значение скорости движения каретки, мм/мин
#define SPEED_MAX 999 // Максимальное значение скорости движения каретки, мм/мин

#define PROGRAM_MIN 1 // Минимальный номер программы
#define PROGRAM_MAX 70 // Максимальный номер номер программы



/*

ВЛЕВО: [0], "-" - 128, "+" - 64, , STLED №5;
ВПРАВО: [0], "-" - 16, "+" - 32, , STLED №5;
ЗАДЕРЖКА (Delay_s): [0], "-" - 8, "+" - 4, , STLED №5;
СКОРОСТЬ: [0], "-" - 2, "+" - 1, , STLED №5;
ЗАДЕРЖКА (Delay_f): [1], "-" - 1, "+" - 4, , STLED №5;
ПРОГРАММА: [1], "-" - 1, STLED №3;
ПРОГРАММА: [1], "-" - ..., "+" - 2, , STLED №5;

ВВЕРХ, ВНИЗ: [1], "-" - 128, "+" - 64, , STLED №5;
ПАРКОВКА, АВТО: [1], "-" - 32, "+" - 16, , STLED №5;

СТАРТ/СТОП: [1], 8, STLED №5;

*/



uint8_t STLED_digVal[6];
//uint8_t PCB_KeyState[6];

uint64_t ReadFlashPage[256];

//enum {PCB_LED_Off = 10, PCB_LED_AUTO = 20, PCB_LED_PARKING, PCB_LED_DOWN, PCB_LED_UP, PCB_LED_AUTOpPRKING, PCB_LED_AUTOpDOWN, PCB_LED_AUTOpPRKINGpDOWN, PCB_LED_PRKINGpDOWN, PCB_LED_PRKINGpUP};

void IndicatorPanel_Ini (void); // Инициализация табло управления и контроля

void IndicatorPanel_SetBrightness (IndicatorValues *IndicatorPanel0); // Установка яроксти табло управления и контроля

void IndicatorPanel_SetValue (IndicatorValues *IndicatorPanel0); // Вывод значенийна табло управления и контроля

int32_t separate (uint16_t in_data, uint8_t *out_data); // Ф-я разделения числа по разрядам

void IndicatorPanel_offset_4dig (uint8_t num, uint8_t *dataArray); // Упаковка пакета данных (сдвиг разрядов) с учетом разрядности и схемы поделючения 7 сегм. инкатора

void IndicatorPanel_offset_2dig (uint8_t num, uint8_t *dataArray); // Упаковка пакета данных (сдвиг разрядов) с учетом разрядности и схемы поделючения 7 сегм. инкатора

void IndicatorPanel_offset_3dig (uint8_t num, uint8_t *dataArray); // Упаковка пакета данных (сдвиг разрядов) с учетом разрядности и схемы поделючения 7 сегм. инкатора

void Carriage_Move (uint16_t Speed, uint8_t Dir, uint8_t NumSM); // Движение каретки с заданной скоростью, мм/мин.

void PCB_KeyScan (void);

//void SaveProgramToFlash();

void SaveProgramToFlash2();

void PCB_InputsScan (void);

void ReadProgramFromFlash2(void);

void PCB_LEDs_OUT (uint8_t LEDs_val);

#endif
