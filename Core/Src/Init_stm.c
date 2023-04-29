
#include "main.h"

volatile uint32_t SysTimer_ms = 0; //Переменная, аналогичная HAL_GetTick()
volatile uint32_t Delay_counter_ms = 0; //Счетчик для функции Delay_ms
volatile uint32_t Timeout_counter_ms = 0; //Переменная для таймаута функций
volatile uint32_t time_game = 0;          //Переменная для времени игры
//volatile uint16_t sound_delay = 0;        // для звука
volatile uint16_t over_locking_time = 0;    // для разгона / торможения
extern struct flags flag;                   // флаги
uint16_t time_to_next_game = 0;             // время между играми
uint16_t provoke = 0;                       // для провоцирования кота если он не трогает игрушку
/*
 ***************************************************************************************
 *  @breif Настройка МК STM32F103C8T6 на частоту 72MHz от внешнего кварцевого резонатора
 *  Внешний кварцевый резонатор на 8 MHz
 *  ADC настроен на 12MHz
 *  USB настроен на 48MHz
 *  MCO подключен к PLL и тактируется от 36MHz
 *  Reference Manual/см. п.7.3 RCC registers (стр. 99)
 *  В настройке также необходимо настроить FLASH на работу, совместимую с 72MHz:
 *  Reference Manual/см. п.3.2 Memory organization (стр. 49)
 ***************************************************************************************
 */

void CMSIS_RCC_SystemClock_72MHz(void) {
	SET_BIT(RCC->CR, RCC_CR_HSION); //Запустим внутренний RC генератор на 8 МГц
	while (READ_BIT(RCC->CR, RCC_CR_HSIRDY) == 0); //Дождемся поднятия флага о готовности

	SET_BIT(RCC->CR, RCC_CR_HSEON); //Запустим внешний кварцевый резонатор. Он у нас на 8 MHz.
	while (READ_BIT(RCC->CR, RCC_CR_HSERDY) == 0); //Дождемся поднятия флага о готовности

	MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_HSE); //Выберем HSE в качестве System Clock(PLL лучше пока не выбирать, он у нас отключен)
	SET_BIT(FLASH->ACR, FLASH_ACR_PRFTBE); //Prefetch is enabled(В Cube MX включено и я включил...)
	MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_1); //010 Two wait states, if 48 MHz < SYSCLK <= 72 MHz

	MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1);     //APB Prescaler /1
	MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV2);   //APB1 Prescaler /2, т.к. PCLK1 max 36MHz
	MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV1);   //APB2 Prescaler /1. Тут нас ничего не ограничивает. Будет 72MHz.

	CLEAR_BIT(RCC->CFGR, RCC_CFGR_PLLXTPRE_HSE); //Никакое предделение перед PLL нам не нужно. Поэтому /1.
	SET_BIT(RCC->CFGR, RCC_CFGR_PLLSRC);         //В качестве входного сигнала для PLL выберем HSE
	MODIFY_REG(RCC->CFGR, RCC_CFGR_PLLMULL, RCC_CFGR_PLLMULL9); //т.к. кварц у нас 8Mhz, а нам нужно 72MHz, то в PLL нужно сделать умножение на 9. 8MHz * 9 = 72MHz.

	SET_BIT(RCC->CR, RCC_CR_CSSON);                       //Включим CSS
	SET_BIT(RCC->CR, RCC_CR_PLLON);                       //Запустим PLL
	while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) == 0);        //Дожидемся поднятия флага включения PLL
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);  //Выберем PLL в качестве System Clock


	//MODIFY_REG(RCC->CFGR, RCC_CFGR_ADCPRE, RCC_CFGR_ADCPRE_DIV6); //ADC Prescaler /6, чтоб было 12MHz, т.к. максимальная частота тут 14 MHz
    //CLEAR_BIT(RCC->CFGR, RCC_CFGR_USBPRE); //Для USB 72MHz/1.5 = 48MHz
	//MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO, RCC_CFGR_MCO_PLLCLK_DIV2); //В качестве тактирования для MCO выбрал PLL. Будет 36 MHz.
}

/**
 ***************************************************************************************
 *  @breif Настройка SysTick на микросекунды
 *  На этом таймере мы настроим Delay и аналог HAL_GetTick()
 *  PM0056 STM32F10xxx/20xxx/21xxx/L1xxxx Cortex®-M3 programming manual/
 *  см. п.4.5 SysTick timer (STK) (стр. 150)
 ***************************************************************************************
 */
void CMSIS_SysTick_Timer_init(void) {
	/* п. 4.5.1 SysTick control and status register (STK_CTRL) (стр. 151)*/

	CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk); //Выключим таймер для проведения настроек.
	SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk); //Разрешим прерывания по таймеру
	SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk); //Выберем без делителя. Будет 72MHz
	MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk,
			71999 << SysTick_LOAD_RELOAD_Pos); //Настроим прерывание на частоту в 1 кГц(т.е. сработка будет каждую мс)
	MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk,
			71999 << SysTick_VAL_CURRENT_Pos); //Начнем считать с 71999
	SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk); //Запускаем таймер

}

/**
 ***************************************************************************************
 *  @breif Настройка Delay и аналог HAL_GetTick()
 ***************************************************************************************
 */

/**
 ******************************************************************************
 *  @breif Delay_ms
 *  @param   uint32_t Milliseconds - Длина задержки в миллисекундах
 ******************************************************************************
 */
void Delay_ms(uint32_t Milliseconds) {
	Delay_counter_ms = Milliseconds;
	while (Delay_counter_ms != 0);
}

void delay(uint32_t Milliseconds) {
	Delay_counter_ms = (uint32_t) Milliseconds;
	while (Delay_counter_ms != 0);
}

/**
 ******************************************************************************
 *  @breif Прерывание по флагу COUNTFLAG (см. п. 4.5.1 SysTick control and status register (STK_CTRL))
 *  Список векторов(прерываний) можно найти в файле startup_stm32f103c8tx.S
 ******************************************************************************
 */
void SysTick_Handler(void) {
	SysTimer_ms++;

	if (Delay_counter_ms)       Delay_counter_ms--;      // задеркжка
	if (Timeout_counter_ms)	    Timeout_counter_ms--;    // максимальное время опроса датчика
	if (time_game )             time_game --;            // время до конца игры
	if (time_game == 1)         flag.time_of = true;     // чтоб остановить игру 1 раз, так как если будет 0 останавливать будет каждый раз
	if (flag.start_polling)     flag.start_polling --;   // старт опроса, циклично запускать
	if (time_to_next_game)      time_to_next_game --;    // время для паузы между играми
	if (provoke)                provoke --;              // провокация)
	else flag.mov_provoke       = true;



	/*if (flag.sound) {
		if (sound_delay)            sound_delay --;
    	else {
    		treck_go (flag.sound);
    	}
	}*/


}

/**
 ***************************************************************************************
 *  @breif Инициализация PIN PC13 на выход в режиме Push-Pull с максимальной скоростью 50 MHz
 *  Reference Manual/см. п.9.2 GPIO registers (стр. 171)
 *  Перед настройкой (GPIOs and AFIOs) нужно включить тактирование порта.
 ***************************************************************************************
 */
void CMSIS_PC13_OUTPUT_Push_Pull_init(void) {
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPCEN); //Запуск тактирования порта C
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN); //Запуск тактирования порта B
	MODIFY_REG(GPIOC->CRH, GPIO_CRH_MODE13, 0b11 << GPIO_CRH_MODE13_Pos); //Настройка GPIOC порта 13 на выход со максимальной скоростью в 50 MHz
	MODIFY_REG(GPIOC->CRH, GPIO_CRH_CNF13, 0b00 << GPIO_CRH_CNF13_Pos); //Настройка GPIOC порта 13 на выход в режиме Push-Pull

	MODIFY_REG(GPIOB->CRL, GPIO_CRL_MODE1, 0b11 << GPIO_CRL_MODE1_Pos); //Настройка GPIOB порта 1 на выход со максимальной скоростью в 50 MHz
	MODIFY_REG(GPIOB->CRL, GPIO_CRL_CNF1, 0b00 << GPIO_CRL_CNF1_Pos); //Настройка GPIOB порта 1 на выход в режиме Push-Pull
	MODIFY_REG(GPIOB->CRL, GPIO_CRL_MODE0, 0b11 << GPIO_CRL_MODE0_Pos); //Настройка GPIOB порта 0 на выход со максимальной скоростью в 50 MHz
	MODIFY_REG(GPIOB->CRL, GPIO_CRL_CNF0, 0b00 << GPIO_CRL_CNF0_Pos); //Настройка GPIOB порта 0 на выход в режиме Push-Pull
}

void CMSIS_B12_Input_init(void) {
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN); //Запуск тактирования порта B
	MODIFY_REG(GPIOB->CRH, GPIO_CRH_CNF12, 0b01 << GPIO_CRH_CNF12_Pos); //Настройка GPIOB порта 13 на вход
	SET_BIT(GPIOB -> BSRR, GPIO_BSRR_BS12);    // подтяжка к пул ап
}

void PWM_Out (void) {                            // настроим ШИМ
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN); //Запуск тактирования альтернативных функций
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN); //Запуск тактирования порта A
	MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE8, 0b11 << GPIO_CRH_MODE8_Pos); //Настройка GPIOA порта 8 на выход со максимальной скоростью в 50 MHz
	MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF8, 0b10 << GPIO_CRH_CNF8_Pos); //Настройка GPIOA порта 8 на выход в режиме Push-Pull
	MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE9, 0b11 << GPIO_CRH_MODE9_Pos); //Настройка GPIOA порта 9 на выход со максимальной скоростью в 50 MHz
	MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF9, 0b10 << GPIO_CRH_CNF9_Pos); //Настройка GPIOA порта 9 на выход в режиме Push-Pull
	MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE10, 0b11 << GPIO_CRH_MODE10_Pos); //Настройка GPIOA порта 10 на выход со максимальной скоростью в 50 MHz
	MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF10, 0b10 << GPIO_CRH_CNF10_Pos); //Настройка GPIOA порта 10 на выход в режиме Push-Pull
	MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE11, 0b11 << GPIO_CRH_MODE11_Pos); //Настройка GPIOA порта 11 на выход со максимальной скоростью в 50 MHz
	MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF11, 0b10 << GPIO_CRH_CNF11_Pos); //Настройка GPIOA порта 11 на выход в режиме Push-Pull
	MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE0, 0b11 << GPIO_CRL_MODE0_Pos); //Настройка GPIOA порта 0 на выход со максимальной скоростью в 50 MHz
	MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF0, 0b10 << GPIO_CRL_CNF0_Pos); //Настройка GPIOA порта 0 на выход в режиме Push-Pull
}

void Tim1_init (void) {
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM1EN); //Запуск тактирования TIM1
	TIM1->PSC = 20;    //делитель
	TIM1->ARR = 1000;  //значение перезагрузки
	TIM1->BDTR |= TIM_BDTR_MOE;          //разрешим использовать выводы таймера как выходы

	SET_BIT(TIM1->CCMR1, 0b110 << TIM_CCMR1_OC1M_Pos);   //PWM mode 1
	SET_BIT(TIM1->CCMR1, 0b110 << TIM_CCMR1_OC2M_Pos);   //PWM mode 1
	SET_BIT(TIM1->CCMR2, 0b110 << TIM_CCMR2_OC3M_Pos);   //PWM mode 1
	SET_BIT(TIM1->CCMR2, 0b110 << TIM_CCMR2_OC4M_Pos);   //PWM mode 1

	SET_BIT(TIM1->CCER, TIM_CCER_CC1E);  //настроим на выход канал 1
	SET_BIT(TIM1->CCER, TIM_CCER_CC2E);  //настроим на выход канал 2
	SET_BIT(TIM1->CCER, TIM_CCER_CC3E);  //настроим на выход канал 3
	SET_BIT(TIM1->CCER, TIM_CCER_CC4E);  //настроим на выход канал 4

	SET_BIT(TIM1->CR1, TIM_CR1_CEN);     //включаем счётчик

	TIM1->CCR1 = 0;  //коэф. заполнения 1
	TIM1->CCR2 = 0;  //коэф. заполнения 2
	TIM1->CCR3 = 0;  //коэф. заполнения 3
	TIM1->CCR4 = 0;  //коэф. заполнения 4
}

void Tim3_init (void) {
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN);     //Запуск тактирования TIM3
	TIM3->PSC = 10;                               //делитель
	TIM3->ARR = 1000;                              //значение перезагрузки
	SET_BIT(TIM3->DIER, TIM_DIER_UIE);             // прерывание по переполнению
	SET_BIT(TIM3->CR1, TIM_CR1_CEN);               //включаем счётчик

	NVIC_EnableIRQ(TIM3_IRQn);                     //Включим прерывания по TIM3
	NVIC_SetPriority(TIM3_IRQn, 1);                // выставим приоритет
}

void TIM3_IRQHandler(void) {
	CLEAR_BIT(TIM3->SR, TIM_SR_UIF);               // сбросим флаг прерывания
    if (over_locking_time)      over_locking_time --;    // для разгона/торможения
    else {over_locking_time = MAX_SPEED; flag.change_PWM = true;}
}

void Tim2_init (void) {
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);     //Запуск тактирования TIM2
	TIM2->PSC = 10;                                 //делитель
	TIM2->ARR = 100;                               //значение перезагрузки
	TIM2->BDTR |= TIM_BDTR_MOE;                    //разрешим использовать выводы таймера как выходы
	SET_BIT(TIM2->CCMR1, 0b110 << TIM_CCMR1_OC1M_Pos);   //PWM mode 1
	SET_BIT(TIM2->CCER, TIM_CCER_CC1E);                  //настроим на выход канал 1
	SET_BIT(TIM2->CR1, TIM_CR1_CEN);                     //включаем счётчик


	TIM2->CCR1 = 0;  //коэф. заполнения 1

}


/**
 ***************************************************************************************
 *  @breif Настройка MCO c выходом на ножку PA8
 *  Reference Manual/см. п.9.2 GPIO registers (стр. 171)
 *  Перед настройкой (GPIOs and AFIOs) нужно включить тактирование порта.
 ***************************************************************************************
 */
void CMSIS_PA8_MCO_init(void) {
	//Тактирование MCO должно быть настроено в регистре RCC
	MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO, RCC_CFGR_MCO_PLLCLK_DIV2); //В качестве тактирования для MCO выбрал PLL. Будет 36 MHz.
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN); //Запуск тактирования порта A
	MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE8, 0b11 << GPIO_CRH_MODE8_Pos); //Настройка GPIOA порта 8 на выход со максимальной скоростью в 50 MHz
	MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF8, 0b10 << GPIO_CRH_CNF8_Pos); //Настройка GPIOA порта 8, как альтернативная функция, в режиме Push-Pull
}
