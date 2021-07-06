#include  "MicrostepDriver.h"


void MicrostepDriver_Ini(void)
{
    htim3.Init.Prescaler = 7199; // Предделитель таймера 3
    htim3.Init.Period = 10000; // Период ШИМа
    TIM3->CCR1 = 5000; // Длительность импульса
    HAL_TIM_Base_Init(&htim3);
}


void MicrostepDriver_Run (uint16_t freq, uint8_t dir, GPIO_TypeDef *EN1_Port, uint16_t EN1_Pin, GPIO_TypeDef *DIR1_Port, uint16_t DIR1_Pin, uint32_t PWM_Channel)
{
	if (freq != 0)
	{

	htim3.Init.Period = (htim3.Init.Prescaler /  freq) / MICRO_STEP_DEV; // Расчет периода следования импульсов
	TIM3->CCR1 = htim3.Init.Period / 2; // Расчет коэффицента заполнения
	HAL_TIM_Base_Init(&htim3);

	if (dir == 0) // Направление вращения
	{
		HAL_GPIO_WritePin(DIR1_Port, DIR1_Pin, GPIO_PIN_RESET);

	}
	else
	{
		HAL_GPIO_WritePin(DIR1_Port, DIR1_Pin, GPIO_PIN_SET);
	}


	HAL_TIM_PWM_Start(&htim3, PWM_Channel); // Начало генерации ШИМ сигнала

	}
	else // Если частота задана как 0, то останов ШД
	{
		HAL_TIM_PWM_Stop(&htim3, PWM_Channel);
	}

}
