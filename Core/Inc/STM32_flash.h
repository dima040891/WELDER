#ifndef STM32_FLASH_H
#define STM32_FLASH_H


#define MAX_PAGES 128 // Всего страниц
#define SIZE_PAGE 2048 // Размер страницы (количество двойных слов - ячейка 64 бита)
#define BASE_ADDRES 0x8000000 // Адрес страрта

#define N_PAGES 127 // Интересуемая страница для записи/стираия

#define PAGE (BASE_ADDRES + (N_PAGES * SIZE_PAGE)) // Адрес первой ячейки в странице кторую необходимо очистить/записать

void STM32_WriteToFlash (void);
void STM32_WriteToFlash2 (uint16_t *Data, uint32_t Len, uint32_t AddresMemory);

void STM32_WriteToFlash3 (uint64_t *Data, uint32_t Len);

#endif
