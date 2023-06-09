
#include "main.h"

#define tonePin 100

extern volatile uint16_t sound_delay;
extern struct flags flag;

uint16_t sound_1 [] = {
		584, 450,
		0, 500,
};


void new_sound_PWM (uint16_t *buf, uint8_t len) {
	static uint8_t number = 0;
	TIM2->ARR = SYS_TIM / buf [number];
	TIM2->CCR1 = TIM2->ARR / 2;
	number ++;
	sound_delay = buf [number];
	if (number == len) {
		flag.sound = 0;
	}
}

void treck_go (uint8_t num_track) {
	switch (num_track) {
	case 1:
		new_sound_PWM (sound_1, 50);
		break;
	case 2:

		break;

	}
}

void tone (uint8_t no, uint16_t freq, uint16_t len){ //
	TIM2->ARR = SYS_TIM / (uint16_t) freq;
	TIM2->CCR1 = TIM2->ARR / 2;
	Delay_ms (len);
}




/*void JoyToTheWorld() {
	tone(tonePin, 220, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 220, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 220, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 293, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 220, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 329, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 293, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 329, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 391, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 329, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 195, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 195, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 246, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 195, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 195, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 293, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 174, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 174, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 220, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 174, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 174, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 246, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 220, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 220, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 220, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 293, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 220, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 329, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 293, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 329, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 391, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 329, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 195, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 195, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 246, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 195, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 195, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 293, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 174, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 174, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 220, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 174, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 174, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 246, 109.489);
	    delay(121.654444444);
	    delay(21958.6272222);
	    tone(tonePin, 622, 263.45790625);
	    delay(292.731006944);
	    delay(72.2323263889);
	    tone(tonePin, 622, 263.45790625);
	    delay(292.731006944);
	    delay(72.2323263889);
	    tone(tonePin, 880, 263.45790625);
	    delay(292.731006944);
	    delay(72.2323263889);
	    tone(tonePin, 1244, 263.45790625);
	    delay(292.731006944);
	    delay(11751.0589931);
	    tone(tonePin, 220, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 220, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 220, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 293, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 220, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 329, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 293, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 329, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 391, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 329, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 195, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 195, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 246, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 195, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 195, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 293, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 174, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 174, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 220, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 174, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 174, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 246, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 220, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 220, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 220, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 293, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 220, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 329, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 293, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 329, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 391, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 329, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 195, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 195, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 246, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 195, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 195, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 293, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 174, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 174, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 220, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 174, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 174, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 261, 109.489);
	    delay(121.654444444);
	    delay(60.8272222222);
	    tone(tonePin, 246, 109.489);
	    delay(121.654444444);
	    delay(11922.1355556);
	    tone(tonePin, 880, 205.291875);
	    delay(228.102083333);
	    delay(45.6204166667);
	    tone(tonePin, 783, 205.291875);
	    delay(228.102083333);
	    delay(45.6204166667);
	    tone(tonePin, 698, 205.291875);
	    delay(228.102083333);
	    delay(45.6204166667);
	    tone(tonePin, 659, 205.291875);
	    delay(228.102083333);
	    delay(45.6204166667);
	    tone(tonePin, 587, 171.0765625);
	    delay(190.085069444);
	    delay(174.878263889);
	    tone(tonePin, 880, 205.291875);
	    delay(228.102083333);
	    delay(45.6204166667);
	    tone(tonePin, 783, 205.291875);
	    delay(228.102083333);
	    delay(45.6204166667);
	    tone(tonePin, 698, 205.291875);
	    delay(228.102083333);
	    delay(45.6204166667);
	    tone(tonePin, 659, 205.291875);
	    delay(228.102083333);
	    delay(45.6204166667);
	    tone(tonePin, 587, 171.0765625);
	    delay(190.085069444);
	    delay(174.878263889);
	    tone(tonePin, 880, 205.291875);
	    delay(228.102083333);
	    delay(45.6204166667);
	    tone(tonePin, 783, 205.291875);
	    delay(228.102083333);
	    delay(45.6204166667);
	    tone(tonePin, 698, 205.291875);
	    delay(228.102083333);
	    delay(45.6204166667);
	    tone(tonePin, 659, 205.291875);
	    delay(228.102083333);
}*/


