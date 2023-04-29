
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f1xx.h"

#define    DEBUG_MODE

#define    MAX_VALUE     500         // значения для настройки чувствительности
#define    RMAX_VALUE   -500         // тоже самое но с минусом
#define    REVERS_COUNT  40          // чем выше тем ниже чуствительность

#define    MAX_TIME      3000        // максимальное время игры 3000
#define    MIN_TIME      1000         // минимальное время игры  600
#define    MAX_REPLEY    12          // допустимое количество реплеев (не ставить кратно 10!)

#define    MAX_PWM       850         // максимальный ШИМ движков
#define    MIN_PWM       700         // минимальный ШИМ движков
#define    MAX_SPEED     4           // Скорость нарастания ШИМ
#define    PAUSE_ON_GAME 2000        // пауза между играми
#define    PROVOKE       35000       // время через которое автоматом произойдет движение для провокации кота
#define    MAX_ROUND     450         // если уперлись в стену и вращаемся
#define    MIN_ACC       1.1         // минимальное ускорение
#define    MIN_ACC_M     -1.1        // минимальное ускорение

#define    POLLING_TIME  50          // время опроса

#define    true          1
#define    false         0
#define I2C_BUS_FREQ_KHZ 400
#define SYS_TIM          7200000
//---------- объявления для быстрого доступа ----------//

#define LED_ON                  SET_BIT(GPIOC -> BSRR, GPIO_BSRR_BS13)
#define LED_OFF                 SET_BIT(GPIOC -> BSRR, GPIO_BSRR_BR13)

#define LED_L_ON                SET_BIT(GPIOB -> BSRR, GPIO_BSRR_BS1)
#define LED_L_OFF               SET_BIT(GPIOB -> BSRR, GPIO_BSRR_BR1)
#define LED_R_ON                SET_BIT(GPIOB -> BSRR, GPIO_BSRR_BS0)
#define LED_R_OFF               SET_BIT(GPIOB -> BSRR, GPIO_BSRR_BR0)

#define ZERO_PROGRAM            READ_BIT(GPIOB ->IDR, GPIO_IDR_IDR12)

//A8, A9, A10, A11 motor pin



//---------- объявления для быстрого доступа ----------//
void CMSIS_RCC_SystemClock_72MHz(void);
void CMSIS_SysTick_Timer_init(void);
void Delay_ms(uint32_t Milliseconds);
void CMSIS_PC13_OUTPUT_Push_Pull_init(void);
void CMSIS_B12_Input_init(void);
void PWM_Out (void);
void Tim1_init (void);
void Tim2_init (void) ;
void Tim3_init (void) ;
void CMSIS_I2C2_Init(void);
void init_MPU6050 (void);
uint8_t CMSIS_I2C_MemRead(I2C_TypeDef *I2C, uint8_t Adress_Device, uint16_t Adress_data, uint8_t Size_adress, uint8_t* data, uint16_t Size_data, uint32_t Timeout_ms);
uint8_t CMSIS_I2C_MemWrite(I2C_TypeDef *I2C, uint8_t Adress_Device, uint16_t Adress_data, uint8_t Size_adress, uint8_t* data, uint16_t Size_data, uint32_t Timeout_ms);
void read_data_MPU6050 (void);
void parsing_MPU6050 (void);
void game (uint32_t game);
void stop_game (void);
void check_new_game (void);
void init_flag (void);
uint32_t random_time (uint32_t in_time);
void restart_value (void) ;
void overclocking_PWM (void) ;
void braking_PWM (void) ;
void start (void);
void treck_go (uint8_t num_track);
void JoyToTheWorld() ;
uint32_t constrain (uint32_t Value, uint32_t Min, uint32_t Max);
void set_PWM (uint16_t ch_A, uint16_t ch_B, uint16_t ch_C, uint16_t ch_D);
void read_from_flash (void);
void write_flash (void);
void write_to_struct (uint32_t X, uint32_t Y, uint32_t Z);
void check_let (void);

struct flags {
	uint32_t start_game: 1;          // запуск игры
	uint32_t time_of: 1;             // флаг когда нужно остановится
	uint32_t start_polling: 6;       // время опроса
	uint32_t repleys_game: 6;        // количество повторений игр
	uint32_t repleys_buzer: 4;       // количество повторений бузера
	uint32_t overlocking:1;          // разгон разрешен
	uint32_t braking:1;              // торможение разрешено
	uint32_t change_PWM:1;           // разрешить изменять шим
	uint32_t sound:4;                // вкл музыку
	uint32_t provoke: 1;             // разрешить самопроизвольно включатся
	uint32_t mov_provoke: 1;         // авто запуск
	uint32_t check_let: 1;           // разрешить детектирование препятствия
};

struct _flash {
	int32_t gyrX_zero ;
	int32_t gyrY_zero ;
	int32_t gyrZ_zero ;

};

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
