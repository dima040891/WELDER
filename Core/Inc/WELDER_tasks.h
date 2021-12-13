#ifndef WELDER_TASKS_H
#define WELDER_TASKS_H

//#include "FreeRTOS.h"
//#include "semphr.h"

#include "main.h"
//#include "STLED316S.h"
//#include "PCB_WELDER.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "cmsis_os.h"
#include "semphr.h"

#include "STM32_flash.h"

void freeRTOS_Tasks_Ini (void);
void vCarriage_GoTo(void *pvParameters);
void vKeyScan(void *pvParameters);
void vIndicatorPanel_Out(void *pvParameters);
void vBuzzer_beep(void *pvParameters);
void vCarriage_Calibration(void *pvParameters);
void vKey_Action(void *pvParameters);
void vWelder_Run(void *pvParameters);

#define Valve_L_OPEN HAL_GPIO_WritePin(oSolenoidValve1_GPIO_Port, oSolenoidValve1_Pin, GPIO_PIN_RESET); // Поднять (освободить) левую часть зажима заготовки
#define Valve_L_CLOSE HAL_GPIO_WritePin(oSolenoidValve1_GPIO_Port, oSolenoidValve1_Pin, GPIO_PIN_SET); // Опустить (зажать) левую часть зажима заготовки

#define Valve_R_OPEN HAL_GPIO_WritePin(oSolenoidValve2_GPIO_Port, oSolenoidValve2_Pin, GPIO_PIN_RESET); // Поднять (освободить) правую часть зажима заготовки
#define Valve_R_CLOSE HAL_GPIO_WritePin(oSolenoidValve2_GPIO_Port, oSolenoidValve2_Pin, GPIO_PIN_SET);	// Опустить (зажать) правую часть зажима заготовки

#define WELDER_HEAD_UP HAL_GPIO_WritePin(oSol_Valve_a_Heade_GPIO_Port, oSol_Valve_a_Heade_Pin|oSol_Valve_b_Heade_Pin, GPIO_PIN_SET); // Поднять сваручную головку
#define WELDER_HEAD_DOWN HAL_GPIO_WritePin(oSol_Valve_a_Heade_GPIO_Port, oSol_Valve_a_Heade_Pin|oSol_Valve_b_Heade_Pin, GPIO_PIN_RESET); // Опустить сваручную головку

#define SYNC_ARC_ON HAL_GPIO_WritePin(oSyncArc_GPIO_Port, oSyncArc_Pin, GPIO_PIN_SET);	// Подача дуги
#define SYNC_ARC_OFF HAL_GPIO_WritePin(oSyncArc_GPIO_Port, oSyncArc_Pin, GPIO_PIN_RESET); // Отключение дуги

// Для поля WELDER_State.HolderState
#define WELDER_STATE_HOLDER_R 0x08 // Держатель заготвки правый закрыт (удерживает)
#define WELDER_STATE_HOLDER_L 0x04 // Держатель заготвки левый закрыт (удерживает)
#define WELDER_STATE_PEDAL_R 0x02 // Нажата педаль для зажима правой части заготовки
#define WELDER_STATE_PEDAL_L 0x01 // Нажата педаль для зажима левой части заготовки
#define WELDER_STATE_BACK_DOOR_CLOSE 0x10 // Признак закрытой задней дверцы

// Для поля WELDER_State.State
#define WELDER_STATE_CALIBRATED 0x02 // Аппарат откалиброван
#define WELDER_MOVE_ENABLE 0x20 // Разрешение движения каретки (битмаска)
#define WELDER_CLIBRATION_PROCESS 0x40 // Признак текущего выполнения калибровки, 1 - выполняется калибровка, 0- калибровка не выполняется

#define LED_AUTO 0x01
#define LED_PARKING 0x02
#define LED_DOWN 0x04
#define LED_UP 0x08

// Режим работы аппарата WELDER_State.Mode
#define WELDER_MODE_CALIBRATION 0x01 // Режим калибровки
#define WELDER_MODE_MANUAL 0x02 // Ручной режим
#define WELDER_MODE_AUTO 0x03 // Автоматический режим

#define KICKBACK 200 // Значение отката коретки после окончания варки, мм

#define CALIBRATION_PHASE_SPEED_1 600 // Скорость перемещения картеки при первой фазе калибровки (движение картеки на встречу концевику), см/мин
#define CALIBRATION_PHASE_SPEED_2 200 // Скорость перемещения картеки при второй фазе калибровки (движение картеки от концевика), см/мин
#define CALIBRATION_PHASE_SPEED_3 24 // Скорость перемещения картеки при третьей фазе калибровки (движение картеки на встречу концевику), см/мин



	xSemaphoreHandle xSemaphore_StepCount; // Декларирование переменной xSemaphore_StepCount, т.е. создание ссылки на будущий семафор

	xSemaphoreHandle xSemaphore_Calibration;

// Структура для вывода значений на информационное табло
typedef struct
{
	uint16_t Xs; 		// Точка начала варки (включение дуги), мм
	uint16_t Xf; 		// Точка конца варки (отключение дуги), мм
	uint8_t Delay_s; 	// Задержка перед включением дуги (заполнение аргоном), мС (Ст 2 разряда задейстованы, дискрет 100 мС. Значение 20 на индикаторе будет соответсововать 2000 мС задержки, 21 - 2100 мС задержки)
	uint8_t Delay_f; 	// Задержка после выключения дуги (заполнение аргоном), мС
	uint16_t Speed;		// Скорость движения каретки (дуги) при варке
	uint16_t Program; 	// Текущая программа варки. Обеспечивает варку по данным сохраненным во flash (Xs, Xf, Delay_s, Delay_f и Speed )
	uint8_t Brightness; //Яркость
	uint8_t KeyState[4]; // Состояния кнопок (1 - нажата, 0 - отпущена)
	uint8_t LEDsState;	// Состояние светодиодоы платы
} IndicatorValues;

IndicatorValues IndicatorPanel;

typedef struct
{
	uint16_t Xs; 		// Точка начала варки (включение дуги), мм
	uint16_t Xf; 		// Точка конца варки (отключение дуги), мм
	uint16_t Position; // Текущая позиция каретки, мм
	uint16_t Speed; // Заданная скорость варки, мм/мин
	uint16_t GoTo; // Занять позицию
	uint8_t Delay_s; 	// Задержка перед включением дуги (заполнение аргоном)
	uint8_t Delay_f; 	// Задержка после выключения дуги (заполнение аргоном), мС
	uint16_t Program; 	// Текущая программа варки. Обеспечивает варку по данным сохраненным во flash (Xs, Xf, Delay_s, Delay_f и Speed )
	uint16_t Steps; // Счетчик шагов ЩД. Используется для подсчета импульсов в обработчике прерываний EXTI
	uint8_t State; 	// 6 бит - выполняется калибровка (1) , (0) - не выполняется
					// 5 бит - разрешение на движение каретки (1) разрешено, (0) - запрещено
					// 4 бит - Признак закрытой задней дверцы  (1) - закрыта, (0) - открыта
					// 3 бит - Состояние вывода iCarriageStop (0) - коневик отжат, (1) - коневик нажат кареткой
					// 2 бит - Движение каретки. 0 - к концевику, 1 - от концевика
					// 1 бит - Не откалибровано (0) / Откалибровано (1);
					//0 бит - Простой (0) / Движение каретки (формирование ШИМ  сигнала для каретки) (1);
	uint8_t HolderState; // Состояние зажимов заготовки
	IndicatorValues IndicatorPanel;
	uint8_t BuzzerMode;
	uint8_t Mode; // Режим работы аппарата. Определят дейсвия по нажатию кнопки СТАРТ/СТОП
				  // 0x01 - Ручной режим
				  // 0x02 - Режим калибровки
				  // 0x03 - Автоматический режим
	uint8_t Calibration_level; // Стадия калибровки. 00 - калибровка не выполняется, 01 - движение каретки на встречу концевику, 02 - откат каретки назад,
								// 03 - медленное движенеи на встречу концевику, 04 - откалибровано (при этом устанавливается бит в State), 05 - калибровка остановлена

	uint8_t vWelder_Run_level;  // Стадия варки (задачи vWelder_Run_level). 00 - Задача не выполнятеся, 01 - движение каретки к точке начала варки,
								// 02 - опускание головки, 03 - выдерживание задержки перед началом варки, 04 - движение каретки к точке конца варки,
								// 05 - выдерживание задержки после окончания варки, 06 - подъем головки, 07 - откат каретки, остановка варки
}WELDER_State;

WELDER_State WelderUnit;

typedef struct
{
	uint8_t Xs_ShortPress_L; // Индикатор ВЛЕВО (Xs), кнопка "-"
	uint8_t Xs_ShortPress_R; // Индикатор ВЛЕВО (Xs), кнопка "-"
	uint8_t Xs_LongPress_L; // Индикатор ВЛЕВО (Xs), кнопка "+"
	uint8_t Xs_LongPress_R; // Индикатор ВЛЕВО (Xs), кнопка "+"

	uint8_t Xf_ShortPress_L; // Индикатор ВПРАВО (Xf), кнопка "-"
	uint8_t Xf_ShortPress_R; // Индикатор ВПРАВО (Xf), кнопка "-"
	uint8_t Xf_LongPress_L; // Индикатор ВПРАВО (Xf), кнопка "+"
	uint8_t Xf_LongPress_R; // Индикатор ВПРАВО (Xf), кнопка "+"

	uint8_t Delay_s_ShortPress_L; // Индикатор ЗАДЕРЖКА (Delay_s), кнопка "-"
	uint8_t Delay_s_ShortPress_R; // Индикатор ЗАДЕРЖКА (Delay_s), кнопка "-"
	uint8_t Delay_s_LongPress_L; // Индикатор ЗАДЕРЖКА (Delay_s), кнопка "+"
	uint8_t Delay_s_LongPress_R; // Индикатор ЗАДЕРЖКА (Delay_s), кнопка "+"

	uint8_t Speed_ShortPress_L; // Индикатор СКОРОСТЬ, кнопка "-"
	uint8_t Speed_ShortPress_R; // Индикатор СКОРОСТЬ, кнопка "-"
	uint8_t Speed_LongPress_L; // Индикатор СКОРОСТЬ, кнопка "+"
	uint8_t Speed_LongPress_R; // Индикатор СКОРОСТЬ, кнопка "+"

	uint8_t Delay_f_ShortPress_L; // Индикатор ЗАДЕРЖКА (Delay_f), кнопка "-"
	uint8_t Delay_f_ShortPress_R; // Индикатор ЗАДЕРЖКА (Delay_f), кнопка "-"
	uint8_t Delay_f_LongPress_L; // Индикатор ЗАДЕРЖКА (Delay_f), кнопка "+"
	uint8_t Delay_f_LongPress_R; // Индикатор ЗАДЕРЖКА (Delay_f), кнопка "+"

	uint8_t Program_ShortPress_L; // Индикатор ПРОШРАММА, кнопка "-"
	uint8_t Program_ShortPress_R; // Индикатор ПРОШРАММА, кнопка "-"
	uint8_t Program_LongPress_L; // Индикатор ПРОШРАММА, кнопка "+"
	uint8_t Program_LongPress_R; // Индикатор ПРОШРАММА, кнопка "+"

	uint8_t Program_ShortPress_L_and_R; // Индикатор ПРОШРАММА, кнопки "-" + "+"
	uint8_t Program_LongPress_L_and_R; // Индикатор ПРОШРАММА, кнопки "-" + "+"

	uint8_t Up_ShortPress; // Кнопка "ВВЕРХ"
	uint8_t Up_LongPress; // Кнопка "ВВЕРХ"

	uint8_t Down_ShortPress; // Кнопка "ВНИЗ"
	uint8_t Down_LongPress; // Кнопка "ВНИЗ"

	uint8_t Parking_ShortPress; // Кнопка "ПАРКОВКА"
	uint8_t Parking_LongPress;  // Кнопка "ПАРКОВКА"

	uint8_t Auto_ShortPress; // Кнопка "АВТО"
	uint8_t Auto_LongPress;  // Кнопка "АВТО"

	uint8_t StartStop_ShortPress; // Кнопка "СТАРТ/СТОП"
	uint8_t StartStop_LongPress;  // Кнопка "СТАРТ/СТОП"
}KeyPress;

KeyPress Key;

xQueueHandle qBeepMode; // Ссылка на очередь

xQueueHandle qWelderCmd; // Команды управления для задачи vCarriage_GoTo

xQueueHandle qGoToResponse; // Команды управления для задачи vCarriage_GoTo

xQueueHandle qWelderRun; // Команды управления для задачи vCarriageRun

xQueueHandle qWelderCalibrated; // Предача команды калибровка

xQueueHandle qKeyPress; // Очередь для предача события о нажатии кнопки в задачу реагирования (vKeyAction)

xQueueHandle qDebug; // Очередь для предача события о нажатии кнопки в задачу реагирования (vKeyAction)

enum {beep_off, beep_click, beep_1short, beep_2short, beep_3short, beep_long}; // Режимы работы бипера

enum {Cmd_CarriageStop, Cmd_CarriageGoTo, Carriage_Done, Welder_Run, Calibrated };

enum {noPress,
	press_short_Xs_L, press_long_Xs_L, press_short_Xs_R, press_long_Xs_R,
	press_short_Xf_L, press_long_Xf_L, press_short_Xf_R, press_long_Xf_R,
	press_short_Delay_s_L, press_long_Delay_s_L, press_short_Delay_s_R, press_long_Delay_s_R,
	press_short_Speed_L, press_long_Speed_L, press_short_Speed_R, press_long_Speed_R,
	press_short_Delay_f_L, press_long_Delay_f_L, press_short_Delay_f_R, press_long_Delay_f_R,
	press_short_Program_L, press_long_Program_L, press_short_Program_R, press_long_Program_R,
	press_short_Parking, press_long_Parking, press_short_Auto, press_long_Auto,
	press_short_StartStop, press_long_StartStop,
	press_short_Program_L_and_R, press_long_Program_L_and_R,
	press_short_Down, press_long_Down, press_short_Up, press_long_Up
};



#endif
