#include "main.h"

#define PAGE_ADR                   ((uint32_t)0x08007C00) // адрес страницы для записи
#define SIZE_DATA_FLASH            3                      // сколько переменных считать из памяти

struct _flash flash;

void unlook_flash (void) {               // последовательность для разблокировки памяти
	  FLASH->KEYR = FLASH_KEY1;
	  FLASH->KEYR = FLASH_KEY2;
}
// he
void erase_flash (void) {
	unlook_flash ();
	SET_BIT(FLASH->CR, FLASH_CR_PER);     // установим бит стирания
	FLASH->AR = PAGE_ADR;                 // запишем страницу для стирания
	SET_BIT(FLASH->CR, FLASH_CR_STRT);    // запустим стирание
	while (!(FLASH->SR & FLASH_SR_EOP));  //Дождемся поднятия флага о готовности
	CLEAR_BIT(FLASH->CR, FLASH_CR_PER);   // сбросим бит стирания
	FLASH->SR |= FLASH_SR_EOP;
	SET_BIT(FLASH->CR, FLASH_CR_LOCK);    // заблокируем память
}

void devide_32_to_16 (int16_t* adr_flash, int32_t *data) {
	*adr_flash =  (int16_t) *data & 0xFFFF;
    adr_flash ++;
    *adr_flash = (int16_t) (*data >> 16) & 0xFFFF;
}

void write_flash (void) {                // запись в флешу
	int32_t* point_data = &flash.gyrX_zero;
	erase_flash ();
	unlook_flash ();
	SET_BIT(FLASH->CR, FLASH_CR_PG);     // установим бит записи
    int16_t* adr_flash1 = (int16_t*) PAGE_ADR;
	for (int i = 0; i < SIZE_DATA_FLASH; i++){
		devide_32_to_16 (adr_flash1, point_data + i);
		adr_flash1 += 2;
	}
    while (!(FLASH->SR & FLASH_SR_EOP));  //Дождемся поднятия флага о готовности
	CLEAR_BIT(FLASH->CR, FLASH_CR_PG);    // сбросим бит записи
	SET_BIT(FLASH->CR, FLASH_CR_LOCK);    // заблокируем память
}

void read_from_flash (void){                     // считать данные из флеша
	flash.gyrX_zero = *((uint32_t*) PAGE_ADR);
	flash.gyrY_zero = *((uint32_t*) PAGE_ADR + 1);
	flash.gyrZ_zero = *((uint32_t*) PAGE_ADR + 2);
	if (flash.gyrX_zero == 0xFFFFFFFF) flash.gyrX_zero = 0;
	if (flash.gyrY_zero == 0xFFFFFFFF) flash.gyrY_zero = 0;
	if (flash.gyrZ_zero == 0xFFFFFFFF) flash.gyrZ_zero = 0;
}

void write_to_struct (uint32_t X, uint32_t Y, uint32_t Z) {
	flash.gyrX_zero = X;
	flash.gyrY_zero = Y;
	flash.gyrZ_zero = Z;
}

