#include "main.h"
#include "STM32_flash.h"


void STM32_WriteToFlash (void)
{
	   HAL_FLASH_Unlock();

		 FLASH_EraseInitTypeDef EraseInitStruct;
	   uint32_t PAGEError = 0;
	   EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;  // Тип стирания. FLASH_TYPEERASE_PAGES стирание одиночной страницы
	   EraseInitStruct.PageAddress = PAGE; // Адрес первой стираемой ячейки в странице
	   EraseInitStruct.NbPages     = 1;       // Количесвто стираемых страниц
	   HAL_FLASHEx_Erase(&EraseInitStruct,&PAGEError);

	   //Запись начиная с адреса 0x803F800
		 for(uint16_t i = 0; i < 2048; i++)
		 {
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, PAGE + (i * 4), i);  // FLASH_TYPEPROGRAM_WORD - запись 32 битного числа, адрес записи, данные
		 }

		 HAL_FLASH_Lock();

}

void STM32_WriteToFlash2 (uint16_t *Data, uint32_t Len, uint32_t offset)
{
	   HAL_FLASH_Unlock();

		FLASH_EraseInitTypeDef EraseInitStruct;
	   uint32_t PAGEError = 0;
	   EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;  // Тип стирания. FLASH_TYPEERASE_PAGES стирание одиночной страницы
	   EraseInitStruct.PageAddress = PAGE; // Адрес первой стираемой ячейки в странице
	   EraseInitStruct.NbPages     = 1;       // Количесвто стираемых страниц
	   //HAL_FLASHEx_Erase(&EraseInitStruct,&PAGEError);

	   //Запись начиная с адреса 0x803F800
		 for(uint16_t i = 0; i < Len; i++)
		 {
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, PAGE + (i * 4) + offset, Data[i]);  // FLASH_TYPEPROGRAM_WORD - запись 32 битного числа, адрес записи, данные
		 }

		 HAL_FLASH_Lock();
}

void STM32_WriteToFlash3 (uint64_t *Data, uint32_t Len)
{
	   HAL_FLASH_Unlock();

		FLASH_EraseInitTypeDef EraseInitStruct;
	   uint32_t PAGEError = 0;
	   EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;  // Тип стирания. FLASH_TYPEERASE_PAGES стирание одиночной страницы
	   EraseInitStruct.PageAddress = PAGE; // Адрес первой стираемой ячейки в странице
	   EraseInitStruct.NbPages     = 1;       // Количесвто стираемых страниц
	   HAL_FLASHEx_Erase(&EraseInitStruct,&PAGEError);

	   //Запись начиная с адреса 0x803F800
		 for(uint16_t i = 0; i < Len; i++)
		 {
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, PAGE + (i*4), Data[i]);  // FLASH_TYPEPROGRAM_WORD - запись 32 битного числа, адрес записи, данные
		 }

		 HAL_FLASH_Lock();
}
