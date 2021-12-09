#include "WELDER_tasks.h"
#include "PCB_WELDER.h"

	uint8_t STLED_Tx[4];
	uint8_t STLED_Rx[8];



void freeRTOS_Tasks_Ini (void)
{
	xSemaphore_StepCount = xQueueCreateCountingSemaphore(4, 0);

	//xSemaphore_Calibration = xQueueCreateCountingSemaphore(1, 0); // Первый аргмент - это максимальное количество счета, второй - начальное значение счетчика

	vSemaphoreCreateBinary(xSemaphore_Calibration); // Создание двоичного семафора

	qBeepMode = xQueueCreate(1, sizeof( uint8_t )); // Очередь для передачи режима работы бипера

	qWelderCmd = xQueueCreate(1, sizeof( uint8_t )); // Очередь для передачи команд управления кареткой

	qGoToResponse = xQueueCreate(1, sizeof( uint8_t )); // Требуемая позиция занята

	//vSemaphoreCreateBinary(qDebug);

	qWelderRun = xQueueCreate(1, sizeof( uint8_t )); // Начать процесс варки

	qWelderCalibrated = xQueueCreate(1, sizeof( uint8_t )); // Начать калибровку

	qKeyPress = xQueueCreate(1, sizeof( uint8_t )); // Предача события о нажатии кнопки

	MicrostepDriver_Ini();

//	WELDER_Preset();

	xTaskCreate(vIndicatorPanel_Out, "IndicatorPanel_Out", 250, NULL, 2, NULL); // З-а вывода значений на панель

	xTaskCreate(vKeyScan, "KeyScan", 200, NULL, 2, NULL); // З-а опроса кнопок

	xTaskCreate(vBuzzer_beep, "Buzzer_beep", 200, NULL, 1, NULL); // З-а опроса кнопок

	xTaskCreate(vWelder_Run, "Weleder_Run", 200, NULL, 2, NULL);

	xTaskCreate(vCarriage_GoTo, "Carriage_GoTo", 200, NULL, 3, NULL); // З-а перемещения каретки в заданную точку

	xTaskCreate(vCarriage_Calibration, "Carriage_Calibration", 200, NULL, 2, NULL); // З-а перемещения каретки в заданную точку

	xTaskCreate(vKey_Action, "Key_Action", 200, NULL, 2, NULL); // З-а выполняет дейсвия в зависимости от нажатой кнопки и режима работы устройсво

	vTaskStartScheduler();

}

void vKey_Action(void *pvParameters)
{
	uint8_t KeyPressed; // Номер нажатой кнопки или комбинации кнопок
	uint8_t beep_mode_send = beep_click; // Режим работы бипера
	uint8_t Cal = 0; // Передача команды калибровка
	uint8_t Run; // Переменная для передачи команды начало варки
	uint8_t Carriage_cmd = Cmd_CarriageGoTo; //Передача команды для начала движения каретки в заданную точку


	for(;;)
	{
		xQueueReceive(qKeyPress, &KeyPressed, portMAX_DELAY ); // Ждать нажатия кнопки

		switch(KeyPressed)
		{
		case noPress:
		{
			break;
		}

		case press_short_Xs_L:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия

			if (WelderUnit.Xs > XS_MIN) // Проверка на попадание в допустимый диапазон значений
			{
				WelderUnit.Xs--; // Исполнить

				// Если ручной режим и аппарат откалиброван, то двигать каретку при одиночных нажатиях кнопок "-", "+"
				if ((WelderUnit.Mode == WELDER_MODE_MANUAL) && (WelderUnit.State & WELDER_STATE_CALIBRATED))
				{
					Carriage_cmd = Cmd_CarriageGoTo;
					WelderUnit.GoTo = WelderUnit.Xs;
					xQueueSendToBack( qWelderCmd, &Carriage_cmd, 0 ); // Идити к

				}
			}
			break;
		}

		case press_long_Xs_L:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if (WelderUnit.Xs > XS_MIN + 20) // Проверка на попадание в допустимый диапазон значений
			{
				WelderUnit.Xs -= 20; // Дейсвия при длинном нажатии
			}
			break;
		}

		case press_short_Xs_R:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if ((WelderUnit.Xs < XS_MAX) && (WelderUnit.Xs < WelderUnit.Xf)) // Проверка на попадание в допустимый диапазон значений  и что бы Xs была меньше Xf
			{

			WelderUnit.Xs++; // Исполнить

			// Если ручной режим и аппарат откалиброван, то двигать каретку при одиночных нажатиях кнопок "-", "+"
			if ((WelderUnit.Mode == WELDER_MODE_MANUAL) && (WelderUnit.State & WELDER_STATE_CALIBRATED))
			{
				Carriage_cmd = Cmd_CarriageGoTo;
				WelderUnit.GoTo = WelderUnit.Xs;
				xQueueSendToBack( qWelderCmd, &Carriage_cmd, 0 ); // Идити к
			}

			}
			break;
		}

		case press_long_Xs_R:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if (WelderUnit.Xs < XS_MAX - 20 && WelderUnit.Xs < WelderUnit.Xf - 20) // Проверка на попадание в допустимый диапазон значений и что бы Xs была меньше Xf
			{
				WelderUnit.Xs += 20; // Дейсвия при длинном нажатии
			}
			break;
		}

		case press_short_Xf_L:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if (WelderUnit.Xf > XF_MIN && WelderUnit.Xf > WelderUnit.Xs) // Проверка на попадание в допустимый диапазон значений
			{
				WelderUnit.Xf--; // Исполнить
				WelderUnit.GoTo = WelderUnit.Xf;

				// Если ручной режим и аппарат откалиброван, то двигать каретку при одиночных нажатиях кнопок "-", "+"
				if ((WelderUnit.Mode == WELDER_MODE_MANUAL) && (WelderUnit.State & WELDER_STATE_CALIBRATED))
				{
					Carriage_cmd = Cmd_CarriageGoTo;
					WelderUnit.GoTo = WelderUnit.Xf;
					xQueueSendToBack( qWelderCmd, &Carriage_cmd, 0 ); // Идити к

				}
			}
			break;
		}

		case press_long_Xf_L:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if (WelderUnit.Xf > XF_MIN + 20 && WelderUnit.Xf > WelderUnit.Xs + 20 ) // Проверка на попадание в допустимый диапазон значений и что бы Xs была меньше Xf
			{
				WelderUnit.Xf -= 20; // Дейсвия при длинном нажатии
				WelderUnit.GoTo = WelderUnit.Xf;
			}
			break;
		}

		case press_short_Xf_R:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if (WelderUnit.Xf < XF_MAX ) // Проверка на попадание в допустимый диапазон значений
			{
				WelderUnit.Xf++; // Исполнить
				WelderUnit.GoTo = WelderUnit.Xf;

				// Если ручной режим и аппарат откалиброван, то двигать каретку при одиночных нажатиях кнопок "-", "+"
				if ((WelderUnit.Mode == WELDER_MODE_MANUAL) && (WelderUnit.State & WELDER_STATE_CALIBRATED))
				{
					Carriage_cmd = Cmd_CarriageGoTo;
					WelderUnit.GoTo = WelderUnit.Xf;
					xQueueSendToBack( qWelderCmd, &Carriage_cmd, 0 ); // Идити к

				}
			}
			break;
		}

		case press_long_Xf_R:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if (WelderUnit.Xf < XF_MAX - 20 ) // Проверка на попадание в допустимый диапазон значений
			{
				WelderUnit.Xf += 20; // Дейсвия при длинном нажатии
				WelderUnit.GoTo = WelderUnit.Xf;
			}
			break;

		}

		case press_short_Delay_s_L:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if (WelderUnit.Delay_s > DELAY_S_MIN) // Проверка на попадание в допустимый диапазон значений
			{
				WelderUnit.Delay_s--; // Исполнить
			}
			break;
		}

		case press_long_Delay_s_L:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if (WelderUnit.Delay_s > DELAY_S_MIN + 10) // Проверка на попадание в допустимый диапазон значений
			{
				WelderUnit.Delay_s -= 10; // Дейсвия при длинном нажатии
			}
			break;
		}

		case press_short_Delay_s_R:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if (WelderUnit.Delay_s < DELAY_S_MAX ) // Проверка на попадание в допустимый диапазон значений
			{
				WelderUnit.Delay_s++; // Исполнить
			}
			break;
		}

		case press_long_Delay_s_R:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if (WelderUnit.Delay_s < DELAY_S_MAX - 10 ) // Проверка на попадание в допустимый диапазон значений
			{
				WelderUnit.Delay_s += 10; // Дейсвия при длинном нажатии
			}
			break;
		}

		case press_short_Speed_L:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if (WelderUnit.Speed > SPEED_MIN) // Проверка на попадание в допустимый диапазон значений
			{
				WelderUnit.Speed--; // Исполнить
			}
			break;
		}

		case press_long_Speed_L:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if (WelderUnit.Speed > SPEED_MIN + 20) // Проверка на попадание в допустимый диапазон значений
			{
				WelderUnit.Speed -= 20; // Дейсвия при длинном нажатии
			}
			break;
		}

		case press_short_Speed_R:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if (WelderUnit.Speed < SPEED_MAX ) // Проверка на попадание в допустимый диапазон значений
			{
				WelderUnit.Speed++; // Исполнить
			}
			break;
		}

		case press_long_Speed_R:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if (WelderUnit.Speed < SPEED_MAX- 20 ) // Проверка на попадание в допустимый диапазон значений
			{
				WelderUnit.Speed += 20; // Дейсвия при длинном нажатии
			}
			break;
		}

		case press_short_Delay_f_L:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if (WelderUnit.Delay_f > DELAY_F_MIN) // Проверка на попадание в допустимый диапазон значений
			{
				WelderUnit.Delay_f--; // Исполнить
			}
			break;
		}

		case press_long_Delay_f_L:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if (WelderUnit.Delay_f > DELAY_F_MIN + 10) // Проверка на попадание в допустимый диапазон значений
			{
				WelderUnit.Delay_f -= 10; // Дейсвия при длинном нажатии
			}
			break;
		}

		case press_short_Delay_f_R:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if (WelderUnit.Delay_f < DELAY_F_MAX) // Проверка на попадание в допустимый диапазон значений
			{
				WelderUnit.Delay_f++; // Исполнить
			}
			break;
		}

		case press_long_Delay_f_R:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if (WelderUnit.Delay_f < DELAY_F_MAX - 10 ) // Проверка на попадание в допустимый диапазон значений
			{
				WelderUnit.Delay_f += 10; // Дейсвия при длинном нажатии
			}
			break;
		}

		case press_short_Program_L:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if (WelderUnit.Program > PROGRAM_MIN) // Проверка на попадание в допустимый диапазон значений
			{
				WelderUnit.Program--; // Исполнить
				ReadProgramFromFlash2();
			}
			break;
		}

		case press_long_Program_L:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if (WelderUnit.Program > PROGRAM_MIN + 20) // Проверка на попадание в допустимый диапазон значений
			{
				WelderUnit.Program -= 20; // Дейсвия при длинном нажатии
			}
			break;
		}

		case press_short_Program_R:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 );
			if (WelderUnit.Program < PROGRAM_MAX) // Проверка на попадание в допустимый диапазон значений
			{
				WelderUnit.Program++; // Исполнить
				ReadProgramFromFlash2(); //
				//SaveProgramToFlash2();
			}
			break;
		}

		case press_long_Program_R:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			if (WelderUnit.Program < PROGRAM_MAX- 20 ) // Проверка на попадание в допустимый диапазон значений
			{
				WelderUnit.Program += 20; // Дейсвия при длинном нажатии
			}
			break;
		}


		case press_short_Down:
				{
					xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
					if ((WelderUnit.State & WELDER_STATE_BACK_DOOR_CLOSE) && WelderUnit.Mode == WELDER_MODE_MANUAL) // Если задняя дверца закрыта и режим работы аппарата ручной, то разрешить опускание головки.
					{
						WELDER_HEAD_DOWN // Опустить сварочную головку
						WelderUnit.IndicatorPanel.LEDsState |= LED_DOWN; // Индикация что головка опущена
						WelderUnit.IndicatorPanel.LEDsState &= ~LED_UP; // Индикация что головка опущена
					}

					break;
				}

				case press_long_Down:
				{
					xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
//					if (WelderUnit.Speed > SPEED_MIN + 20) // Проверка на попадание в допустимый диапазон значений
//					{
//						WelderUnit.Speed -= 20; // Дейсвия при длинном нажатии
//					}
					break;
				}

				case press_short_Up:
				{
					xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
					if ((WelderUnit.State & WELDER_STATE_BACK_DOOR_CLOSE) && WelderUnit.Mode == WELDER_MODE_MANUAL) // Если задняя дверца закрыта и режим работы аппарата ручной, то разрешить подъем головки
					{
					WELDER_HEAD_UP // Поднять сварочную головку
					WelderUnit.IndicatorPanel.LEDsState |= LED_UP; // Индикация что головка поднята
					WelderUnit.IndicatorPanel.LEDsState &= ~LED_DOWN; // Индикация что головка поднята
					}
					break;
				}

				case press_long_Up:
				{
					xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
					if (WelderUnit.Speed < SPEED_MAX - 20 ) // Проверка на попадание в допустимый диапазон значений
					{
						WelderUnit.Speed += 20; // Дейсвия при длинном нажатии
					}
					break;
				}



		case press_short_Parking:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия

			WelderUnit.Mode = WELDER_MODE_CALIBRATION; //Задать режим работы аппарата - калибровка

			//WelderUnit.IndicatorPanel.LEDsState &= ~LED_AUTO; // Отключении индикации режима АВТО
			WelderUnit.IndicatorPanel.LEDsState |= LED_PARKING; // Отображение что выбран режим калибровки

			break;
		}

		case press_long_Parking:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			WelderUnit.Xf -= 20; // Дейсвия при длинном нажатии

			break;
		}

		case press_short_Auto:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 );


			if (WelderUnit.State & WELDER_STATE_CALIBRATED) // Если аппарат откалиброван, то разрешить включение режима АВТО
			{
				if (WelderUnit.Mode != WELDER_MODE_AUTO) // Если уже не выбран режим АВТО, то включить его, иначе включить Ручной режим
				{
					WelderUnit.Mode = WELDER_MODE_AUTO;
					WelderUnit.IndicatorPanel.LEDsState |= LED_AUTO; // Отображение что выбран режим калибровки
				}
				else
				{
					WelderUnit.Mode = WELDER_MODE_MANUAL;
					WelderUnit.IndicatorPanel.LEDsState &= ~LED_AUTO; // Отображение что выбран режим калибровки
				}

			}

			break;
		}

		case press_long_Auto:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия
			WelderUnit.Xf += 20; // Дейсвия при длинном нажатии
			break;
		}

		case press_short_StartStop:
		{
			xQueueSendToBack( qBeepMode, &beep_mode_send, 0 ); // Звук нажатия

			// Дейсвия при нажатии на кнопку СТАРТ/СТОП в зависимости от установленного режима работы аппарата
			switch(WelderUnit.Mode)
			{
			case WELDER_MODE_CALIBRATION:
			{
				if (WelderUnit.State & WELDER_CLIBRATION_PROCESS)
				{
					WelderUnit.State &= ~WELDER_CLIBRATION_PROCESS; // Запретить калибровку

					if (WelderUnit.Calibration_level == 02)
					{
						Carriage_Move(0, 0, 1); // Стоп
						WelderUnit.Calibration_level = 05; // Калибровка остановлена
					}

					WelderUnit.Calibration_level = 05; // Калибровка остановлена


					xSemaphoreGive(xSemaphore_Calibration);
					vTaskDelay(10);
					xSemaphoreGive(xSemaphore_Calibration);
					vTaskDelay(10);
					xSemaphoreGive(xSemaphore_Calibration);
					vTaskDelay(10);
					xSemaphoreGive(xSemaphore_Calibration);
					vTaskDelay(10);

				}
				else
				{
					WelderUnit.State |= WELDER_CLIBRATION_PROCESS; // Разрешить калибровку
					Cal = Calibrated;
					xQueueSendToBack( qWelderCalibrated, &Cal, 0 ); // Начать калибровку
				}

				break;
			}

			case WELDER_MODE_MANUAL:
			{



				Run = Welder_Run;

				if (WelderUnit.State & 0x01) // Если каретка уже движеся, то остановать её (остановка варки)
				{

					WelderUnit.State &= ~WELDER_MOVE_ENABLE; // Заппретить движение каретки

					SYNC_ARC_OFF // Прекращение подачи дуги

					WELDER_HEAD_UP // Поднять головку
					WelderUnit.IndicatorPanel.LEDsState |= LED_UP; // Индикация поднятой сварочной головки
					WelderUnit.IndicatorPanel.LEDsState &= ~LED_DOWN;

					// Далее идет очистка очередей задачи vWelder_Run. Задержка необхадима для переключения контеста задачи
					Run = Carriage_Done;
					xQueueSendToBack(qGoToResponse, &Run, 0 );
					vTaskDelay(10);
					xQueueSendToBack(qGoToResponse, &Run, 0 );
					vTaskDelay(10);
					xQueueSendToBack(qGoToResponse, &Run, 0 );
					vTaskDelay(10);
					xQueueSendToBack(qGoToResponse, &Run, 0 );
					vTaskDelay(10);
					xQueueSendToBack(qGoToResponse, &Run, 0 );
					vTaskDelay(10);


					xQueueReset(qGoToResponse); // Сброс очереди в исходное состояние


				}
				else // Иначе начать варку
				{

					//UBaseType_t Q;

					if (!(WelderUnit.State & WELDER_MOVE_ENABLE)) // Если движение было запрещено (остановлено), то просто разрешить движение каретки
					{
					WelderUnit.State |= WELDER_MOVE_ENABLE; // Разрешить движение каретки
					}
					else // иначе начать варку
					{
						Run = Welder_Run;
						WelderUnit.State |= WELDER_MOVE_ENABLE; // Разрешить движение каретки
						xQueueSendToBack(qWelderRun, &Run, 0 ); // Начать варку в автоматическом режиме

					}




				}
				break;
			}


			case WELDER_MODE_AUTO:
			{

				Run = Welder_Run;

				if (WelderUnit.State & 0x01) // Если каретка уже движеся, то остановать её (остановка варки)
				{

					WelderUnit.State &= ~WELDER_MOVE_ENABLE; // Заппретить движение каретки

					SYNC_ARC_OFF // Прекращение подачи дуги

					WELDER_HEAD_UP // Поднять головку
					WelderUnit.IndicatorPanel.LEDsState |= LED_UP; // Индикация поднятой сварочной головки
					WelderUnit.IndicatorPanel.LEDsState &= ~LED_DOWN;

					// Далее идет очистка очередей задачи vWelder_Run. Задержка необхадима для переключения контеста задачи
					Run = Carriage_Done;
					xQueueSendToBack(qGoToResponse, &Run, 0 );
					vTaskDelay(10);
					xQueueSendToBack(qGoToResponse, &Run, 0 );
					vTaskDelay(10);
					xQueueSendToBack(qGoToResponse, &Run, 0 );
					vTaskDelay(10);
					xQueueSendToBack(qGoToResponse, &Run, 0 );
					vTaskDelay(10);
					xQueueSendToBack(qGoToResponse, &Run, 0 );
					vTaskDelay(10);


					xQueueReset(qGoToResponse); // Сброс очереди в исходное состояние


				}
				else // Иначе начать варку
				{

					//UBaseType_t Q;

					if (!(WelderUnit.State & WELDER_MOVE_ENABLE)) // Если движение было запрещено (остановлено), то просто разрешить движение каретки
					{
					WelderUnit.State |= WELDER_MOVE_ENABLE; // Разрешить движение каретки
					}
					else // иначе начать варку
					{
						Run = Welder_Run;
						WelderUnit.State |= WELDER_MOVE_ENABLE; // Разрешить движение каретки
						xQueueSendToBack(qWelderRun, &Run, 0 ); // Начать варку в автоматическом режиме

					}




				}
				break;
			}
			}


//			Carriage_cmd = Cmd_CarriageGoTo;
//			xQueueSendToBack( qWelderCmd, &Carriage_cmd, 0 ); // Идити к

			break;
		}

		case press_long_StartStop:
		{
			WelderUnit.Xf += 20; // Дейсвия при длинном нажатии
//			Key.StartStop_ShortPress = CNT_KEY_SHORT + 2; // Для запрета работы коротких нажатий
//			Key.StartStop_LongPress = CNT_LONG_PRESS; // Сброс счета для определения длинного нажатия. Не 0 что бы дейсвия выполнялось быстрее
			break;
		}



		// *********************************************************Комбинации клавишь*********************************************************

		case press_short_Program_L_and_R:
		{
			beep_mode_send = beep_long;
			xQueueSendToBack( qBeepMode, &beep_mode_send , 0 ); // Звук нажатия
			SaveProgramToFlash2();
			beep_mode_send = beep_click;
			break;
		}

		}

		//vTaskDelay(100 / portTICK_RATE_MS);
	}

	vTaskDelete(NULL);

}

void vWelder_Run(void *pvParameters)
{
	uint8_t Carriage_cmd;
	uint8_t lReceivedValue;

	uint8_t beep;

	for(;;)
	{
		xQueueReceive(qWelderRun, &lReceivedValue, portMAX_DELAY ); // Ожидание команды на начало варки

		if (lReceivedValue == Welder_Run && (WelderUnit.State & WELDER_MOVE_ENABLE) && (WelderUnit.State & WELDER_STATE_BACK_DOOR_CLOSE) ) // Если пришла команда на начло калибровки и движение каретки разрешено и задняя дверца закрыта
		{
			beep = beep_1short;
			xQueueSendToBack( qBeepMode, &beep, 0 ); // Звук нажатия

		if (WelderUnit.Position != WelderUnit.Xs) // Если текущая позиция каретки не равна стартовой позиции, то занять её
		{
			Carriage_cmd = Cmd_CarriageGoTo;
			WelderUnit.GoTo = WelderUnit.Xs;
			xQueueSendToBack( qWelderCmd, &Carriage_cmd, 0 ); // Идти к

			xQueueReceive(qGoToResponse, &lReceivedValue, 0 ); // Без этого не работает. В очереди откуда то берутся данные
			xQueueReceive(qGoToResponse, &lReceivedValue, portMAX_DELAY ); // Ждать ответа от задачи CarriageGoTo о том что нужная позиция картеки занята
		}

		if ((WelderUnit.Position == WelderUnit.Xs) && (WelderUnit.State & WELDER_MOVE_ENABLE)) // Если каретка на заданной позиции и движение каретки разрешено
		{

		WELDER_HEAD_DOWN // Опустить головку
		WelderUnit.IndicatorPanel.LEDsState |= LED_DOWN; // Индикация опущенной сварочной головки
		WelderUnit.IndicatorPanel.LEDsState &= ~LED_UP;
		vTaskDelay(100 / portTICK_RATE_MS); // Ожидание опускания головки
		SYNC_ARC_ON // Подача дуги

		vTaskDelay(WelderUnit.Delay_s * 100 / portTICK_RATE_MS); // Выдержка времени для заполнения точки начала сварки аргоном.

		WelderUnit.GoTo = WelderUnit.Xf; // Указание точки осановки головки
		Carriage_cmd = Cmd_CarriageGoTo; // Команда на начала движение каретки
		xQueueSendToBack( qWelderCmd, &Carriage_cmd, 0 ); // Идти к


		xQueueReceive(qGoToResponse, &lReceivedValue, 0 ); // Без этого не работает. В очереди откуда то берутся данные
		xQueueReceive(qGoToResponse, &lReceivedValue, portMAX_DELAY ); // Ждать ответа от задачи CarriageGoTo о том что нужная позиция картеки занята


		SYNC_ARC_OFF // Прекращение подачи дуги

		vTaskDelay(WelderUnit.Delay_f * 100 / portTICK_RATE_MS); // Выдержка времени для заполнения точки останова сварки аргоном.

		beep = beep_3short;
		xQueueSendToBack( qBeepMode, &beep, 0 ); // Звук окончания варки

		WELDER_HEAD_UP // Поднять головку
		vTaskDelay(100 / portTICK_RATE_MS); // Ожидание подъема головки

		WelderUnit.IndicatorPanel.LEDsState |= LED_UP; // Индикация поднятой сварочной головки
		WelderUnit.IndicatorPanel.LEDsState &= ~LED_DOWN;

		// Откат каретки

			if (WelderUnit.Position > KICKBACK)
			{
				WelderUnit.GoTo = WelderUnit.Position -	KICKBACK;
			}
			else
			{
				WelderUnit.GoTo = 0;
			}

			Carriage_cmd = Cmd_CarriageGoTo;
			xQueueSendToBack( qWelderCmd, &Carriage_cmd, 0 ); // Идити к
			xQueueReceive(qGoToResponse, &lReceivedValue, portMAX_DELAY ); // Ждать ответа от задачи CarriageGoTo о том что нужная позиция картеки ханята

		}



		}

	}

	vTaskDelete(NULL);

}



void vCarriage_Calibration(void *pvParameters)
{
	uint8_t lReceivedValue;


	for(;;)
	{
		xQueueReceive(qWelderCalibrated, &lReceivedValue, portMAX_DELAY ); // Ждать команды на начало калибровки

		if (lReceivedValue == Calibrated && (WelderUnit.State & 0x10) && (WelderUnit.State & WELDER_STATE_BACK_DOOR_CLOSE) && ((WelderUnit.State & WELDER_CLIBRATION_PROCESS))) // Если пришла команда на калибровку и движение каретки разрешено и задняя дверца закрыта
		{																																										// и если калибровка разрешена			WelderUnit.Calibration_level = 01; // Первая фаза калибровки - движение каретки к концевуику с большной скоростью
			WELDER_HEAD_UP // Поднять головку
			vTaskDelay(100 / portTICK_RATE_MS); // Ожидание подъема головки

			WelderUnit.IndicatorPanel.LEDsState &= ~LED_PARKING; // индикации калибровки


			WelderUnit.IndicatorPanel.LEDsState |= LED_UP; // Индикация поднятой сварочной головки
			WelderUnit.IndicatorPanel.LEDsState &= ~LED_DOWN;

			Carriage_Move(CALIBRATION_PHASE_SPEED_1, 0, 1); // Начать перемещение каретки в сторону концевика

			// Когда концевик замкнулся выдается семофор, что коретка дошла до самого начала

			// По какой-то причине при первой попытке взять 2-й семафор он берется (даже если не выдан). Потому семафор берется дважды
			xSemaphoreTake( xSemaphore_Calibration, portMAX_DELAY ); // Попытка взять семафор.
			xSemaphoreTake( xSemaphore_Calibration, portMAX_DELAY );

			Carriage_Move(0, 0, 1); // Стоп

				if ((WelderUnit.State & 0x10) && (WelderUnit.State & WELDER_STATE_BACK_DOOR_CLOSE) && ((WelderUnit.State & WELDER_CLIBRATION_PROCESS)) ) // Если движение каретки разрешено и задняя дверца закрыта
				{
					WelderUnit.Calibration_level = 02; // Вторая фаза калибровки - откат каретки от концевика в течении некторого времени
					Carriage_Move(CALIBRATION_PHASE_SPEED_2, 1, 1); // Отъехать немного назад
					vTaskDelay(1000 / portTICK_RATE_MS);

					Carriage_Move(0, 0, 1); // Стоп

					vTaskDelay(100/ portTICK_RATE_MS);

					if (WelderUnit.Calibration_level == 02) // Если фаза калибровки 2 (а не 05, что значит остановить калибровку)
					{

					WelderUnit.Calibration_level = 03; // Третья фаза калибровки - медленное движение каретки к концевуику на низкой скорости

					Carriage_Move(CALIBRATION_PHASE_SPEED_3, 0, 1); // Начать перемещение каретки в сторону концевика

					// По какой-то причине при первой попытке взять 2-й семафор он берется (даже если не выдан). Потому семафор берется дважды
					xSemaphoreTake( xSemaphore_Calibration, portMAX_DELAY ); // Попытка взять семафор по прерыванию срабатывания концевика
					xSemaphoreTake( xSemaphore_Calibration, portMAX_DELAY );

					Carriage_Move(0, 0, 1); // Стоп

					}

					if ((WelderUnit.State & 0x10) && (WelderUnit.State & WELDER_STATE_BACK_DOOR_CLOSE) && ((WelderUnit.State & WELDER_CLIBRATION_PROCESS)) && (WelderUnit.Calibration_level == 03) )
					{
						WelderUnit.Calibration_level = 04; // Откалибровано

						WelderUnit.State |= 1<<1; //1 бит - Откалибровано

						WelderUnit.Position = 0; // Позиция каретки

						WelderUnit.Mode = WELDER_MODE_MANUAL; // После калибровки режим работы аппарата - ручной.

						WelderUnit.IndicatorPanel.LEDsState |= LED_PARKING; // Отображение что выбран режим калибровки

						WelderUnit.State |= WELDER_MOVE_ENABLE; // Разрешить движение каретки
					}

					if (WelderUnit.Calibration_level  == 05) // Остановка калибровки
					{
						WelderUnit.Mode = 00; // Режим работа аппарата не выбран
						WelderUnit.IndicatorPanel.LEDsState &= ~LED_PARKING; // индикации калибровки

					}
				}
		}

	}

	vTaskDelete(NULL);
}


void vCarriage_GoTo(void *pvParameters)
{
	int32_t dX;

	uint8_t lReceivedValue;
	uint8_t SendVal;
	//portBASE_TYPE xStatus;

	for(;;)
	{
		//Попытка получить команду для передвижения каретки в нужную позицию.
		// Данные из очереди при этом не удаляются
		xQueuePeek(qWelderCmd, &lReceivedValue, portMAX_DELAY );

//		if (!(WelderUnit.State & WELDER_MOVE_ENABLE))
//		{
//			Carriage_Move(0, 1, 1); // стоп
//			WelderUnit.State &= ~0x01;
//			xQueueReceive(qWelderCmd, &lReceivedValue, portMAX_DELAY );
//			//WelderUnit.GoTo = WelderUnit.Position;
//		}

		if ((lReceivedValue == Cmd_CarriageGoTo) ) // Если пришла команда на начло движения
		{

		WelderUnit.Position = WelderUnit.Steps * DISTANCE_PER_ROTATE / (STEPS_PER_ROTATE * MICRO_STEP_DEV); // Вычисление текущей позиции каретки, мм

		dX = WelderUnit.GoTo - WelderUnit.Position; // Определение сколько и в какую сторону нужно сдвинуть каретку

		// Если каретка на нужном месте, то остановить движение каретки, очищаем очередь и снова взврат к xQueuePeek, для ожидания след команды
		if (dX == 0)
		{
			Carriage_Move(0, 1, 1); // стоп
			WelderUnit.State &= ~0x01;
			SendVal = Carriage_Done;
			xQueueSendToBack( qGoToResponse, &SendVal, 0 );

			//xQueueSendToBack( qDebug, &SendVal, 0 );

			xQueueReceive(qWelderCmd, &lReceivedValue, portMAX_DELAY );

		}

		if (dX > 0 && (WelderUnit.State & WELDER_MOVE_ENABLE)) // Если двигать в сторону концевика
		{

			if (WelderUnit.Position <= WelderUnit.GoTo && !(WelderUnit.State & 0x01)) // Если позиция не достигнута и каретка находится не в движении, то начать перемещение
			{
				Carriage_Move(WelderUnit.Speed, 1, 1); // Начать перемещение
				WelderUnit.State |= 0x01; // Статус - каретка в движении
				WelderUnit.State &= ~0x04; // Статус картека движется на встречу концевику
			}
		}

		if (dX < 0 && (WelderUnit.State & WELDER_MOVE_ENABLE)) // Если нужно двигать от концевика
		{
			if (WelderUnit.Position >= WelderUnit.GoTo && !(WelderUnit.State & 0x01)) // Если позиция не достигнута и каретка находится не в движении, то начать перемещение
			{
				Carriage_Move(WelderUnit.Speed, 0, 1); // Начать перемещение
				WelderUnit.State |= 0x01; // Статус - каретка в движении
				WelderUnit.State |= 0x04; // Статус картека движется от концевика
			}
		}

		if(!(WelderUnit.State & WELDER_MOVE_ENABLE))
		{
			Carriage_Move(0, 1, 1); // стоп
			WelderUnit.State &= ~0x01;
		}

		if (xSemaphoreTake(xSemaphore_StepCount, (500 / portTICK_RATE_MS)) == pdTRUE) // Если через 100 мС семафор так и не получен, то считать что двигатель осановлен
		//xSemaphoreTake(xSemaphore_StepCount, portMAX_DELAY );
		{
			if (!(WelderUnit.State & 0x04)) // Если каретка движется на встречу концевику, то инкрментирование Steps на каждый шаг ШД
				{
					WelderUnit.Steps++;
				}

				if (WelderUnit.State & 0x04) // Если каретка движется от концевика, то декрементирование Steps на каждый шаг ШД
				{
						WelderUnit.Steps--;
				}
		}

		}



	}
	vTaskDelete(NULL);
}

void vKeyScan(void *pvParameters)
{


	uint8_t nKeyPressed; // Передача команды о том какая кнопка или комбинация кнопок нажаты
	//portBASE_TYPE xStatus;

	vTaskDelay(100 / portTICK_RATE_MS);

	for(;;)
	{
		PCB_KeyScan();
		PCB_OutputControl();
		//PCB_InputsScan();

		if (WelderUnit.IndicatorPanel.KeyState[0] == 128) // ВЛЕВО: [0], "-" - 128, "+" - 64, , STLED №5;
		{
			// Если времы выдрежки короткого нажатия достигло заданного итервала времени и это нажатие не считается длинным, то нажатие считается кортоким
			if (Key.Xs_ShortPress_L > CNT_KEY_SHORT && Key.Xs_ShortPress_L != CNT_KEY_SHORT + 2)
			{
//				nKeyPressed = press_short_Xs_L;
//				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Xs_ShortPress_L = CNT_KEY_SHORT + 3; // Для ожидания отпускания клавиши
			}

			if (Key.Xs_ShortPress_L != CNT_KEY_SHORT + 2) // Если нажатие не считается долгим
			{
				Key.Xs_ShortPress_L++; // Счетчик для определения короткого нажатия на кнопку
			}


			Key.Xs_LongPress_L++; // Счетчик для определения длинного нажатия на кнопку
			if (Key.Xs_LongPress_L > CNT_KEY_LONG) // Если времы выдрежки длинного нажатия достигло заданного интервала времени, то нажатие считается длинным
			{
				nKeyPressed = press_long_Xs_L;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши


				Key.Xs_ShortPress_L = CNT_KEY_SHORT + 2; // Для запрета работы коротких нажатий
				Key.Xs_LongPress_L = CNT_LONG_PRESS; // Сброс счета для определения длинного нажатия. Не 0 что бы дейсвия выполнялось быстрее
			}


		}
		else // Если кнопка была отпущена то сбросить все счетчики выдержки
		{


			if (Key.Xs_ShortPress_L >= CNT_KEY_SHORT + 3)
			{
				nKeyPressed = press_short_Xs_L;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши
			}

			Key.Xs_ShortPress_L = 0;
			Key.Xs_LongPress_L = 0;
		}

		if (WelderUnit.IndicatorPanel.KeyState[0] == 64) // Если есть факт нажатия кнопки
		{
			// Если времы выдрежки короткого нажатия достигло заданного итервала времени и это нажатие не считается длинным, то нажатие считается кортоким
			if (Key.Xs_ShortPress_R > CNT_KEY_SHORT && Key.Xs_ShortPress_R != CNT_KEY_SHORT + 2)
			{

				nKeyPressed = press_short_Xs_R;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Xs_ShortPress_R = 0; // Сброс счетика выдержки
			}

			if (Key.Xs_ShortPress_R != CNT_KEY_SHORT + 2) // Если нажатие не считается долгим
			{
				Key.Xs_ShortPress_R++; // Счетчик для определения короткого нажатия на кнопку
			}


			Key.Xs_LongPress_R++; // Счетчик для определения длинного нажатия на кнопку
			if (Key.Xs_LongPress_R > CNT_KEY_LONG) // Если времы выдрежки длинного нажатия достигло заданного интервала времени, то нажатие считается длинным
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_long_Xs_R;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Xs_ShortPress_R = CNT_KEY_SHORT + 2; // Для запрета работы коротких нажатий
				Key.Xs_LongPress_R = CNT_LONG_PRESS; // Сброс счета для определения длинного нажатия. Не 0 что бы дейсвия выполнялось быстрее
			}


		}
		else // Если кнопка была отпущена то сбросить все счетчики выдержки
		{
			Key.Xs_ShortPress_R = 0;
			Key.Xs_LongPress_R = 0;
		}

		if (WelderUnit.IndicatorPanel.KeyState[0] == 16) // ВПРАВО: [0], "-" - 16, "+" - 32, , STLED №5;
		{
			// Если времы выдрежки короткого нажатия достигло заданного итервала времени и это нажатие не считается длинным, то нажатие считается кортоким
			if (Key.Xf_ShortPress_L > CNT_KEY_SHORT && Key.Xf_ShortPress_L != CNT_KEY_SHORT + 2)
			{

				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_short_Xf_L;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши


				Key.Xf_ShortPress_L = 0; // Сброс счетика выдержки
			}

			if (Key.Xf_ShortPress_L != CNT_KEY_SHORT + 2) // Если нажатие не считается долгим
			{
				Key.Xf_ShortPress_L++; // Счетчик для определения короткого нажатия на кнопку
			}


			Key.Xf_LongPress_L++; // Счетчик для определения длинного нажатия на кнопку
			if (Key.Xf_LongPress_L > CNT_KEY_LONG) // Если времы выдрежки длинного нажатия достигло заданного интервала времени, то нажатие считается длинным
			{

				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_long_Xf_L;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши


				Key.Xf_ShortPress_L = CNT_KEY_SHORT + 2; // Для запрета работы коротких нажатий
				Key.Xf_LongPress_L = CNT_LONG_PRESS; // Сброс счета для определения длинного нажатия. Не 0 что бы дейсвия выполнялось быстрее
			}


		}
		else // Если кнопка была отпущена то сбросить все счетчики выдержки
		{
			Key.Xf_ShortPress_L = 0;
			Key.Xf_LongPress_L = 0;
		}

		if (WelderUnit.IndicatorPanel.KeyState[0] == 32)
		{
			// Если времы выдрежки короткого нажатия достигло заданного итервала времени и это нажатие не считается длинным, то нажатие считается кортоким
			if (Key.Xf_ShortPress_R > CNT_KEY_SHORT && Key.Xf_ShortPress_R != CNT_KEY_SHORT + 2)
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_short_Xf_R;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши


				Key.Xf_ShortPress_R = 0; // Сброс счетика выдержки
			}

			if (Key.Xf_ShortPress_R != CNT_KEY_SHORT + 2) // Если нажатие не считается долгим
			{
				Key.Xf_ShortPress_R++; // Счетчик для определения короткого нажатия на кнопку
			}


			Key.Xf_LongPress_R++; // Счетчик для определения длинного нажатия на кнопку
			if (Key.Xf_LongPress_R > CNT_KEY_LONG) // Если времы выдрежки длинного нажатия достигло заданного интервала времени, то нажатие считается длинным
			{

				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_long_Xf_R;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Xf_ShortPress_R = CNT_KEY_SHORT + 2; // Для запрета работы коротких нажатий
				Key.Xf_LongPress_R = CNT_LONG_PRESS; // Сброс счета для определения длинного нажатия. Не 0 что бы дейсвия выполнялось быстрее
			}


		}
		else // Если кнопка была отпущена то сбросить все счетчики выдержки
		{
			Key.Xf_ShortPress_R = 0;
			Key.Xf_LongPress_R = 0;
		}

		if (WelderUnit.IndicatorPanel.KeyState[0] == 8) //ЗАДЕРЖКА (Delay_s): [0], "-" - 8, "+" - 4, , STLED №5;
		{
			// Если времы выдрежки короткого нажатия достигло заданного итервала времени и это нажатие не считается длинным, то нажатие считается кортоким
			if (Key.Delay_s_ShortPress_L > CNT_KEY_SHORT && Key.Delay_s_ShortPress_L != CNT_KEY_SHORT + 2)
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_short_Delay_s_L;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Delay_s_ShortPress_L = 0; // Сброс счетика выдержки
			}

			if (Key.Delay_s_ShortPress_L != CNT_KEY_SHORT + 2) // Если нажатие не считается долгим
			{
				Key.Delay_s_ShortPress_L++; // Счетчик для определения короткого нажатия на кнопку
			}


			Key.Delay_s_LongPress_L++; // Счетчик для определения длинного нажатия на кнопку
			if (Key.Delay_s_LongPress_L > CNT_KEY_LONG) // Если времы выдрежки длинного нажатия достигло заданного интервала времени, то нажатие считается длинным
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_long_Delay_s_L;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Delay_s_ShortPress_L = CNT_KEY_SHORT + 2; // Для запрета работы коротких нажатий
				Key.Delay_s_LongPress_L = CNT_LONG_PRESS; // Сброс счета для определения длинного нажатия. Не 0 что бы дейсвия выполнялось быстрее
			}


		}
		else // Если кнопка была отпущена то сбросить все счетчики выдержки
		{
			Key.Delay_s_ShortPress_L = 0;
			Key.Delay_s_LongPress_L = 0;
		}

		if (WelderUnit.IndicatorPanel.KeyState[0] == 4)
			{
				// Если времы выдрежки короткого нажатия достигло заданного итервала времени и это нажатие не считается длинным, то нажатие считается кортоким
				if (Key.Delay_s_ShortPress_R > CNT_KEY_SHORT && Key.Delay_s_ShortPress_R != CNT_KEY_SHORT + 2)
				{
					// Дейсвие при нажатии на клавишу
					nKeyPressed = press_short_Delay_s_R;
					xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

					Key.Delay_s_ShortPress_R = 0; // Сброс счетика выдержки
				}

				if (Key.Delay_s_ShortPress_R != CNT_KEY_SHORT + 2) // Если нажатие не считается долгим
				{
					Key.Delay_s_ShortPress_R++; // Счетчик для определения короткого нажатия на кнопку
				}


				Key.Delay_s_LongPress_R++; // Счетчик для определения длинного нажатия на кнопку
				if (Key.Delay_s_LongPress_R > CNT_KEY_LONG) // Если времы выдрежки длинного нажатия достигло заданного интервала времени, то нажатие считается длинным
				{
					// Дейсвие при нажатии на клавишу
					nKeyPressed = press_long_Delay_s_R;
					xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

					Key.Delay_s_ShortPress_R = CNT_KEY_SHORT + 2; // Для запрета работы коротких нажатий
					Key.Delay_s_LongPress_R = CNT_LONG_PRESS; // Сброс счета для определения длинного нажатия. Не 0 что бы дейсвия выполнялось быстрее
				}


			}
			else // Если кнопка была отпущена то сбросить все счетчики выдержки
			{
				Key.Delay_s_ShortPress_R = 0;
				Key.Delay_s_LongPress_R = 0;
			}

		if (WelderUnit.IndicatorPanel.KeyState[0] == 2) // СКОРОСТЬ: [0], "-" - 2, "+" - 1, , STLED №5;
		{
			// Если времы выдрежки короткого нажатия достигло заданного итервала времени и это нажатие не считается длинным, то нажатие считается кортоким
			if (Key.Speed_ShortPress_L > CNT_KEY_SHORT && Key.Speed_ShortPress_L != CNT_KEY_SHORT + 2)
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_short_Speed_L;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Speed_ShortPress_L = 0; // Сброс счетика выдержки
			}

			if (Key.Speed_ShortPress_L != CNT_KEY_SHORT + 2) // Если нажатие не считается долгим
			{
				Key.Speed_ShortPress_L++; // Счетчик для определения короткого нажатия на кнопку
			}


			Key.Speed_LongPress_L++; // Счетчик для определения длинного нажатия на кнопку
			if (Key.Speed_LongPress_L > CNT_KEY_LONG) // Если времы выдрежки длинного нажатия достигло заданного интервала времени, то нажатие считается длинным
			{

				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_long_Speed_L;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Speed_ShortPress_L = CNT_KEY_SHORT + 2; // Для запрета работы коротких нажатий
				Key.Speed_LongPress_L = CNT_LONG_PRESS; // Сброс счета для определения длинного нажатия. Не 0 что бы дейсвия выполнялось быстрее
			}


		}
		else // Если кнопка была отпущена то сбросить все счетчики выдержки
		{
			Key.Speed_ShortPress_L = 0;
			Key.Speed_LongPress_L = 0;
		}

		if (WelderUnit.IndicatorPanel.KeyState[0] == 1)
		{
			// Если времы выдрежки короткого нажатия достигло заданного итервала времени и это нажатие не считается длинным, то нажатие считается кортоким
			if (Key.Speed_ShortPress_R > CNT_KEY_SHORT && Key.Speed_ShortPress_R != CNT_KEY_SHORT + 2)
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_short_Speed_R;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Speed_ShortPress_R = 0; // Сброс счетика выдержки
			}

			if (Key.Speed_ShortPress_R != CNT_KEY_SHORT + 2) // Если нажатие не считается долгим
			{
				Key.Speed_ShortPress_R++; // Счетчик для определения короткого нажатия на кнопку
			}


			Key.Speed_LongPress_R++; // Счетчик для определения длинного нажатия на кнопку
			if (Key.Speed_LongPress_R > CNT_KEY_LONG) // Если времы выдрежки длинного нажатия достигло заданного интервала времени, то нажатие считается длинным
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_long_Speed_R;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Speed_ShortPress_R = CNT_KEY_SHORT + 2; // Для запрета работы коротких нажатий
				Key.Speed_LongPress_R = CNT_LONG_PRESS; // Сброс счета для определения длинного нажатия. Не 0 что бы дейсвия выполнялось быстрее
			}


		}
		else // Если кнопка была отпущена то сбросить все счетчики выдержки
		{
			Key.Speed_ShortPress_R = 0;
			Key.Speed_LongPress_R = 0;
		}

		if (WelderUnit.IndicatorPanel.KeyState[1] == 1) // ЗАДЕРЖКА (Delay_f): [1], "-" - 1, "+" - 4, , STLED №5;
		{
			// Если времы выдрежки короткого нажатия достигло заданного итервала времени и это нажатие не считается длинным, то нажатие считается кортоким
			if (Key.Delay_f_ShortPress_L > CNT_KEY_SHORT && Key.Delay_f_ShortPress_L != CNT_KEY_SHORT + 2)
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_short_Delay_f_L;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Delay_f_ShortPress_L = 0; // Сброс счетика выдержки
			}

			if (Key.Delay_f_ShortPress_L != CNT_KEY_SHORT + 2) // Если нажатие не считается долгим
			{
				Key.Delay_f_ShortPress_L++; // Счетчик для определения короткого нажатия на кнопку
			}


			Key.Delay_f_LongPress_L++; // Счетчик для определения длинного нажатия на кнопку
			if (Key.Delay_f_LongPress_L > CNT_KEY_LONG) // Если времы выдрежки длинного нажатия достигло заданного интервала времени, то нажатие считается длинным
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_long_Delay_f_L;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши


				Key.Delay_f_ShortPress_L = CNT_KEY_SHORT + 2; // Для запрета работы коротких нажатий
				Key.Delay_f_LongPress_L = CNT_LONG_PRESS; // Сброс счета для определения длинного нажатия. Не 0 что бы дейсвия выполнялось быстрее
			}


		}
		else // Если кнопка была отпущена то сбросить все счетчики выдержки
		{
			Key.Delay_f_ShortPress_L = 0;
			Key.Delay_f_LongPress_L = 0;
		}

		if (WelderUnit.IndicatorPanel.KeyState[1] == 4)
		{
			// Если времы выдрежки короткого нажатия достигло заданного итервала времени и это нажатие не считается длинным, то нажатие считается кортоким
			if (Key.Delay_f_ShortPress_R > CNT_KEY_SHORT && Key.Delay_f_ShortPress_R != CNT_KEY_SHORT + 2)
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_short_Delay_f_R;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Delay_f_ShortPress_R = 0; // Сброс счетика выдержки
			}

			if (Key.Delay_f_ShortPress_R != CNT_KEY_SHORT + 2) // Если нажатие не считается долгим
			{
				Key.Delay_f_ShortPress_R++; // Счетчик для определения короткого нажатия на кнопку
			}


			Key.Delay_f_LongPress_R++; // Счетчик для определения длинного нажатия на кнопку
			if (Key.Delay_f_LongPress_R > CNT_KEY_LONG) // Если времы выдрежки длинного нажатия достигло заданного интервала времени, то нажатие считается длинным
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_long_Delay_f_R;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Delay_f_ShortPress_R = CNT_KEY_SHORT + 2; // Для запрета работы коротких нажатий
				Key.Delay_f_LongPress_R = CNT_LONG_PRESS; // Сброс счета для определения длинного нажатия. Не 0 что бы дейсвия выполнялось быстрее
			}


		}
		else // Если кнопка была отпущена то сбросить все счетчики выдержки
		{
			Key.Delay_f_ShortPress_R = 0;
			Key.Delay_f_LongPress_R = 0;
		}

		if (WelderUnit.IndicatorPanel.KeyState[3] == 1 && !(WelderUnit.IndicatorPanel.KeyState[1] == 2)) // ВЛЕВО: [0], "-" - 128, "+" - 64, , STLED №5; ПРОГРАММА: [1], "-" - ..., "+" - 2, , STLED №5;
		{
			// Если времы выдрежки короткого нажатия достигло заданного итервала времени и это нажатие не считается длинным, то нажатие считается кортоким
			if (Key.Program_ShortPress_L > CNT_KEY_SHORT && Key.Program_ShortPress_L != CNT_KEY_SHORT + 2)
			{
				// Дейсвие при нажатии на клавишу
//				nKeyPressed = press_short_Program_L;
//				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Program_ShortPress_L = CNT_KEY_SHORT + 3; // Сброс счетика выдержки
			}

			if (Key.Program_ShortPress_L != CNT_KEY_SHORT + 2) // Если нажатие не считается долгим
			{
				Key.Program_ShortPress_L++; // Счетчик для определения короткого нажатия на кнопку
			}


			Key.Program_LongPress_L++; // Счетчик для определения длинного нажатия на кнопку
			if (Key.Program_LongPress_L > CNT_KEY_LONG) // Если времы выдрежки длинного нажатия достигло заданного интервала времени, то нажатие считается длинным
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_long_Program_L;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Program_ShortPress_L = CNT_KEY_SHORT + 2; // Для запрета работы коротких нажатий
				Key.Program_LongPress_L = CNT_LONG_PRESS; // Сброс счета для определения длинного нажатия. Не 0 что бы дейсвия выполнялось быстрее
			}


		}
		else // Если кнопка была отпущена то сбросить все счетчики выдержки
		{
			if (Key.Program_ShortPress_L >= CNT_KEY_SHORT + 3) // Дейсвия при отжатии кнопки
			{
				nKeyPressed = press_short_Program_L;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши
			}

			Key.Program_ShortPress_L = 0;
			Key.Program_LongPress_L = 0;
		}

		if (WelderUnit.IndicatorPanel.KeyState[1] == 2 && !(WelderUnit.IndicatorPanel.KeyState[3] == 1))
		{
			// Если времы выдрежки короткого нажатия достигло заданного итервала времени и это нажатие не считается длинным, то нажатие считается кортоким
			if (Key.Program_ShortPress_R > CNT_KEY_SHORT && Key.Program_ShortPress_R != CNT_KEY_SHORT + 2)
			{
				// Дейсвие при нажатии на клавишу
//				nKeyPressed = press_short_Program_R;
//				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Program_ShortPress_R = CNT_KEY_SHORT + 3; // Сброс счетика выдержки
			}

			if (Key.Program_ShortPress_R != CNT_KEY_SHORT + 2) // Если нажатие не считается долгим
			{
				Key.Program_ShortPress_R++; // Счетчик для определения короткого нажатия на кнопку
			}


			Key.Program_LongPress_R++; // Счетчик для определения длинного нажатия на кнопку
			if (Key.Program_LongPress_R > CNT_KEY_LONG) // Если времы выдрежки длинного нажатия достигло заданного интервала времени, то нажатие считается длинным
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_long_Program_R;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Program_ShortPress_R = CNT_KEY_SHORT + 2; // Для запрета работы коротких нажатий
				Key.Program_LongPress_R = CNT_LONG_PRESS; // Сброс счета для определения длинного нажатия. Не 0 что бы дейсвия выполнялось быстрее
			}


		}
		else // Если кнопка была отпущена то сбросить все счетчики выдержки
		{
			if (Key.Program_ShortPress_R >= CNT_KEY_SHORT + 3) // Дейсвия при отжатии кнопки
			{
				nKeyPressed = press_short_Program_R;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши
			}

			Key.Program_ShortPress_R = 0;
			Key.Program_LongPress_R = 0;
		}



		if (WelderUnit.IndicatorPanel.KeyState[1] == 128) //ВВЕРХ, ВНИЗ: [1], "-" - 128, "+" - 64, , STLED №5;
		{
			// Если времы выдрежки короткого нажатия достигло заданного итервала времени и это нажатие не считается длинным, то нажатие считается кортоким
			if (Key.Up_ShortPress > CNT_KEY_SHORT && Key.Up_ShortPress != CNT_KEY_SHORT + 2)
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_short_Up;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Up_ShortPress = 0; // Сброс счетика выдержки
			}

			if (Key.Up_ShortPress != CNT_KEY_SHORT + 2) // Если нажатие не считается долгим
			{
				Key.Up_ShortPress++; // Счетчик для определения короткого нажатия на кнопку
			}


			Key.Up_LongPress++; // Счетчик для определения длинного нажатия на кнопку
			if (Key.Up_LongPress > CNT_KEY_LONG) // Если времы выдрежки длинного нажатия достигло заданного интервала времени, то нажатие считается длинным
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_long_Up;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши


				Key.Up_ShortPress = CNT_KEY_SHORT + 2; // Для запрета работы коротких нажатий
				Key.Up_LongPress = CNT_LONG_PRESS; // Сброс счета для определения длинного нажатия. Не 0 что бы дейсвия выполнялось быстрее
			}


		}
		else // Если кнопка была отпущена то сбросить все счетчики выдержки
		{
			Key.Up_ShortPress = 0;
			Key.Up_LongPress = 0;
		}
		if (WelderUnit.IndicatorPanel.KeyState[1] == 64)
		{
			// Если времы выдрежки короткого нажатия достигло заданного итервала времени и это нажатие не считается длинным, то нажатие считается кортоким
			if (Key.Down_ShortPress > CNT_KEY_SHORT && Key.Down_ShortPress != CNT_KEY_SHORT + 2)
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_short_Down;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Down_ShortPress = 0; // Сброс счетика выдержки
			}

			if (Key.Down_ShortPress != CNT_KEY_SHORT + 2) // Если нажатие не считается долгим
			{
				Key.Down_ShortPress++; // Счетчик для определения короткого нажатия на кнопку
			}


			Key.Down_LongPress++; // Счетчик для определения длинного нажатия на кнопку
			if (Key.Down_LongPress > CNT_KEY_LONG) // Если времы выдрежки длинного нажатия достигло заданного интервала времени, то нажатие считается длинным
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_long_Down;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши


				Key.Down_ShortPress = CNT_KEY_SHORT + 2; // Для запрета работы коротких нажатий
				Key.Down_LongPress = CNT_LONG_PRESS; // Сброс счета для определения длинного нажатия. Не 0 что бы дейсвия выполнялось быстрее
			}


		}
		else // Если кнопка была отпущена то сбросить все счетчики выдержки
		{
			Key.Down_ShortPress = 0;
			Key.Down_LongPress = 0;
		}

		if (WelderUnit.IndicatorPanel.KeyState[1] == 32) // ПАРКОВКА, АВТО: [1], "-" - 32, "+" - 16, , STLED №5;
		{
			// Если времы выдрежки короткого нажатия достигло заданного итервала времени и это нажатие не считается длинным, то нажатие считается кортоким
			if (Key.Parking_ShortPress > CNT_KEY_SHORT && Key.Parking_ShortPress != CNT_KEY_SHORT + 2)
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_short_Parking;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши


				Key.Parking_ShortPress = 0; // Сброс счетика выдержки
			}

			if (Key.Parking_ShortPress != CNT_KEY_SHORT + 2) // Если нажатие не считается долгим
			{
				Key.Parking_ShortPress++; // Счетчик для определения короткого нажатия на кнопку
			}


			Key.Parking_LongPress++; // Счетчик для определения длинного нажатия на кнопку
			if (Key.Parking_LongPress > CNT_KEY_LONG) // Если времы выдрежки длинного нажатия достигло заданного интервала времени, то нажатие считается длинным
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_long_Parking;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Parking_ShortPress = CNT_KEY_SHORT + 2; // Для запрета работы коротких нажатий
				Key.Parking_LongPress = CNT_LONG_PRESS; // Сброс счета для определения длинного нажатия. Не 0 что бы дейсвия выполнялось быстрее
			}


		}
		else // Если кнопка была отпущена то сбросить все счетчики выдержки
		{
			Key.Parking_ShortPress = 0;
			Key.Parking_LongPress = 0;
		}

		if (WelderUnit.IndicatorPanel.KeyState[1] == 16)
		{
			// Если времы выдрежки короткого нажатия достигло заданного итервала времени и это нажатие не считается длинным, то нажатие считается кортоким
			if (Key.Auto_ShortPress > CNT_KEY_SHORT && Key.Auto_ShortPress != CNT_KEY_SHORT + 2)
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_short_Auto;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Auto_ShortPress = 0; // Сброс счетика выдержки
			}

			if (Key.Auto_ShortPress != CNT_KEY_SHORT + 2) // Если нажатие не считается долгим
			{
				Key.Auto_ShortPress++; // Счетчик для определения короткого нажатия на кнопку
			}


			Key.Auto_LongPress++; // Счетчик для определения длинного нажатия на кнопку
			if (Key.Auto_LongPress > CNT_KEY_LONG) // Если времы выдрежки длинного нажатия достигло заданного интервала времени, то нажатие считается длинным
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_long_Auto;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Auto_ShortPress = CNT_KEY_SHORT + 2; // Для запрета работы коротких нажатий
				Key.Auto_LongPress = CNT_LONG_PRESS; // Сброс счета для определения длинного нажатия. Не 0 что бы дейсвия выполнялось быстрее
			}


		}
		else // Если кнопка была отпущена то сбросить все счетчики выдержки
		{
			Key.Auto_ShortPress = 0;
			Key.Auto_LongPress = 0;
		}

		if (WelderUnit.IndicatorPanel.KeyState[1] == 8)
		{
			// Если времы выдрежки короткого нажатия достигло заданного итервала времени и это нажатие не считается длинным, то нажатие считается кортоким
			if (Key.StartStop_ShortPress > CNT_KEY_SHORT && Key.StartStop_ShortPress != CNT_KEY_SHORT + 2)
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_short_StartStop;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.StartStop_ShortPress = 0; // Сброс счетика выдержки
			}

			if (Key.StartStop_ShortPress != CNT_KEY_SHORT + 2) // Если нажатие не считается долгим
			{
				Key.StartStop_ShortPress++; // Счетчик для определения короткого нажатия на кнопку
			}


			Key.StartStop_LongPress++; // Счетчик для определения длинного нажатия на кнопку
			if (Key.StartStop_LongPress > CNT_KEY_LONG) // Если времы выдрежки длинного нажатия достигло заданного интервала времени, то нажатие считается длинным
			{
				// Дейсвие при нажатии на клавишу
				nKeyPressed = press_long_StartStop;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.StartStop_ShortPress = CNT_KEY_SHORT + 2; // Для запрета работы коротких нажатий
				Key.StartStop_LongPress = CNT_LONG_PRESS; // Сброс счета для определения длинного нажатия. Не 0 что бы дейсвия выполнялось быстрее

			}

		}
		else // Если кнопка была отпущена то сбросить все счетчики выдержки
		{
			Key.StartStop_ShortPress = 0;
			Key.StartStop_LongPress = 0;
		}

		// *********************************************************Комбинации клавишь*********************************************************

		if (WelderUnit.IndicatorPanel.KeyState[3] == 1 && WelderUnit.IndicatorPanel.KeyState[1] == 2) // "ПРОГРАММА" "-" одновременно с "+"
		{
			// Если времы выдрежки короткого нажатия достигло заданного итервала времени и это нажатие не считается длинным, то нажатие считается кортоким
			if (Key.Program_ShortPress_L_and_R > CNT_KEY_SHORT && Key.Program_ShortPress_L_and_R != CNT_KEY_SHORT + 2)
			{
				// Дейсвие при нажатии на клавишу
//				nKeyPressed = press_short_StartStop;
//				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

				Key.Program_ShortPress_L_and_R = CNT_KEY_SHORT + 3; // Сброс счетика выдержки
			}

			if (Key.Program_ShortPress_L_and_R != CNT_KEY_SHORT + 2) // Если нажатие не считается долгим
			{
				Key.Program_ShortPress_L_and_R++; // Счетчик для определения короткого нажатия на кнопку
			}


			Key.Program_LongPress_L_and_R++; // Счетчик для определения длинного нажатия на кнопку
			if (Key.Program_LongPress_L_and_R > CNT_KEY_LONG) // Если времы выдрежки длинного нажатия достигло заданного интервала времени, то нажатие считается длинным
			{
				// Дейсвие при нажатии на клавишу
//				nKeyPressed = press_long_Program_L_and_R;
//				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

			}

		}
		else // Если кнопка была отпущена то сбросить все счетчики выдержки
		{
			if (Key.Program_ShortPress_L_and_R >= CNT_KEY_SHORT + 3) // Дейсвия при отжатии кнопки
			{
				nKeyPressed = press_short_Program_L_and_R;
				xQueueSendToBack( qKeyPress, &nKeyPressed, 0 ); // Передача номера нажатой клавиши

//				Key.Program_ShortPress_L = 0;
//				Key.Program_LongPress_L = 0;
//
//				Key.Program_ShortPress_R = 0;
//				Key.Program_LongPress_R = 0;
			}

			Key.Program_ShortPress_L_and_R = 0;
			Key.Program_LongPress_L_and_R = 0;


		}



		vTaskDelay(TIME_KEY_SCAN / portTICK_RATE_MS);

	}
	vTaskDelete(NULL);
}

void vIndicatorPanel_Out(void *pvParameters)
{
	WelderUnit.Xs = 30;
	WelderUnit.Xf = 90;
	WelderUnit.Steps = 0;
	WelderUnit.Delay_s = 10;
	WelderUnit.Speed = 120;
	WelderUnit.Delay_f = 20;
	WelderUnit.Program = 4;

	WelderUnit.GoTo = WelderUnit.Xf;
	WelderUnit.Position = 0;
	WelderUnit.State = 0;

	WelderUnit.IndicatorPanel.Brightness = 2;

//	WelderUnit.IndicatorPanel.LEDsState |= LED_AUTO;
//
//	WelderUnit.IndicatorPanel.LEDsState |= LED_PARKING;

	IndicatorPanel_Ini();

	IndicatorPanel_SetBrightness(&WelderUnit.IndicatorPanel);

	vTaskDelay(100 / portTICK_RATE_MS);


//	SaveProgramToFlash2();

	for(;;)
	{
		//WelderUnit.Xs++;

		//PCB_LEDs_OUT(PCB_LEDs_OUT(LEDs_val))



		if (WelderUnit.State & 0x01) // Если каретка в движении то отображать ее позицию
		{
			WelderUnit.IndicatorPanel.Xs = WelderUnit.Position;
		}

		if (!(WelderUnit.State & 0x01)) // Если каретка не в движении то отображать точку начала варки
		{
			WelderUnit.IndicatorPanel.Xs = WelderUnit.Xs;
		}

		WelderUnit.IndicatorPanel.Xf = WelderUnit.Xf;
		WelderUnit.IndicatorPanel.Delay_s = WelderUnit.Delay_s;
		WelderUnit.IndicatorPanel.Speed = WelderUnit.Speed;
		WelderUnit.IndicatorPanel.Delay_f = WelderUnit.Delay_f;
		WelderUnit.IndicatorPanel.Program = WelderUnit.Program;


		IndicatorPanel_SetValue(&WelderUnit.IndicatorPanel);

		vTaskDelay(100 / portTICK_RATE_MS);


//		IndicatorPanel_SetBrightness(&WelderUnit.IndicatorPanel);
	}
	vTaskDelete(NULL);
}


void vBuzzer_beep(void *pvParameters)
{
	uint8_t lReceivedValue;
	portBASE_TYPE xStatus;

	for(;;)
	{
		xStatus = xQueueReceive( qBeepMode, &lReceivedValue, portMAX_DELAY );

		if( xStatus == pdPASS ) // Если данные были успешно приняты из очереди
		{

		switch(lReceivedValue)
		{
		case beep_off:
		{
			BUZZER_OFF
			break;
		}

		case beep_click:
		{
			BUZZER_ON
			vTaskDelay(10 / portTICK_RATE_MS);
			BUZZER_OFF
			vTaskDelay(50 / portTICK_RATE_MS);
			break;
		}

		case beep_1short:
		{
			for(uint16_t i = 0; i < 10; i++)
			    {
			    BUZZER_ON
			    vTaskDelay(1 / portTICK_RATE_MS);
			    BUZZER_OFF
			    vTaskDelay(1 / portTICK_RATE_MS);
			    }
			break;
		}

		case beep_2short:
		{
			BUZZER_ON
			vTaskDelay(100 / portTICK_RATE_MS);
			BUZZER_OFF
			vTaskDelay(100 / portTICK_RATE_MS);

			BUZZER_ON
			vTaskDelay(100 / portTICK_RATE_MS);
			BUZZER_OFF
			vTaskDelay(100 / portTICK_RATE_MS);
			break;
		}

		case beep_3short:
		{
			for(uint16_t i = 0; i < 40; i++)
			    {
				BUZZER_ON
				vTaskDelay(1 / portTICK_RATE_MS);
				BUZZER_OFF
				vTaskDelay(1 / portTICK_RATE_MS);
			    }

			vTaskDelay(40 / portTICK_RATE_MS);

			for(uint16_t i = 0; i < 40; i++)
			    {
				BUZZER_ON
				vTaskDelay(1 / portTICK_RATE_MS);
				BUZZER_OFF
				vTaskDelay(1 / portTICK_RATE_MS);
			    }

			vTaskDelay(40 / portTICK_RATE_MS);

			for(uint16_t i = 0; i < 50; i++)
			    {
				BUZZER_ON
				vTaskDelay(1 / portTICK_RATE_MS);
				BUZZER_OFF
				vTaskDelay(1 / portTICK_RATE_MS);
			    }

			vTaskDelay(40 / portTICK_RATE_MS);

			break;
		}

		case beep_long:
		{
			for(uint16_t i = 0; i < 200; i++)
			    {
				BUZZER_ON
				vTaskDelay(1 / portTICK_RATE_MS);
				BUZZER_OFF
				vTaskDelay(1 / portTICK_RATE_MS);
			    }
			break;
		}

		}

		}

		vTaskMissedYield(); // Переключение контекста.


	}
	vTaskDelete(NULL);
}


