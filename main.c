/*
 * main.c
 * Author: Muhammet Kamil KALAYCI
 * Project 2 - Fully Operational Scientific Calculator
 *
 */

#include "bsp.h"

int main(void) {
	//NVIC_SetPriority(TIM2_IRQn, 2);
   // NVIC_EnableIRQ(TIM2_IRQn);

 	init_all(); // initialize the system
 	//init_timer_2();
	PWM_init();
 	init_timer1();
    while(1){
    	compare_display(); // call the display function which will be interrupted by external keypad presses
    }

    return 0;
}
