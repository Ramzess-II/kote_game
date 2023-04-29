#include "main.h"
#include "stdio.h"

//---------- переменные из других файлов ----------//
extern float gyrX_f;
extern float gyrY_f;
extern float gyrZ_f;
extern float accX_f;
extern float accY_f;
extern float accZ_f;
extern volatile uint32_t SysTimer_ms;
extern volatile uint32_t time_game;
extern uint16_t time_to_next_game;
extern uint16_t provoke;
extern struct _flash flash;
//---------- переменные из этого файла ------------//

float sleep_gyrX;              // для накопления данных с гиро
float sleep_gyrY;
float sleep_gyrZ;

struct flags flag;             // флаги

uint16_t  chanel_A = 0;        // установка шима
uint16_t  chanel_B = 0;
uint16_t  chanel_C = 0;
uint16_t  chanel_D = 0;

int main(void) {

	CMSIS_RCC_SystemClock_72MHz();           // инит тактирование
	CMSIS_SysTick_Timer_init();              // инит сис тик
	CMSIS_PC13_OUTPUT_Push_Pull_init();      // инит выходов
	CMSIS_B12_Input_init();                  // пин на вход
	Delay_ms(10);
	CMSIS_I2C2_Init();                       // инит И2С
	init_MPU6050();                          // настройка гиро
	PWM_Out();                               // настройка ШИМ выводов
	Tim1_init();                             // инит таймеров
	Tim2_init();
	Tim3_init();
	init_flag ();                            // сброс флагов в ноль
	start ();                                // моргаем лампами
    if (!ZERO_PROGRAM) {                     // калибровка нуля
		read_data_MPU6050();                 // читаем данные с модуля
		parsing_MPU6050();                   // расбрасываем их в флоаты
		write_to_struct ((int32_t)gyrX_f, (int32_t)gyrY_f, (int32_t)gyrZ_f);
		write_flash ();
    }
    Delay_ms(10);
    read_from_flash ();                      // читаем из памяти
	//JoyToTheWorld ();

	for (;;) {
		if (!flag.start_polling) {           // задержка опроса, заходим только когда время выйдет
			read_data_MPU6050();             // читаем данные с модуля
			parsing_MPU6050();               // расбрасываем их в флоаты
			flag.start_polling = POLLING_TIME;  // устанавливаем новое время для опроса
			check_new_game();                // проверяем не начать ли нам новую игру
			check_let ();
		}
		if (flag.time_of) {                  // это штука чтоб остановить игру один раз
			flag.time_of = false;            // вот этим флагом
			stop_game();                     // останавливаем игру
		}
		if (flag.change_PWM) {               // это плавное нарастание шим, заходим сюда через какое то время, но если мы уже разогнались то ничего неделает
			overclocking_PWM ();             // разгон
			braking_PWM ();                  // торможение
			flag.change_PWM = false;         // сбросим флаг, до дальнейшего истечения времени
		}
		if (flag.provoke) {                  // если поднят флаг провокаций
			if (flag.mov_provoke) {          // ждем флаг что можно двигать
				flag.mov_provoke = false;
				flag.start_game = true;      // поднимаем флаг игры
				game(SysTimer_ms);           // запускаем игру
				flag.repleys_game = 3;       // с тремя повторениями
			}

		}
	}
}

void overclocking_PWM (void) {               // разгон
	if (flag.overlocking) {                  // если включен разгон
		uint8_t stop_overlocking = 0;        // временная переменная
		if (TIM1->CCR1 < chanel_A) TIM1->CCR1 += 1; else stop_overlocking ++;   // если еще не разогнались разгоняемся
		if (TIM1->CCR2 < chanel_B) TIM1->CCR2 += 1; else stop_overlocking ++;   // если же разогнались плюсуем переменную чтоб остановить разгон
		if (TIM1->CCR3 < chanel_C) TIM1->CCR3 += 1; else stop_overlocking ++;
		if (TIM1->CCR4 < chanel_D) TIM1->CCR4 += 1; else stop_overlocking ++;
		if (stop_overlocking == 4) {flag.overlocking = false; }                 // если все 4 уже разогнались тормозим
	}
}

void braking_PWM (void) {                    // торможение. аналогично разгону
	if (flag.braking) {
		uint8_t stop_braking = 0;
		if (TIM1->CCR1 > chanel_A) TIM1->CCR1 -= 1; else stop_braking ++;
		if (TIM1->CCR2 > chanel_B) TIM1->CCR2 -= 1; else stop_braking ++;
		if (TIM1->CCR3 > chanel_C) TIM1->CCR3 -= 1; else stop_braking ++;
		if (TIM1->CCR4 > chanel_D) TIM1->CCR4 -= 1; else stop_braking ++;
		if (stop_braking == 4) { flag.braking = false; flag.time_of = true; LED_L_ON; LED_R_ON;}  // только еще включаем лампы
	}
}

uint32_t random_time (uint32_t in_time){       // рандомное время
	uint32_t mirror;                           // зеркало
	mirror = in_time % MAX_TIME;               // присваиваем число которое всегда будет в диапазоне от нуля до MAX_TIME
	if (mirror < MIN_TIME) mirror = MIN_TIME;  // но так как ноль нам не подходит то делаем минимум
	LED_ON;
	return mirror;
}

void stop_game(void) {                         // остановить игру
	if (TIM1->CCR1 > 0 || TIM1->CCR2 > 0 || TIM1->CCR3 > 0 || TIM1->CCR4 > 0) {     // проверим шим у нас в нулях или не
		chanel_A = 0;    // если нет, ставим коэф. заполнения 0
		chanel_B = 0;
		chanel_C = 0;
		chanel_D = 0;
		flag.braking = true;     // поднимаем флаг торможения
		return;                  // покидаем функцию
	}
	if (flag.repleys_game) {     // тут мы потихоньку сбрасываем количество повторений
		flag.repleys_game -- ;
		game(SysTimer_ms);       // запуская новую игру
		return;
	}
	flag.start_game = false;     // если уже нету повторений говорим что игра выкл
	restart_value ();            // сбрасываем показания накопленных значений гиро
	LED_OFF;                     // лампу на плате
	time_to_next_game = PAUSE_ON_GAME;  // и вводим ограничитель между играми чтоб можно было остановится
	flag.provoke = true;         // и запускаем возможность провокаций
	provoke = PROVOKE;           // выставляем время между ними
}

void restart_value (void) {      // сброс данных
	sleep_gyrX = 0;
	sleep_gyrY = 0;
	sleep_gyrZ = 0;
}

void check_new_game(void) {      // проверка не нужно ли начать новую игру
	if (!flag.start_game && !flag.repleys_game && !time_to_next_game) {    // если у нас не игра, если количество реплеев 0, и если выдержали паузу между играми
		sleep_gyrX += gyrX_f;             // потихоньку увеличиваем переменную хранения гиро
		sleep_gyrY += gyrY_f;
		sleep_gyrZ += gyrZ_f;
		if (sleep_gyrX >= MAX_VALUE || sleep_gyrY >= MAX_VALUE || sleep_gyrZ >= MAX_VALUE) {  // если по какой то из осей есть движение
			flag.start_game = true;                         // поднимаем флаг
			game(SysTimer_ms);                              // запускаем игру
			flag.repleys_game = SysTimer_ms % MAX_REPLEY;   // и выбираем количество повторов
			restart_value ();                               // и сбросим переменные, чтоб их использовать в движении
		}
		if (sleep_gyrX <= RMAX_VALUE || sleep_gyrY <= RMAX_VALUE || sleep_gyrZ <= RMAX_VALUE) {  // тоже самое если показания в минус
			flag.start_game = true;
			game(SysTimer_ms);
			flag.repleys_game = SysTimer_ms % MAX_REPLEY;
			restart_value ();
		}
		if (sleep_gyrX > 0) sleep_gyrX -= REVERS_COUNT; else sleep_gyrX += REVERS_COUNT;       // и паралельно сбросим показания
		if (sleep_gyrY > 0) sleep_gyrY -= REVERS_COUNT; else sleep_gyrY += REVERS_COUNT;
		if (sleep_gyrZ > 0) sleep_gyrZ -= REVERS_COUNT; else sleep_gyrZ += REVERS_COUNT;
	}
}

void check_let (void) {
	if (flag.start_game) {
		if (accX_f > MIN_ACC || accX_f < MIN_ACC_M || accY_f > MIN_ACC || accY_f < MIN_ACC_M || accZ_f > MIN_ACC || accZ_f < MIN_ACC_M) {
			return;
		}
		time_game = 5;
		/*if ((gyrX_f + gyrY_f + gyrZ_f) >  MAX_ROUND) {
			time_game = 5;
		}*/
	}
}

void start (void) {    // моргание при старте
	LED_L_ON;
	Delay_ms (1000);
	LED_R_ON;
	Delay_ms (1000);
	LED_L_OFF;
	Delay_ms (1000);
	LED_R_OFF;
	Delay_ms (1000);
	LED_L_ON; LED_R_ON;
}

void game(uint32_t game) {              // игра
	flag.provoke = false;               // запретим провокацию
	uint32_t mirror = game % MAX_PWM;   // промежуточная переменная для constrain
	switch (game % 4) {                 // +1
	case 0:
		set_PWM (constrain (mirror, MIN_PWM, MAX_PWM),0,constrain (mirror, MIN_PWM, MAX_PWM),0);  // установим шим под каждый вариант игры
		time_game = random_time(game);  // время игры
		LED_L_OFF;                      // светик рандомный
		break;
	case 1:
		set_PWM (0,constrain (mirror, MIN_PWM, MAX_PWM),0,constrain (mirror, MIN_PWM, MAX_PWM));
		time_game = random_time(game);
		LED_R_OFF;
		break;
	case 2:
		set_PWM (0,constrain (mirror, MIN_PWM, MAX_PWM),0,0);
		time_game = random_time(game);
		LED_R_OFF;
		break;
	case 3:
		set_PWM (0,0,constrain (mirror, MIN_PWM, MAX_PWM),0);
		time_game = random_time(game);
		LED_L_OFF;
		break;
	}
}

void set_PWM (uint16_t ch_A, uint16_t ch_B, uint16_t ch_C, uint16_t ch_D) {
	chanel_A = ch_A;    // перенесем коэф
	chanel_B = ch_B;
	chanel_C = ch_C;
	chanel_D = ch_D;
	flag.overlocking = true;
}

void init_flag (void) {          // инициализируем флаги при старте
	flag.start_game = false;
	flag.time_of = 0;
	flag.start_polling = 0;
	flag.repleys_game = 0;
	flag.repleys_buzer = 0;
	flag.overlocking = false;
	flag.braking = false;
	flag.change_PWM = false;
	flag.start_polling = 0;
	flag.provoke = false;
	flag.mov_provoke = false;
}

uint32_t constrain (uint32_t Value, uint32_t Min, uint32_t Max){
  return (Value < Min)? Min : (Value > Max)? Max : Value;
}
















int _write(int file, char *ptr, int len) // это переопределение функции для вывода в СВО
{
	(void) file;
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++) {
		ITM_SendChar(*ptr++);
	}
	return len;
}
