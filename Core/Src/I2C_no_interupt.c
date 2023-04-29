#include "main.h"

extern volatile uint32_t Timeout_counter_ms;


void CMSIS_I2C_Reset(void) {
	//п.п. 26.6.1 I2C Control register 1 (I2C_CR1) (стр. 772)
	SET_BIT(I2C2->CR1, I2C_CR1_SWRST); //: I2C Peripheral not under reset
	while (READ_BIT(I2C2->CR1, I2C_CR1_SWRST) == 0) ;
	CLEAR_BIT(I2C2->CR1, I2C_CR1_SWRST); //: I2C Peripheral not under reset
	while (READ_BIT(I2C2->CR1, I2C_CR1_SWRST)) ;
	/* Примечание: Этот бит можно использовать для повторной инициализации
	 * периферийного устройства после ошибки или заблокированного состояния.
	 * Например, если бит BUSY установлен и остается заблокированным из-за сбоя на шине,
	 * бит SWRST можно использовать для выхода из этого состояния.*/
}


void CMSIS_I2C2_Init(void) {
	//Настройки тактирования
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN); //Запуск тактирование порта B
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN); //Запуск тактирования альтернативных функций
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C2EN); //Запуск тактирования I2C1

	//Настройки ножек SDA и SCL
	//PB11 SDA (I2C Data I/O) Alternate function open drain
	MODIFY_REG(GPIOB->CRH, GPIO_CRH_CNF11_Msk, 0b11 << GPIO_CRH_CNF11_Pos); //Alternate function open drain
	MODIFY_REG(GPIOB->CRH, GPIO_CRH_MODE11_Msk, 0b11 << GPIO_CRH_MODE11_Pos); //Maximum output speed 50 MHz
	//PB10 SCL (I2C clock) Alternate function open drain
	MODIFY_REG(GPIOB->CRH, GPIO_CRH_CNF10_Msk, 0b11 << GPIO_CRH_CNF10_Pos); //Alternate function open drain
	MODIFY_REG(GPIOB->CRH, GPIO_CRH_MODE10_Msk, 0b11 << GPIO_CRH_MODE10_Pos); //Maximum output speed 50 MHz

	CMSIS_I2C_Reset();

	I2C2->CR1= 0;                            // очистим регистр
	I2C2->CR2=(((APB1_Clock/1000000ul)<<I2C_CR2_FREQ_Pos)&I2C_CR2_FREQ_Msk) ;// на какой скорости работает шина I2C2
	//I2C2->CCR= 0 | ((((SystemCoreClock+1000*I2C_BUS_FREQ_KHZ)/1000/I2C_BUS_FREQ_KHZ/2)<<I2C_CCR_CCR_Pos)&I2C_CCR_CCR_Msk); //скорость шины из даташита ??
	I2C2->CCR =  (APB1_Clock / (I2C_BUS_FREQ_KHZ*1000*3)) ;    //((1/(I2C_BUS_FREQ_KHZ*1000))*3/(1/SystemCoreClock));
	SET_BIT(I2C2->CCR, I2C_CCR_FS);           //Fast mode I2C
	CLEAR_BIT(I2C2->CCR, I2C_CCR_DUTY);       //Fm mode tlow/thigh = 2
	//SET_BIT(I2C2->CCR, I2C_CCR_DUTY);       //Fm mode tlow/thigh = 16/9 (see CCR)
	I2C2->TRISE=APB1_Clock/3333333ul+1;       //1000000 или 3333333 это скорость нарастания от частоты  1000нс для стандарт, 300нс для фаст (такие цифры потому что
	I2C2->OAR1=0 ;                            // 300нс это частота 3333333Гц, а 1000нс это 1000000Гц)
	I2C2->OAR2= 0;
	SET_BIT(I2C2->CR1, I2C_CR1_PE);          // включим I2C2

	//Расчет CCR. Смотри примеры расчета
	//MODIFY_REG(I2C1->CCR, I2C_CCR_CCR_Msk, 180 << I2C_CCR_CCR_Pos); //для Sm mode
	//MODIFY_REG(I2C1->CCR, I2C_CCR_CCR_Msk, 30 << I2C_CCR_CCR_Pos); //для Fm mode. DUTY 0.
	//MODIFY_REG(I2C1->CCR, I2C_CCR_CCR_Msk, 4 << I2C_CCR_CCR_Pos); //для Fm mode. DUTY 1.

	//п.п. 26.6.9 I2C TRISE register (I2C_TRISE)(стр. 782)
	//MODIFY_REG(I2C1->TRISE, I2C_TRISE_TRISE_Msk, 37 << I2C_TRISE_TRISE_Pos); //для Sm mode
	//MODIFY_REG(I2C1->TRISE, I2C_TRISE_TRISE_Msk, 12 << I2C_TRISE_TRISE_Pos); //для Fm mode

}

/**
 **************************************************************************************************
 *  @breif Функция записи в память по указанному адресу
 *  @param  *I2C - шина I2C
 *  @param  Adress_Device - Адрес устройства
 *  @param  Adress_data - Адрес в памяти, куда будем записывать данные
 *  @param  Size_adress - Размер адреса в байтах. Пример: 1 - 8 битный адрес. 2 - 16 битный адрес.
 *  @param  *data - Данные, которые будем записывать
 *  @param  Size_data - Размер, сколько байт будем записывать.
 *  @retval  Возвращает статус записи. True - Успешно. False - Ошибка.
 **************************************************************************************************
 */
uint8_t CMSIS_I2C_MemWrite(I2C_TypeDef *I2C, uint8_t Adress_Device, uint16_t Adress_data, uint8_t Size_adress, uint8_t* data, uint16_t Size_data, uint32_t Timeout_ms) {

	/*-------------------Проверка занятости шины-------------------*/
	if (READ_BIT(I2C->SR2, I2C_SR2_BUSY)) {
		//Если шина занята

		if ((READ_BIT(GPIOB->IDR, GPIO_IDR_IDR10)) && (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR11))) {
			//Если линия на самом деле свободна, а BUSY висит
			CMSIS_I2C_Reset(); //ресет
			CMSIS_I2C2_Init(); //повторная инициализация
		}

		if (READ_BIT(I2C->SR2, I2C_SR2_MSL)) {
			//Если стоит статус, что мы в мастере
			SET_BIT(I2C->CR1, I2C_CR1_STOP); //Отправим сигнал STOP
		}

		if (I2C->CR1 != 1) {
			//Если в CR1 что-то лишнее, то перезагрузим I2C
			CLEAR_BIT(I2C->CR1, I2C_CR1_PE);
			SET_BIT(I2C->CR1, I2C_CR1_PE);
		}

		return false;
	}
	/*-------------------Проверка занятости шины-------------------*/

	CLEAR_BIT(I2C->CR1, I2C_CR1_POS); //Бит ACK управляет (N)ACK текущего байта, принимаемого в сдвиговом регистре. ??
	SET_BIT(I2C->CR1, I2C_CR1_START); //Стартуем.

	Timeout_counter_ms = Timeout_ms;
	while (READ_BIT(I2C->SR1, I2C_SR1_SB) == 0) {
		//Ожидаем до момента, пока не сработает Start condition generated

		if (!Timeout_counter_ms) {
			return false;
		}

	}
	//ВНИМАНИЕ!
	/* Бит I2C_SR1_SB очищается программно путем чтения регистра SR1 с последующей записью в регистр DR или когда PE=0*/
	I2C->SR1;
	I2C->DR = (Adress_Device << 1); //Адрес + Write

	Timeout_counter_ms = Timeout_ms;
	while ((READ_BIT(I2C->SR1, I2C_SR1_AF) == 0) && (READ_BIT(I2C->SR1, I2C_SR1_ADDR) == 0)) {
		//Ждем, пока адрес отзовется

		if (!Timeout_counter_ms) {
			return false;
		}

	}
	if (READ_BIT(I2C->SR1, I2C_SR1_ADDR)) {
		//Если устройство отозвалось, сбросим бит ADDR
		/*Сброс бита ADDR производится чтением SR1, а потом SR2*/
		I2C->SR1;
		I2C->SR2;

		/*Отправим адрес памяти*/
		for (uint16_t i = 0; i < Size_adress; i++) {
			I2C->DR = *((uint8_t*)&Adress_data + (Size_adress - 1 - i)); //Запись байта
			while (READ_BIT(I2C->SR1, I2C_SR1_TXE) == 0) {
				//Ждем, пока данные загрузятся в регистр сдвига.

				if ((READ_BIT(I2C->SR1, I2C_SR1_AF) == 1)) {
					//Если устройство не отозвалось, прилетит 1 в I2C_SR1_AF
					SET_BIT(I2C->CR1, I2C_CR1_STOP); //Останавливаем
					CLEAR_BIT(I2C->SR1, I2C_SR1_AF); //Сбрасываем бит AF
					return false;
				}
			}
		}

		/*Будем записывать данные в ячейку памяти, начиная с указанного адреса*/
		for (uint16_t i = 0; i < Size_data; i++) {
			I2C->DR = *(data + i); //Запись байта
			while (READ_BIT(I2C->SR1, I2C_SR1_TXE) == 0) {
				//Ждем, пока данные загрузятся в регистр сдвига.

				if ((READ_BIT(I2C->SR1, I2C_SR1_AF) == 1)) {
					//Если устройство не отозвалось, прилетит 1 в I2C_SR1_AF
					SET_BIT(I2C->CR1, I2C_CR1_STOP); //Останавливаем
					CLEAR_BIT(I2C->SR1, I2C_SR1_AF); //Сбрасываем бит AF
					return false;
				}
			}
		}

		SET_BIT(I2C->CR1, I2C_CR1_STOP); //Останавливаем

		return true;

	} else {
		//Если устройство не отозвалось, прилетит 0 в I2C_SR1_ADDR
		SET_BIT(I2C->CR1, I2C_CR1_STOP); //Останавливаем
		//CLEAR_BIT(I2C->SR1, I2C_SR1_AF); //Сбрасываем бит AF

		return false;
	}
}


/**
 **************************************************************************************************
 *  @breif Функция чтения из памяти по указанному адресу
 *  @param  *I2C - шина I2C
 *  @param  Adress_Device - Адрес устройства
 *  @param  Adress_data - Адрес в памяти, откуда будем считывать данные
 *  @param  Size_adress - Размер адреса в байтах. Пример: 1 - 8 битный адрес. 2 - 16 битный адрес.
 *  @param  *data - Данные, в которые будем записывать считанную информацию.
 *  @param  Size_data - Размер, сколько байт будем считывать.
 *  @retval  Возвращает статус считывания. True - Успешно. False - Ошибка.
 **************************************************************************************************
 */
uint8_t CMSIS_I2C_MemRead(I2C_TypeDef *I2C, uint8_t Adress_Device, uint16_t Adress_data, uint8_t Size_adress, uint8_t* data, uint16_t Size_data, uint32_t Timeout_ms) {

	/*-------------------Проверка занятости шины-------------------*/
	if (READ_BIT(I2C->SR2, I2C_SR2_BUSY)) {
		//Если шина занята

		if ((READ_BIT(GPIOB->IDR, GPIO_IDR_IDR10)) && (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR11))) {
			//Если линия на самом деле свободна, а BUSY висит
			CMSIS_I2C_Reset(); //ресет
			CMSIS_I2C2_Init(); //повторная инициализация
		}

		if (READ_BIT(I2C->SR2, I2C_SR2_MSL)) {
			//Если стоит статус, что мы в мастере
			SET_BIT(I2C->CR1, I2C_CR1_STOP); //Отправим сигнал STOP
		}

		if (I2C->CR1 != 1) {
			//Если в CR1 что-то лишнее, то перезагрузим I2C
			CLEAR_BIT(I2C->CR1, I2C_CR1_PE);
			SET_BIT(I2C->CR1, I2C_CR1_PE);
		}

		return false;
	}
	/*-------------------Проверка занятости шины-------------------*/

	CLEAR_BIT(I2C->CR1, I2C_CR1_POS); //Бит ACK управляет (N)ACK текущего байта, принимаемого в сдвиговом регистре.
	SET_BIT(I2C->CR1, I2C_CR1_START); //Стартуем.

	Timeout_counter_ms = Timeout_ms;
	while (READ_BIT(I2C->SR1, I2C_SR1_SB) == 0) {
		//Ожидаем до момента, пока не сработает Start condition generated

		if (!Timeout_counter_ms) {
			return false;
		}

	}
	//ВНИМАНИЕ!
	/* Бит I2C_SR1_SB очищается программно путем чтения регистра SR1 с последующей записью в регистр DR или когда PE=0*/
	I2C->SR1;
	I2C->DR = (Adress_Device << 1); //Адрес + команда Write

	Timeout_counter_ms = Timeout_ms;
	while ((READ_BIT(I2C->SR1, I2C_SR1_AF) == 0) && (READ_BIT(I2C->SR1, I2C_SR1_ADDR) == 0)) {
		//Ждем, пока адрес отзовется

		if (!Timeout_counter_ms) {
			return false;
		}

	}

	if (READ_BIT(I2C->SR1, I2C_SR1_ADDR)) {
		//Если устройство отозвалось, сбросим бит ADDR
        /*Сброс бита ADDR производится чтением SR1, а потом SR2*/
		I2C->SR1;
		I2C->SR2;

		/*Отправим адрес памяти*/
		for (uint16_t i = 0; i < Size_adress; i++) {
			I2C->DR = *((uint8_t*)&Adress_data + (Size_adress - 1 - i)); //Запись байта
			while (READ_BIT(I2C->SR1, I2C_SR1_TXE) == 0) {
				//Ждем, пока данные загрузятся в регистр сдвига.

				if ((READ_BIT(I2C->SR1, I2C_SR1_AF) == 1)) {
					//Если устройство не отозвалось, прилетит 1 в I2C_SR1_AF
					SET_BIT(I2C->CR1, I2C_CR1_STOP); //Останавливаем
					CLEAR_BIT(I2C->SR1, I2C_SR1_AF); //Сбрасываем бит AF
					return false;
				}
			}
		}

		//Повторный старт
		SET_BIT(I2C->CR1, I2C_CR1_START); //Стартуем.

		Timeout_counter_ms = Timeout_ms;
		while (READ_BIT(I2C->SR1, I2C_SR1_SB) == 0) {
			//Ожидаем до момента, пока не сработает Start condition generated

			if (!Timeout_counter_ms) {
				return false;
			}

		}
		//ВНИМАНИЕ!
		/* Бит I2C_SR1_SB очищается программно путем чтения регистра SR1 с последующей записью в регистр DR или когда PE=0*/
		I2C->SR1;
		I2C->DR = (Adress_Device << 1 | 1); //Адрес + команда Read

		Timeout_counter_ms = Timeout_ms;
		while ((READ_BIT(I2C->SR1, I2C_SR1_AF) == 0) && (READ_BIT(I2C->SR1, I2C_SR1_ADDR) == 0)) {
			//Ждем, пока адрес отзовется

			if (!Timeout_counter_ms) {
				return false;
			}

		}

		if (READ_BIT(I2C->SR1, I2C_SR1_ADDR)) {
			//Если устройство отозвалось, сбросим бит ADDR
			/*Сброс бита ADDR производится чтением SR1, а потом SR2*/
			I2C->SR1;
			I2C->SR2;

			/*Прочтем данные, начиная с указанного адреса*/
			for (uint16_t i = 0; i < Size_data; i++) {
				if (i < Size_data - 1) {
					SET_BIT(I2C->CR1, I2C_CR1_ACK); //Если мы хотим принять следующий байт, то отправляем ACK
					while (READ_BIT(I2C->SR1, I2C_SR1_RXNE) == 0) ;
					*(data + i) = I2C->DR; //Чтение байта
				} else {
					CLEAR_BIT(I2C->CR1, I2C_CR1_ACK); //Если мы знаем, что следующий принятый байт будет последним, то отправим NACK

					SET_BIT(I2C->CR1, I2C_CR1_STOP); //Останавливаем
					while (READ_BIT(I2C->SR1, I2C_SR1_RXNE) == 0) ; //Подождем, пока сдвиговый регистр пополнится новым байтом данных
					*(data + i) = I2C->DR; //Чтение байта
				}
			} return true;

		} else {
			//Если устройство не отозвалось, прилетит 0 в I2C_SR1_ADDR
			SET_BIT(I2C->CR1, I2C_CR1_STOP); //Останавливаем
			//CLEAR_BIT(I2C->SR1, I2C_SR1_AF); //Сбрасываем бит AF
			return false;
		}

	} else {
		//Если устройство не отозвалось, прилетит 0 в I2C_SR1_ADDR
		SET_BIT(I2C->CR1, I2C_CR1_STOP); //Останавливаем
		//CLEAR_BIT(I2C->SR1, I2C_SR1_AF); //Сбрасываем бит AF
		return false;
	}
}
