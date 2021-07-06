#ifndef MICROSTEP_DRIVER_H
#define MICROSTEP_DRIVER_H

#include "main.h"
#include "PCB_WELDER.h"

#define MICRO_STEP_DEV 1 // Режим микрошага драйвера ШД (1, 2, 4, 8, 16 ...)


extern TIM_HandleTypeDef htim3;

void MicrostepDriver_Ini(void); // Инициализация ШИМ таймера 3 для работы с драйвером шагового двигателя

// Формирование выходной частоты драйвера ШД для его вращения. Параметры:
// Выходная частота драйвера ШД, Гц
// Порт и пин разрешения работы драйвера ШД
// Порт и пин определяющий направления вращаения ШД
// Номер канала ШИМ таймера 3
void MicrostepDriver_Run (uint16_t freq, uint8_t dir, GPIO_TypeDef *EN1_Port, uint16_t EN1_Pin, GPIO_TypeDef *DIR1_Port, uint16_t DIR1_Pin, uint32_t PWM_Channel);






#endif
