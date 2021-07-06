#include "PCB_WELDER.h"

void IndicatorPanel_Ini (void)
{
	STLED316S_Ini(oSTLED316S_STB1_GPIO_Port, oSTLED316S_STB1_Pin); // Индикатор "ВЛЕВО"
	STLED316S_Ini(oSTLED316S_STB2_GPIO_Port, oSTLED316S_STB2_Pin); // Индикатор "ВПРАВО"
	STLED316S_Ini(oSTLED316S_STB3_GPIO_Port, oSTLED316S_STB3_Pin); // Индикаторы "СКОРОСТЬ" (ст. 3 разряда) и "ПРОГРАММА" (мл. 3 разряда)
  //STLED316S_Ini(oSTLED316S_STB4_GPIO_Port, oSTLED316S_STB4_Pin); // Микросхема не установлена, резерв
	STLED316S_Ini(oSTLED316S_STB5_GPIO_Port, oSTLED316S_STB5_Pin); // Индикаторы "ЗАДЕРЖКА" (слева, ст. 2 разряда) "ЗАДЕРЖКА" (справа, 2 и 3  разряды драйвера)
}

void IndicatorPanel_SetValue (IndicatorValues *IndicatorPanel0)
{
	uint8_t num;

	uint8_t LEDsOut;

	LEDsOut = 0;

	//uint8_t gg = 11;

	// Индикатор "ВЛЕВО"
	num = separate(IndicatorPanel0->Xs,  &STLED_digVal[0]);
	IndicatorPanel_offset_4dig(num, STLED_digVal);
	STLED316S_OutData(STLED_digVal, 4, oSTLED316S_STB1_GPIO_Port, oSTLED316S_STB1_Pin);

	//STLED316S_Single_Out(&gg, 5, oSTLED316S_STB1_GPIO_Port, oSTLED316S_STB1_Pin); //Прямой вывод значения (не числа) в нужный разряд идкатора

	// Индикатор "ВПРАВО"
	num = separate(IndicatorPanel0->Xf,  &STLED_digVal[0]);
	IndicatorPanel_offset_4dig(num, STLED_digVal);
	STLED316S_OutData(STLED_digVal, 4, oSTLED316S_STB2_GPIO_Port, oSTLED316S_STB2_Pin);

	// Вывод пред и пост задержки
	num = separate(IndicatorPanel0->Delay_s, &STLED_digVal[0]);
	IndicatorPanel_offset_2dig(num, &STLED_digVal[0]);
	num = separate(IndicatorPanel0->Delay_f, &STLED_digVal[2]);
	IndicatorPanel_offset_2dig(num, &STLED_digVal[2]);
	STLED316S_OutData(STLED_digVal, 4, oSTLED316S_STB5_GPIO_Port, oSTLED316S_STB5_Pin);

	// Индикаторы "СКОРОСТЬ" (ст. 3 разряда) и "ПРОГРАММА" (мл. 3 разряда)
	num = separate(IndicatorPanel0->Speed, &STLED_digVal[0]);
	IndicatorPanel_offset_3dig(num, &STLED_digVal[0]);

	num = separate(IndicatorPanel0->Program, &STLED_digVal[3]);
	IndicatorPanel_offset_3dig(num, &STLED_digVal[3]);
	STLED316S_OutData(STLED_digVal, 6, oSTLED316S_STB3_GPIO_Port, oSTLED316S_STB3_Pin);


	if((IndicatorPanel0->LEDsState & LED_AUTO) == LED_AUTO)
	{
		LEDsOut |= 0x10;
	}

	if((IndicatorPanel0->LEDsState & LED_PARKING) == LED_PARKING)
	{
		LEDsOut |= 0x20;
	}

	if((IndicatorPanel0->LEDsState & LED_DOWN) == LED_DOWN)
	{
		LEDsOut |= 0x40;
	}

	if((IndicatorPanel0->LEDsState & LED_UP) == LED_UP)
	{
		LEDsOut |= 0x80;
	}

	//LEDsOut = 0xF0;

	STLED316S_Direct_Single_Out(&LEDsOut, 2, oSTLED316S_STB1_GPIO_Port, oSTLED316S_STB1_Pin); //Прямой вывод значения (не числа) в нужный разряд идкатора
}

void IndicatorPanel_Set_LEDs_Value (IndicatorValues *IndicatorPanel0)
{

	uint8_t LEDsOut;

	LEDsOut = 0;

	if((IndicatorPanel0->LEDsState & LED_AUTO) == LED_AUTO)
	{
		LEDsOut |= 0x10;
	}

	if((IndicatorPanel0->LEDsState & LED_PARKING) == LED_PARKING)
	{
		LEDsOut |= 0x20;
	}

	if((IndicatorPanel0->LEDsState & LED_DOWN) == LED_DOWN)
	{
		LEDsOut |= 0x40;
	}

	if((IndicatorPanel0->LEDsState & LED_UP) == LED_UP)
	{
		LEDsOut |= 0x80;
	}

	LEDsOut = 0x0F;

	STLED316S_Direct_Single_Out(&LEDsOut, 2, oSTLED316S_STB1_GPIO_Port, oSTLED316S_STB1_Pin); //Прямой вывод значения (не числа) в нужный разряд идкатора

}

void IndicatorPanel_SetBrightness (IndicatorValues *IndicatorPanel0)
{
	if (IndicatorPanel0->Brightness > 7)
	{
		IndicatorPanel0->Brightness = 7;
	}
	STLED316S_SetBrightness (IndicatorPanel0->Brightness, oSTLED316S_STB1_GPIO_Port, oSTLED316S_STB1_Pin);
	STLED316S_SetBrightness (IndicatorPanel0->Brightness, oSTLED316S_STB2_GPIO_Port, oSTLED316S_STB2_Pin);
	STLED316S_SetBrightness (IndicatorPanel0->Brightness, oSTLED316S_STB3_GPIO_Port, oSTLED316S_STB3_Pin);
	STLED316S_SetBrightness (IndicatorPanel0->Brightness, oSTLED316S_STB5_GPIO_Port, oSTLED316S_STB5_Pin);
}

int32_t separate (uint16_t in_data, uint8_t *out_data)
{
    int32_t k,i,m;
    k = in_data;
    m = 0;

    while(k != 0)       //цикл чтобы найти количество цифр в числе
    {
        k = k / 10;
        m++;
    }

    for(i=m-1;i>=0;i--)
    {
    	out_data[i] = in_data%10;      //берем последнюю цифру с числа
    	in_data = in_data / 10;         //забираем эту цифру с числа
    }

    return m;
}

void IndicatorPanel_offset_4dig (uint8_t num, uint8_t *dataArray)
{
	switch(num)
	{

	case 1:
	{
		dataArray[3] = dataArray[0];
		dataArray[2] = 0;
		dataArray[1] = 0;
		dataArray[0] = 0;
		break;
	}

	case 2:
	{
		dataArray[3] = dataArray[1];
		dataArray[2] = dataArray[0];
		dataArray[1] = 0;
		dataArray[0] = 0;
		break;
	}

	case 3:
	{
		dataArray[3] = dataArray[2];
		dataArray[2] = dataArray[1];
		dataArray[1] = dataArray[0];
		dataArray[0] = 0;

		break;
	}

	case 4:
	{
		break;
	}

default:
{
	break;
}

	}

}

void IndicatorPanel_offset_2dig (uint8_t num, uint8_t *dataArray)
{
	switch(num)
	{
	case 1:
		{
			dataArray[1] = dataArray[0];
			dataArray[0] = 0;

			break;
		}

		case 2:
		{
			break;
		}


	default:
	{
		break;
	}

	}

}


void IndicatorPanel_offset_3dig (uint8_t num, uint8_t *dataArray)
{
	switch(num)
	{

	case 1:
	{
		dataArray[2] = dataArray[0];
		dataArray[1] = 0;
		dataArray[0] = 0;
		break;
	}

	case 2:
	{
		dataArray[2] = dataArray[1];
		dataArray[1] = dataArray[0];
		dataArray[0] = 0;
		break;
	}

	case 3:
	{
		break;
	}

default:
{
	break;
}

	}

}

void Carriage_Move (uint16_t Speed, uint8_t Dir, uint8_t NumSM)
{

uint16_t freq_pwm;

	freq_pwm = ((Speed / (60/10)) * STEPS_PER_ROTATE) / DISTANCE_PER_ROTATE; // Вычисление частоты ШИМ для скорости в см/мин

	if (NumSM == 1)
	{
		MicrostepDriver_Run (freq_pwm, Dir, oSMD_EN1_GPIO_Port, oSMD_EN1_Pin, oDIR1_GPIO_Port,  oDIR1_Pin, TIM_CHANNEL_1);
	}
}

void PCB_KeyScan (void)
{
	STLED316S_ReciveData(&WelderUnit.IndicatorPanel.KeyState[0], oSTLED316S_STB5_GPIO_Port, oSTLED316S_STB5_Pin);
	STLED316S_ReciveData(&WelderUnit.IndicatorPanel.KeyState[2], oSTLED316S_STB3_GPIO_Port, oSTLED316S_STB3_Pin);
}

void PCB_InputsScan (void)
{
	if (HAL_GPIO_ReadPin(iPedal_L_GPIO_Port, iPedal_L_Pin)) // Если Нажата педаль для зажима левой части заготовки, то установить бит, иначе сбросить
	{
		WelderUnit.State |= WELDER_STATE_PEDAL_L;
		Valve_L_OPEN
	}
	else
	{
		WelderUnit.State &= ~WELDER_STATE_PEDAL_L;
		Valve_L_CLOSE
	}

	if (HAL_GPIO_ReadPin(iPedal_R_GPIO_Port, iPedal_R_Pin)) // Если Нажата педаль для зажима правой части заготовки, то установить бит, иначе сбросить
	{
		WelderUnit.State |= WELDER_STATE_PEDAL_R;
		Valve_R_OPEN
	}
	else
	{
		WelderUnit.State &= ~WELDER_STATE_PEDAL_R;
		Valve_R_CLOSE
	}

	if (HAL_GPIO_ReadPin(iBackDoor_GPIO_Port, iBackDoor_Pin)) // Если задняя дверца закрыта, то установить бит, иначе сбросить
	{
		WelderUnit.State |= WELDER_STATE_BACK_DOOR_CLOSE;
		WELDER_HEAD_UP
	}
	else
	{
		WelderUnit.State &= ~WELDER_STATE_BACK_DOOR_CLOSE;
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (HAL_GPIO_ReadPin(iSTLED316S_IRQN5_GPIO_Port, iSTLED316S_IRQN5_Pin) == GPIO_PIN_RESET)
	{
//		HAL_GPIO_WritePin(oSTLED316S_STB5_GPIO_Port, oSTLED316S_STB5_Pin, GPIO_PIN_RESET);
//		//SPI1->CR1 |= SPI_CR1_BIDIOE; // Перевод SPI в режим передачи
//		HAL_SPI_Transmit(&hspi1, STLED_Tx, 2, 1000);
//		//SPI1->CR1 &= ~SPI_CR1_BIDIOE; // // Перевод SPI в режим приема
//
//		HAL_SPI_Receive(&hspi1, STLED_Rx, 2, 1000);
//		HAL_GPIO_WritePin(oSTLED316S_STB5_GPIO_Port, oSTLED316S_STB5_Pin, GPIO_PIN_SET);
//
//		IndicatorPanel.Speed = STLED_Rx[0];
//		IndicatorPanel.Programs= STLED_Rx[1];
//		IndicatorPanel_SetValue(&IndicatorPanel);

	}



	if (HAL_GPIO_ReadPin(iFB_STEP1_GPIO_Port, iFB_STEP1_Pin) == GPIO_PIN_SET)
	{
		// В файле FreeRTOSConfig.h закомментировать
		//#define configASSERT( x ) if ((x) == 0) {taskDISABLE_INTERRUPTS(); for( ;; );}
		// Без этого не работают семафоры


		static portBASE_TYPE xHigherPriorityTaskWoken;
		  xHigherPriorityTaskWoken = pdFALSE; // Не переключать контекст

		  //WelderUnit.Position = WelderUnit.Steps * DISTANCE_PER_ROTATE / STEPS_PER_ROTATE; // Вычисление текущей позиции каретки, мм

			  xSemaphoreGiveFromISR(xSemaphore_StepCount,&xHigherPriorityTaskWoken);


		if( xHigherPriorityTaskWoken == pdTRUE )
		  {
			 /* Выдача семафора разблокирует задачу, и приоритет разблокированной
				задачи выше, чем у текущей выполняющейся задачи - поэтому контекст
				выполнения переключается принудительно в разблокированную (с более
				высоким приоритетом) задачу.
				ВН�?МАН�?Е: макрос, реально используемый для переключения контекста
				из ISR, зависит от конкретного порта FreeRTOS. Здесь указано
				имя макроса, корректное для порта Open Watcom DOS. Другие порты
				FreeRTOS могут использовать другой синтаксис. Для определения
				используемого синтаксиса обратитесь к примерам, предоставленным
				вместе с портом FreeRTOS. */
			 //portSWITCH_CONTEXT();
			//vTaskSwitchContext();
			//portYIELD_FROM_ISR(NULL);

			vTaskMissedYield(); // Переключение контекста. Нужно тестить, возможно переключение контекста не происходит

		  }
	}

	if (HAL_GPIO_ReadPin(iCarriageStop_GPIO_Port, iCarriageStop_Pin) == GPIO_PIN_SET)
	{

		// В файле FreeRTOSConfig.h закомментировать
		//#define configASSERT( x ) if ((x) == 0) {taskDISABLE_INTERRUPTS(); for( ;; );}
		// Без этого не работают семафоры

		static portBASE_TYPE xHigherPriorityTaskWoken2;
		  xHigherPriorityTaskWoken2 = pdFALSE; // Не переключать контекст

		  //WelderUnit.Position = WelderUnit.Steps * DISTANCE_PER_ROTATE / STEPS_PER_ROTATE; // Вычисление текущей позиции каретки, мм

			  xSemaphoreGiveFromISR(xSemaphore_Calibration, &xHigherPriorityTaskWoken2);

			  WelderUnit.State |= 1<<3; // 3 бит - Состояние вывода iCarriageStop, 1 - коневик нажат кареткой


		if( xHigherPriorityTaskWoken2 == pdTRUE )
		  {
			vTaskMissedYield(); // Переключение контекста. Нужно тестить, возможно переключение контекста не происходит
		  }
		//Valve1_OPEN

	}
	else
	{
		WelderUnit.State &= ~0x08; //// 3 бит - Состояние вывода iCarriageStop 0 - коневик отжат.

	}

}

void SaveProgramToFlash2()
{
	uint32_t *p; // Указатель на считваемые из flash памяти данные

	uint16_t offset = 0;

	p = (uint32_t*)PAGE; // p содержит начальный адрес записи параметров программы

	for(uint16_t i = 0; i < 256; i++)
	{
		ReadFlashPage[i] = *(p + i);
	}

	offset = WelderUnit.Program * 6;

	ReadFlashPage[offset] = WelderUnit.Program;
	ReadFlashPage[offset + 1] = WelderUnit.Xs;
	ReadFlashPage[offset + 2] = WelderUnit.Xf;
	ReadFlashPage[offset + 3] = WelderUnit.Speed;
	ReadFlashPage[offset + 4] = WelderUnit.Delay_s;
	ReadFlashPage[offset + 5] = WelderUnit.Delay_f;

	STM32_WriteToFlash3(ReadFlashPage, 64);

	p = (uint32_t*)PAGE + offset/4; // p содержит начальный адрес записи параметров программы

}

void ReadProgramFromFlash2()
{
	uint32_t *p; // Указатель на считваемые из flash памяти данные

	uint16_t offset = 0;

	p = (uint32_t*)PAGE; // p содержит начальный адрес записи параметров программы

	for(uint16_t i = 0; i < 256; i++)
	{
		ReadFlashPage[i] = *(p + i);
	}

	offset = WelderUnit.Program * 6;

	if (WelderUnit.Program - ReadFlashPage[offset] <= 1 || ReadFlashPage[offset] - WelderUnit.Program <= 1)
	{
		if (ReadFlashPage[offset] <= PROGRAM_MAX)
		{
			WelderUnit.Program = ReadFlashPage[offset];
			WelderUnit.Xs = ReadFlashPage[offset + 1];
			WelderUnit.Xf = ReadFlashPage[offset + 2];
			WelderUnit.Speed = ReadFlashPage[offset + 3];
			WelderUnit.Delay_s = ReadFlashPage[offset + 4];
			WelderUnit.Delay_f = ReadFlashPage[offset + 5];
		}


	}
	else
	{
//		//WelderUnit.Program = ReadFlashPage[offset];
//		WelderUnit.Xs = 0;
//		WelderUnit.Xf = 0;
//		WelderUnit.Speed = 0;
//		WelderUnit.Delay_s = 0;
//		WelderUnit.Delay_f = 0;
	}





//	STM32_WriteToFlash3(ReadFlashPage, 64);
//
//	p = (uint32_t*)PAGE + offset/4; // p содержит начальный адрес записи параметров программы

}




void ReadProgramFromFlash()
{
	uint64_t ReadStart; // Переменная для хранения считанных из flash памяти данных. Для поиска заголовка сохраненных данных

	uint64_t ReadEnd; // Для поиска стоп байта сохраненных данных

	uint32_t *p; // Указатель на считваемые из flash памяти данные

	uint16_t offset;

	offset = WelderUnit.Program * 32;


	for(;;)
	{

	p = (uint32_t*)PAGE + offset/4; // p содержит начальный адрес записи параметров программы

	// запись данных из flash
	ReadStart = *p; // Препологаемое начало пакета

	p = (uint32_t*)(PAGE + 28) + offset/4; // Смещенеи до предпологаемого конца пакета

	ReadEnd = *p; // Предпологаемый конец пакета

	if (ReadStart == 0xAAAA && ReadEnd == 0x5555)
	{
		if (*(p+4) == WelderUnit.Program)
		{

		}

	}

	offset = offset + 32;

	}

}

void PCB_LEDs_OUT (uint8_t LEDs_val)
{
	STLED_digVal[4] = LEDs_val;

}

