/*
 * bsp.c
 * Author: Muhammet Kamil KALAYCI 1801022085
 * Custom Board Support Package - bsp.h implementation
 *
 */

#include "bsp.h"
#include <math.h>


volatile int A_pressed,B_pressed,C_pressed,D_pressed,enter,m,a,b;
volatile int mode = 1;

volatile int amp[4],freq[4]; // will hold the numbers that pressed from keypad
volatile int i = 0; // index for the array
volatile int j = 0; // index for the array
volatile double entered_numbers[2] = {0.0}; // will hold the numbers to be processed
volatile double res; // result of an operation
volatile int operator, equal, minus, sci, check_dot, presses = 0;

volatile int operator2, dot_count, ten_sec_counter, dot; //operator2 for keeping the value of operator safe,
// dot_count for checking the place of dot with floating numbers, ten_sec_counter for going back to idle state after 10sec if no button pressed


volatile int d1 = -1; // d1, d2, d3 and d4 is the digits will be displayed on ssd
volatile int d2 = -1;
volatile int d3 = -1;
volatile int d4 = -1;
volatile int duty = 1;
//end of globals

//starting of my Board Support Package
void init_clocks() { // activates all clocks
  RCC -> IOPENR |= 7U;
}

void set_onboardLED_output() { // sets the on-board led to output mode
  GPIOC -> MODER &= ~(3U << 2 * 6);
  GPIOC -> MODER |= (1U<< 2 * 6);
}

void turn_on_onboardLED() { // turns on on-board led
  GPIOC -> ODR |= (1U<< 6);
}

void turn_off_onboardLED() {// turns off on-board led
  GPIOC -> ODR &= ~(1U<< 6);
}

void toggle_onboardLED() {// toggles on-board led
  GPIOC -> ODR ^= (1U<< 6);
}

void set_output(int port, int pin) { // sets the given pin to output mode, 1 for PortA, 2 for PortB
  switch (port) {
  case 1:
    GPIOA -> MODER &= ~(3U << 2 * pin);
    GPIOA -> MODER |= (1U<< 2 * pin);
    break;
  case 2:
    GPIOB -> MODER &= ~(3U << 2 * pin);
    GPIOB -> MODER |= (1U<< 2 * pin);
    break;
  }
}

void set_input(int port, int pin) { // sets the given pin to input mode, 1 for PortA, 2 for PortB
  switch (port) {

  case 1:
    GPIOA -> MODER &= ~(3U << 2 * pin);
    break;
  case 2:
    GPIOB -> MODER &= ~(3U << 2 * pin);
    break;
  }
}

void set_pin_to1(int port, int pin) { //set the given output port to logic 1
  switch (port) {

  case 1:
    GPIOA -> ODR |= (1U<< pin);
    break;
  case 2:
    GPIOB -> ODR |= (1U<< pin);
    break;
  }
}

void set_pin_to0(int port, int pin) { //set the given output port to logic 0
  switch (port) {

  case 1:
    GPIOA -> ODR &= ~(1U<< pin);
    break;
  case 2:
    GPIOB -> ODR &= ~(1U<< pin);
    break;
  }
}

void toggle_pin(int port, int pin) {// toggles the value of given output port
  switch (port) {

  case 1:
    GPIOA -> ODR ^= (1U<< pin);
    break;
  case 2:
    GPIOB -> ODR ^= (1U<< pin);
    break;
  }
}

void delay(int time) { // waits until given time
  for (; time > 0; time--);
}

void my_delay() { // short delay for displaying all digits at the same time
  for (int i = 0; i < 600; i++);
}

void clear_rows() { // sets all rows to 0 - clears
  GPIOA -> ODR &= ~(1U<< 10);
  GPIOA -> ODR &= ~(1U<< 9);
  GPIOB -> ODR &= ~(1U<< 0);
  GPIOB -> ODR &= ~(1U<< 3);
}

void set_rows() { // sets all rows to 1 - activates
  GPIOA -> ODR |= (1U<< 10);
  GPIOA -> ODR |= (1U<< 9);
  GPIOB -> ODR |= (1U<< 0);
  GPIOB -> ODR |= (1U<< 3);
}

void EXTI0_1_IRQHandler() { //interrupt from PA0(Column 1) and PB1(Column 2)
  //PA10 PA9 PB0 PB8 outputs - rows
  delay(300); // for preventing bouncing
  ten_sec_counter = 0; // reset 10sec counter if an interrupt happens

  //check rows for column 1
  // first deactivate all digits and activate them one by one while checking
  //status register so we can determine which key is pressed
  if ((EXTI -> RPR1 >> 0) & 1) {
    clear_rows();

    // Row 1 - number 1
    toggle_pin(1,10); // activate first row
    if ((GPIOA -> IDR >> 0) & 1) { // check the status register for PA0 is 1, if it is then number 1 key is pressed
    	if(A_pressed==1){
    		amp[i]=1;
    		i++;
    	}
    	if(B_pressed==1){
    		freq[j]=1;
    		j++;
    	}
    	if(C_pressed==1){
    		mode=1;
    	}
    }
    toggle_pin(1,10); // deactivate row 2

    // Row 2 - number 4
    toggle_pin(1, 9);
    if ((GPIOA -> IDR >> 0) & 1) {
    	if(A_pressed==1){
    	    		amp[i]=4;
    	    		i++;
    	    	}
    	    	if(B_pressed==1){
    	    		freq[j]=4;
    	    		j++;
    	    	}
    	    	if(C_pressed==1){
    	    		mode=4;
    	    	}

    }
    toggle_pin(1, 9);

    // Row 3 - number 7
    toggle_pin(2, 0);
    if ((GPIOA -> IDR >> 0) & 1) {
    	if(A_pressed==1){
    	    		amp[i]=7;
    	    		i++;
    	    	}
    	    	if(B_pressed==1){
    	    		freq[j]=7;
    	    		j++;
    	    	}
    	    	if(C_pressed==1){
    	    		mode=0;
    	    	}

    }
    toggle_pin(2, 0);

    // Row 4 - * or dot key
    toggle_pin(2, 3);
    if ((GPIOA -> IDR >> 0) & 1) {// presses or i will not be increased here since they are for number presses

    	dot=1;

    }
    toggle_pin(2, 3);
  }

  set_rows();

  //check rows for Column 2
  if ((EXTI -> RPR1 >> 1) & 1) { //check status register for PB1
    clear_rows();
    // Row 1 - number 2
    GPIOA -> ODR ^= (1U<< 10);
    if ((GPIOB -> IDR >> 1) & 1) {
    	if(A_pressed==1){
    	    		amp[i]=2;
    	    		i++;
    	    	}
    	    	if(B_pressed==1){
    	    		freq[j]=2;
    	    		j++;
    	    	}
    	    	if(C_pressed==1){
    	    		mode=2;
    	    	}
    }
    GPIOA -> ODR ^= (1U<< 10);

    // Row 2 - number 5
    GPIOA -> ODR ^= (1U<< 9);
    if ((GPIOB -> IDR >> 1) & 1) {
    	if(A_pressed==1){
    	    		amp[i]=5;
    	    		i++;
    	    	}
    	    	if(B_pressed==1){
    	    		freq[j]=5;
    	    		j++;
    	    	}
    	    	if(C_pressed==1){
    	    		mode=5;
    	    	}
    }
    GPIOA -> ODR ^= (1U<< 9);

    // Row 3 - number 8
    toggle_pin(2, 0);
    if ((GPIOB -> IDR >> 1) & 1) {
    	if(A_pressed==1){
    	    		amp[i]=8;
    	    		i++;
    	    	}
    	    	if(B_pressed==1){
    	    		freq[j]=8;
    	    		j++;
    	    	}
    	    	if(C_pressed==1){
    	    		mode=0;
    	    	}
    }
    toggle_pin(2, 0);

    // Row 4 - number 0
    toggle_pin(1, 15);
    if ((GPIOB -> IDR >> 1) & 1) {
    	if(A_pressed==1){
    	    		amp[i]=0;
    	    		i++;
    	    	}
    	    	if(B_pressed==1){
    	    		freq[j]=0;
    	    		j++;
    	    	}
    	    	if(C_pressed==1){
    	    		mode=0;
    	    	}
    }
    toggle_pin(1, 15);

  }

  set_rows();



  if(A_pressed == 1 && B_pressed != 1){
  if(i==4){
  		   d1=amp[3];
  		   d2=amp[2];
  		   d3=amp[1];
  		   d4=amp[0];
  	   }
  	   if(i==3){
  		   d1=-1;
  		   d2=amp[2];
  		   d3=amp[1];
  		   d4=amp[0];
  	   	   }
  	   if(i==2){
  		   d1=-1;
  		   		   d2=-1;
  		   		   d3=amp[1];
  		   		   d4=amp[0];
  		  }
  	   if(i==1){
  		   d1=-1;
  		   d2=-1;
  		   d3=-1;
  		   d4=amp[0];
  	   		  }
  }

  if(A_pressed != 1 && B_pressed == 1){
  if(j==4){
  		   d1=freq[3];
  		   d2=freq[2];
  		   d3=freq[1];
  		   d4=freq[0];
  	   }
  	   if(j==3){
  		   d1=-1;
  		   d2=freq[2];
  		   d3=freq[1];
  		   d4=freq[0];
  	   	   }
  	   if(j==2){
  		   d1=-1;
  		   		   d2=-1;
  		   		   d3=freq[1];
  		   		   d4=freq[0];
  		  }
  	   if(j==1){
  		   d1=-1;
  		   d2=-1;
  		   d3=-1;
  		   d4=freq[0];
  	   		  }
  }


  //reset status registers
  EXTI -> RPR1 |= (1U<< 0);
  EXTI -> RPR1 |= (1U<< 1);

  //display the current number on the SSD
if (A_pressed == 1 || B_pressed == 1){
	while(!( (EXTI ->  RPR1 >> 1) & 1 || (EXTI -> RPR1 >> 0) & 1 || ( (EXTI ->  RPR1 >> 7) & 1)))
	find_digit(d4,d3,d2,d1);
}


}

void EXTI4_15_IRQHandler() { //interrupt from PB7(Column 3) and PB8(Column 4)
  //PA10 PA9 PB0 PB8 outputs - rows
  delay(300);// for preventing bouncing
  ten_sec_counter = 0; // reset 10sec counter if an interrupt happens

  //check rows for column 3
  if ((EXTI -> RPR1 >> 7) & 1) {
    clear_rows();
    // Row 1 - number 3
    toggle_pin(1,10);
    if ((GPIOB -> IDR >> 7) & 1) {
    	if(A_pressed==1){
    	    		amp[i]=3;
    	    		i++;
    	    	}
    	    	if(B_pressed==1){
    	    		freq[j]=3;
    	    		j++;
    	    	}
    	    	if(C_pressed==1){
    	    		mode=3;
    	    	}
    }
    toggle_pin(1,10);
    // Row 2 - number 6
    toggle_pin(1,9);
    if ((GPIOB -> IDR >> 7) & 1) {
    	if(A_pressed==1){
    	    		amp[i]=6;
    	    		i++;
    	    	}
    	    	if(B_pressed==1){
    	    		freq[j]=6;
    	    		j++;
    	    	}
    	    	if(C_pressed==1){
    	    		mode=6;
    	    	}
    }
    toggle_pin(1,9);
    // Row 3 - number 9
    toggle_pin(2, 0);
    if ((GPIOB -> IDR >> 7) & 1) {
    	if(A_pressed==1){
    	    		amp[i]=9;
    	    		i++;
    	    	}
    	    	if(B_pressed==1){
    	    		freq[j]=9;
    	    		j++;
    	    	}
    	    	if(C_pressed==1){
    	    		mode=0;
    	    	}
    }
    toggle_pin(2, 0);
    // Row 4 - # or E key
    toggle_pin(1,15);
    if ((GPIOB -> IDR >> 7) & 1) {// presses or i will not be increased here since they are for number presses
       // pressing # will increase counter sci since we have 3 options for it, if it is pressed repeatedly
    	enter = 1; //set equal to 1 for displaying result

    	 A_pressed=0;
    	 B_pressed=0;
    	 C_pressed=0;
    	 D_pressed=0;

      GPIOC -> ODR ^= (1U<< 6);
      TIM1 -> ARR = (freq[3]*1000+freq[2]*100+freq[1]*10+freq[0])*999;
      toggle_pin(1, 15);
    }

  }
  set_rows();
  //check rows for column 4
  if ((EXTI -> RPR1 >> 8) & 1) {
    clear_rows();
    // presses or i will not be increaosed here since they are for number presses
    // A key
    GPIOA -> ODR ^= (1U<< 10);
    if ((GPIOB -> IDR >> 8) & 1) {
      A_pressed=1;
    }
    GPIOA -> ODR ^= (1U<< 10);
    // B key
    GPIOA -> ODR ^= (1U<< 9);
    if ((GPIOB -> IDR >> 8) & 1) {
      B_pressed=1;
      A_pressed = 0;
      d1 = -1;
      d2 = -1;
      d3 = -1;
      d4 = -1;
    }
    GPIOA -> ODR ^= (1U<< 9);
    // C key
    toggle_pin(2, 0);
    if ((GPIOB -> IDR >> 8) & 1) {
      C_pressed=1;
      A_pressed = 0;
      B_pressed = 0;
      d1 = -1;
      d2 = -1;
      d3 = -1;
      d4 = -1;
    }
    toggle_pin(2, 0);
    // D key
    toggle_pin(1, 15);
    if ((GPIOB -> IDR >> 8) & 1) {
        C_pressed=0;
        A_pressed = 0;
        B_pressed = 0;
      D_pressed=1;
    }
    toggle_pin(1,15);
  }

  set_rows();



   if (D_pressed==1){

	   if(i==4){
		   d1=amp[3];
		   d2=amp[2];
		   d3=amp[1];
		   d4=amp[0];
	   }
	   if(i==3){
		   d1=amp[3];
		   d2=amp[2];
		   d3=amp[1];
		   d4=amp[0];
	   	   }
	   if(i==2){
		   d1=amp[3];
		   		   d2=amp[2];
		   		   d3=amp[1];
		   		   d4=amp[0];
		  }
	   if(i==1){
		   d1=amp[3];
		   d2=amp[2];
		   d3=amp[1];
		   d4=amp[0];
	   		  }
	   for(a=0;a<3200;a++){
	   find_digit(d4, d3, d2, d1);}

	   if(i==4){
	   		   d1=freq[3];
	   		   d2=freq[2];
	   		   d3=freq[1];
	   		   d4=freq[0];
	   	   }
	   	   if(i==3){
	   		   d1=freq[3];
	   		   d2=freq[2];
	   		   d3=freq[1];
	   		   d4=freq[0];
	   	   	   }
	   	   if(i==2){
	   		   d1=freq[3];
	   		   d2=freq[2];
	   		   d3=freq[1];
	   		   d4=freq[0];
	   		  }
	   	   if(i==1){
	   		   d1=freq[3];
	   		   d2=freq[2];
	   		   d3=freq[1];
	   		   d4=freq[0];


	   	   		  }

		   for(a=0;a<3200;a++){		//5 seconds timer
	   	   find_digit(d4, d3, d2, d1);}

		   for(a=0;a<3200;a++){
	   	   display_mode(mode);		//C values(modes)

		   i = 0; j = 0; A_pressed = 0; B_pressed = 0; C_pressed = 0; D_pressed = 0;
		   d1 = -1;
		   d2 = -1;
		   d3 = -1;
		   d4 = -1;
	for(b=0;b<4;b++)
		         {amp[b]=0; freq[b]=0;}
		   }




   }

if(A_pressed == 1 && B_pressed != 1){
   if(i==4){
   		   d1=amp[3];
   		   d2=amp[2];
   		   d3=amp[1];
   		   d4=amp[0];
   	   }
   	   if(i==3){
   		   d1=-1;
   		   d2=amp[2];
   		   d3=amp[1];
   		   d4=amp[0];
   	   	   }
   	   if(i==2){
   		   d1=-1;
   		   		   d2=-1;
   		   		   d3=amp[1];
   		   		   d4=amp[0];
   		  }
   	   if(i==1){
   		   d1=-1;
   		   d2=-1;
   		   d3=-1;
   		   d4=amp[0];
   	   		  }
}


if(A_pressed != 1 && B_pressed == 1){	//freq
if(j==4){
		   d1=freq[3];
		   d2=freq[2];
		   d3=freq[1];
		   d4=freq[0];
	   }
	   if(j==3){
		   d1=-1;
		   d2=freq[2];
		   d3=freq[1];
		   d4=freq[0];
	   	   }
	   if(j==2){
		   d1=-1;
		   		   d2=-1;
		   		   d3=freq[1];
		   		   d4=freq[0];
		  }
	   if(j==1){
		   d1=-1;
		   d2=-1;
		   d3=-1;
		   d4=freq[0];
	   		  }
}

if(C_pressed == 1){
	while(!( (EXTI ->  RPR1 >> 1) & 1 || (EXTI -> RPR1 >> 0) & 1 || ( (EXTI ->  RPR1 >> 7) & 1))){

		if(m != 0){
		display_mode(mode);
	}
}
	m++;}

   //reset status registers
   EXTI -> RPR1 |= (1U<< 0);
   EXTI -> RPR1 |= (1U<< 1);

   //display the current number on the SSD
 if (A_pressed == 1 || B_pressed == 1){
		while(!( (EXTI ->  RPR1 >> 1) & 1 || (EXTI -> RPR1 >> 0) & 1 || ( (EXTI ->  RPR1 >> 7) & 1)))
 	find_digit(d4,d3,d2,d1);
 }

  // Reset status registers
  EXTI -> RPR1 |= (1U<< 7);
  EXTI -> RPR1 |= (1U<< 8);
  // Display the current number on the SS
}

void init_all() { //function that initializes clocks and MODERS

  init_clocks();// Enable GPIOA/B/C clocks
  set_onboardLED_output();
  GPIOB -> MODER &= ~(3U << 2 * 2);

  uint32_t x = 0x82A00;
  GPIOB -> MODER &= ~x;
  x = 0x41500;
  GPIOB -> MODER |= x;

  x = 0xC28AAA08;
  GPIOA -> MODER &= ~x;
  x = 0x1455504;
  GPIOA -> MODER |= x;

  //set PA10 PA9 PB0 PA15 outputs - rows
  set_output(1,10);
  set_output(1,9);
  set_output(2,0);
  set_output(1,15);

  //set PA0 PB1 PB8 PB7 inputs - columns, enable pull-down
  set_input(1,0);
  GPIOA->PUPDR |= (2U << 2*0);
  set_input(2,1);
  GPIOB->PUPDR |= (2U << 2*1);
  set_input(2,7);
  GPIOB->PUPDR |= (2U << 2*7);
  set_input(2,8);
  GPIOB->PUPDR |= (2U << 2*8);

  init_ext_interrupts(); // initialize external interrupts for keypad

  //set input pins to 1 - columns
  set_pin_to1(1,0);
  set_pin_to1(2,1);
  set_pin_to1(2,7);
  set_pin_to1(2,8);

}

void init_ext_interrupts(){ // initializes external interrupts for keypad
  //PA0
  EXTI->EXTICR[0] |= (0U << 0);
  EXTI->RTSR1 |= (1U << 0);
  EXTI->IMR1 |= (1U << 0);
  NVIC_SetPriority(EXTI0_1_IRQn, 0);
  NVIC_EnableIRQ(EXTI0_1_IRQn);

  //PB1
  EXTI->EXTICR[0] |= (1U << 8*1);
  EXTI->RTSR1 |= (1U << 1);
  EXTI->IMR1 |= (1U << 1);
  NVIC_SetPriority(EXTI0_1_IRQn, 0);
  NVIC_EnableIRQ(EXTI0_1_IRQn);

	//PB7
  EXTI->EXTICR[1] |= (1U << 8*3);
  EXTI->RTSR1 |= (1U << 7);
  EXTI->IMR1 |= (1U << 7);
  NVIC_SetPriority(EXTI4_15_IRQn, 0);
  NVIC_EnableIRQ(EXTI4_15_IRQn);

	//PB8
  EXTI->EXTICR[2] |= (1U << 8*0);
  EXTI->RTSR1 |= (1U << 8);
  EXTI->IMR1 |= (1U << 8);
  NVIC_SetPriority(EXTI4_15_IRQn, 0);
  NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void activate_d1() { //function that activates first digit connected to PB4
  GPIOB -> ODR |= (1U<< 4);
}
void activate_d2() { //function that activates second digit connected to PB5
  GPIOB -> ODR |= (1U<< 5);
}
void activate_d3() { //function that activates third digit connected to PB9
  GPIOB -> ODR |= (1U<< 9);
}
void activate_d4() { //function that activates fourth digit connected to PB6
  GPIOB -> ODR |= (1U<< 6);
}
void deactivate_d1() { //function that activates first digit connected to PB4
  GPIOB -> ODR &= ~(1U<< 4);
}
void deactivate_d2() { //function that activates second digit connected to PB5
  GPIOB -> ODR &= ~(1U<< 5);
}
void deactivate_d3() { //function that activates third digit connected to PB9
  GPIOB -> ODR &= ~(1U<< 9);
}
void deactivate_d4() { //function that activates fourth digit connected to PB6
  GPIOB -> ODR &= ~(1U<< 6);
}

//display zero
void zero() { //PA 1 4 6 7 8 12
  uint32_t x = 0x11D2;
  GPIOA -> ODR &= ~(x);
}
//display one
void one() { //PA 7 12
  uint32_t x = 0x1080;
  GPIOA -> ODR &= ~(x);
}
//display two
void two() { //PA 1 4 7 8 11
  uint32_t x = 0x992;
  GPIOA -> ODR &= ~(x);
}
//display three
void three() { //PA 4 7 8 11 12
  uint32_t x = 0x1990;
  GPIOA -> ODR &= ~(x);
}
//display four
void four() { //PA 6 7 11 12
  uint32_t x = 0x18C0;
  GPIOA -> ODR &= ~(x);
}
//display five
void five() { //PA 4 6 8 11 12
  uint32_t x = 0x1950;
  GPIOA -> ODR &= ~(x);
} //display six
void six() { //PA 1 4 6 8 11 12
  uint32_t x = 0x1952;
  GPIOA -> ODR &= ~(x);
}
//display seven
void seven() { //PA 7 8 12
  uint32_t x = 0x1180;
  GPIOA -> ODR &= ~(x);
}
//display eight
void eight() { //PA 1 4 6 7 8 11 12
  uint32_t x = 0x19D2;
  GPIOA -> ODR &= ~(x);
}
//display nine
void nine() { //PA 4 6 7 8 11 12
  uint32_t x = 0x19D0;
  GPIOA -> ODR &= ~(x);
}

//deactivate all digits and segments
void blackout() {
  uint32_t x = 0x270;
  GPIOB -> ODR &= ~(x);
  x = 0x19F2;
  GPIOA -> ODR |= x;
}




void compare_display() { //it is a modificated version of one of my older functions for Project 1, so its name is old

  while (1) { // display a number all the time

//    ten_sec_counter++; // count to 10
    while (A_pressed != 1) //if not pressed to an keys after any interrupt go back to idle state and reset previous key presses-numbers
    	display_mode(mode);

  }
}







void find_digit(int d1, int d2, int d3, int d4) { // function that gets the digits of a number and displays it on the SSD
  switch (d1) { //first digit
  case 0:
    blackout();
    activate_d1();
    zero();
    my_delay();
    break;
  case 1:
    blackout();
    activate_d1();
    one();
    my_delay();
    break;
  case 2:
    blackout();
    activate_d1();
    two();
    my_delay();
    break;
  case 3:
    blackout();
    activate_d1();
    three();
    my_delay();
    break;
  case 4:
    blackout();
    activate_d1();
    four();
    my_delay();
    break;
  case 5:
    blackout();
    activate_d1();
    five();
    my_delay();
    break;
  case 6:
    blackout();
    activate_d1();
    six();
    my_delay();
    break;
  case 7:
    blackout();
    activate_d1();
    seven();
    my_delay();
    break;
  case 8:
    blackout();
    activate_d1();
    eight();
    my_delay();
    break;
  case 9:
    blackout();
    activate_d1();
    nine();
    my_delay();
    break;
  case 10: // if number is negative it will display - sign on the first digit
    blackout();
    activate_d1();
    GPIOA -> ODR &= ~(1U<< 11);
    my_delay();
    break;
  default: // for case -1 so deactivates d1
    deactivate_d1();
  }

  switch (d2) { //second digit
  case 0:
    blackout();
    activate_d2();
    zero();
    my_delay();
    break;
  case 1:
    blackout();
    activate_d2();
    one();
    my_delay();
    break;
  case 2:
    blackout();
    activate_d2();
    two();
    my_delay();
    break;
  case 3:
    blackout();
    activate_d2();
    three();
    my_delay();
    break;
  case 4:
    blackout();
    activate_d2();
    four();
    my_delay();
    break;
  case 5:
    blackout();
    activate_d2();
    five();
    my_delay();
    break;
  case 6:
    blackout();
    activate_d2();
    six();
    my_delay();
    break;
  case 7:
    blackout();
    activate_d2();
    seven();
    my_delay();
    break;
  case 8:
    blackout();
    activate_d2();
    eight();
    my_delay();
    break;
  case 9:
    blackout();
    activate_d2();
    nine();
    my_delay();
    break;
  default:// for case -1 so deactivates d2
    deactivate_d2();
  }

  switch (d3) { //third digit
  case 0:
    blackout();
    activate_d3();
    zero();
    my_delay();
    break;
  case 1:
    blackout();
    activate_d3();
    one();
    my_delay();
    break;
  case 2:
    blackout();
    activate_d3();
    two();
    my_delay();
    break;
  case 3:
    blackout();
    activate_d3();
    three();
    my_delay();
    break;
  case 4:
    blackout();
    activate_d3();
    four();
    my_delay();
    break;
  case 5:
    blackout();
    activate_d3();
    five();
    my_delay();
    break;
  case 6:
    blackout();
    activate_d3();
    six();
    my_delay();
    break;
  case 7:
    blackout();
    activate_d3();
    seven();
    my_delay();
    break;
  case 8:
    blackout();
    activate_d3();
    eight();
    my_delay();
    break;
  case 9:
    blackout();
    activate_d3();
    nine();
    my_delay();
    break;
  default:// for case -1 so deactivates d3
    deactivate_d3();
  }

  switch (d4) { //fourth digit
  case 0:
    blackout();
    activate_d4();
    zero();
    my_delay();

    break;
  case 1:
    blackout();
    activate_d4();
    one();
    my_delay();
    break;

  case 2:
    blackout();
    activate_d4();
    two();
    my_delay();

    break;
  case 3:
    blackout();
    activate_d4();
    three();
    my_delay();

    break;
  case 4:
    blackout();
    activate_d4();
    four();
    my_delay();

    break;
  case 5:
    blackout();
    activate_d4();
    five();
    my_delay();

    break;
  case 6:
    blackout();
    activate_d4();
    six();
    my_delay();

    break;
  case 7:
    blackout();
    activate_d4();
    seven();
    my_delay();

    break;
  case 8:
    blackout();
    activate_d4();
    eight();
    my_delay();

    break;
  case 9:
    blackout();
    activate_d4();
    nine();
    my_delay();

    break;
  default:// for case -1 so deactivates d4
    deactivate_d4();
  }
}

void display_dot() { // function that displays the dot at given digits
  if (dot == 1) { // if dot is 1, our number is like 9.4321 for example and it displays dot on first digit
    activate_d1();
    GPIOA -> ODR &= ~(1U<< 5);
    my_delay();
  } else if (dot == 2) {// if dot is 1, our number is like 94.321 for example and it displays dot on second digit
    activate_d2();
    GPIOA -> ODR &= ~(1U<< 5);
    my_delay();
  } else if (dot == 3) {// if dot is 3, our number is like 943.21 for example and it displays dot on third digit
    activate_d3();
    GPIOA -> ODR &= ~(1U<< 5);
    my_delay();
  } else if (dot == 4) {// if dot is 1, our number is like 9432.1 for example and it displays dot on fourth digit
    if (!(res / pow(10, 4 - dot_count) / check_dot == 1)) {
      activate_d4();
      GPIOA -> ODR &= ~(1U<< 5);
      my_delay();
    }
  }
}

void display_mode(int mode){

	if(mode==1){

		display_sine();

	}
	else if(mode==2){
		display_sqre();

	}
	else if(mode==3){
		display_tria();
	}
	else if(mode==4){
		display_toth();

	}
	else if(mode==5){
		display_gaus();
	}
	else if(mode==6){
		display_digi();
	}
	else{
		display_invalid();

}}


void display_invalid() { //function that displays InVd for few at the screen when an overflow happens
  volatile uint32_t x;

    blackout();
    activate_d1();
    one(); // I
    my_delay();
    blackout();
    activate_d2();
    x = 0x1802; // 0001 1000 0000 0010 N 1 11 12
    GPIOA -> ODR &= ~(x);
    my_delay();
    blackout();
    activate_d3();
    x = 0x10D2; //0001 0000 1101 0010 U
    GPIOA -> ODR &= ~(x);
    my_delay();
    blackout();
    activate_d4();
    x = 0x1892; //0000 0000 0101 0010 d
    GPIOA -> ODR &= ~(x);
    my_delay();
    blackout();
}

void display_sine(){
	volatile uint32_t x;


	blackout();
	activate_d1();
	five(); // 5
	my_delay();
	blackout();
	activate_d2();
	one(); // I
	my_delay();
	blackout();
	activate_d3();
	x = 0x1802; // 0001 1000 0000 0010 N 1 11 12
   GPIOA -> ODR &= ~(x);
   my_delay();
   blackout();
   activate_d4();
   x = 0x952; //0000 0000 0101 0010 d
   GPIOA -> ODR &= ~(x);
   my_delay();
   blackout();
}
void display_tria(){
	volatile uint32_t x;


	blackout();
	activate_d1();
	x = 0x1902; //T
	GPIOA -> ODR &= ~(x);
	my_delay();
	blackout();
	activate_d2();
	x = 0x802; // R
	   GPIOA -> ODR &= ~(x);
	my_delay();
blackout();
activate_d3();
one();
my_delay();
blackout();
activate_d4();
x = 0x19C2; //A
GPIOA -> ODR &= ~(x);
my_delay();
blackout();
}

void display_sqre(){
	volatile uint32_t x;


	blackout();
	activate_d1();
	five();
	my_delay();
	blackout();
	activate_d2();
	x = 0x19C0; // Q
	   GPIOA -> ODR &= ~(x);
	my_delay();
blackout();
activate_d3();
x = 0x802; // R
	   GPIOA -> ODR &= ~(x);
my_delay();
blackout();
activate_d4();
x = 0x952; //E
GPIOA -> ODR &= ~(x);
my_delay();
blackout();
}

void display_toth(){
	volatile uint32_t x;


	blackout();
	activate_d1();
	x = 0x1902; //T
		GPIOA -> ODR &= ~(x);
	my_delay();
	blackout();
	activate_d2();
	zero();
	my_delay();
blackout();
activate_d3();
x = 0x1902; //T
	GPIOA -> ODR &= ~(x);
my_delay();
blackout();
activate_d4();
x = 0x18C2; //h
GPIOA -> ODR &= ~(x);
my_delay();
blackout();
}

void display_digi(){
	volatile uint32_t x;


	blackout();
	activate_d1();
    x = 0x1892; //0000 0000 0101 0010 d
    GPIOA -> ODR &= ~(x);
	my_delay();
	blackout();
	activate_d2();
	one();
	my_delay();
blackout();
activate_d3();
six();
my_delay();
blackout();
activate_d4();
one();
my_delay();
blackout();
}


void display_gaus(){
	volatile uint32_t x;


	blackout();
	activate_d1();
six();
	my_delay();
	blackout();
	activate_d2();
	x = 0x19C2; //A
	GPIOA -> ODR &= ~(x);
	my_delay();
blackout();
activate_d3();
x = 0x10D2; //0001 0000 1101 0010 U
GPIOA -> ODR &= ~(x);
my_delay();
blackout();
activate_d4();
five();
my_delay();
blackout();
}

void PWM_init(void) {
 RCC -> IOPENR |= (2U << 0); // B open
 RCC -> PLLCFGR |= (2U << 0); //hsı16
 RCC -> PLLCFGR |= (1U << 24); // set q n r constant
 RCC -> PLLCFGR |= (1U << 25);// SETQ
 RCC -> PLLCFGR |= (1U << 28);//ENABLE N
 RCC -> PLLCFGR |= (1U << 29);// SET N
 RCC -> PLLCFGR &= ~(7U << 4);
 RCC -> PLLCFGR &= ~(127U << 8);
 RCC -> PLLCFGR |= (8U << 8);//SET R
 GPIOB -> MODER &= ~(3U << 2 * 3);
 GPIOB -> MODER |= (2U << 2 * 3); // making pb3 alterna mode
 // PB3 AF2 timer2 chanel 2
 GPIOB -> AFR[0] &= ~(0xFU << 4 * 3);
 GPIOB -> AFR[0] |= (2U << 4 * 3); // AFSEL11 to AF2 0010
 RCC -> APBENR1 |= (1U << 0); //timer 2 clock open
 TIM2 -> CR1 = 0; // reset
 TIM2 -> CR1 |= (1U << 7); //arpe
 TIM2 -> PSC = 0; // set zero
 TIM2 -> ARR =64;
 TIM2 -> DIER |= (1U << 0);
 TIM2 -> CNT = 4294967295; //max value
 TIM2 -> CCMR1 &= ~(0x7U << 12); // clear
 TIM2 -> CCMR1 &= ~(0x1U << 24); // clean
 TIM2 -> CCMR1 |= (0x6U << 12); //0110
 TIM2 -> CCMR1 |= (1U << 11); //preload enable
 TIM2 -> CCER |= (1U << 4); //cmpr2 enable
 duty = 0;
 TIM2 -> CCR2 = duty;
 TIM2 -> CR1 |= (1U << 0); //enable tim2
 NVIC_SetPriority(TIM2_IRQn, 0); //setting priority and enabling
 NVIC_EnableIRQ(TIM2_IRQn);
}
void init_timer1() { // fs = 8kHz
 // Config Stars
 RCC -> APBENR2 |= (1U << 11);
 TIM1 -> CR1 = 0;
 TIM1 -> CR1 |= (1 << 7);
 TIM1 -> CNT = 0;
 TIM1 -> PSC = 999;
 freq[1]=2;
 TIM1 -> ARR = (freq[3]*1000+freq[2]*100+freq[1]*10+freq[0]); // arttırınca frekans düşer süre uzar
 TIM1 -> DIER |= (1 << 0);
 TIM1 -> CR1 |= (1 << 0);
 NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 0);
 NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
}
int counter =0 ;
int sayac =0;

void mode_arranger(){

	if(mode==2){ ///square

		if(sayac==0){
			duty=(amp[3]*1000+amp[2]*100+amp[1]*10+amp[0]); sayac=1;
		  }
		else if(sayac==1){
				 duty=0; sayac=0;
			 }
	}

	else if(mode==3){
		counter=0;
		 if(counter == 0 ){			////triangler
				duty = (amp[3]*1000+amp[2]*100+amp[1]*10+amp[0]) +1;

			    if ( duty == 999){ counter =1;}
				}
				else if (counter ==1) {

				duty = (amp[3]*1000+amp[2]*100+amp[1]*10+amp[0]) -1;
				if (duty == 0) counter =0;}
	}

	else if(mode==1){
		counter=0;
		if(counter == 0 ){			////sine
		 	 	 			 	 	duty = duty +3;
		 	 	 			 	 	if ( duty == 24){ counter =1;
		 	 	 		 }}
		 	 	 		 else if(counter ==1) {
		 	 	 				 	duty = duty +2;
		 	 	 				 	if (duty == 50) {counter =2;
		 	 	 		 }}
		 	 	 		 else if(counter ==2) {
		 	 	 			 	 	duty = duty +1;
		 	 	 		 			if (duty == 75){ counter =3;
		 	 	 		 }}
		 	 	 		 else if(counter ==3) {
		 	 	 				 	duty = duty + 1;
		 	 	 				 	if (duty == 100){ counter =4;
		 	 	 		 }}
		 	 	 		 else if(counter ==4){
		 	 	 			 duty = duty - 1;
		 	 	 					 if(duty<75){counter =5;
		 	 	 		 }}
		 	 	 		 else if(counter ==5){
		 	 	 		 			 duty = duty-1;
		 	 	 		 			if(duty<50){counter =6;
		 	 	 		 }}
		 	 	 		 else if(counter ==6){
		 	 	 		 			 duty = duty-2;
		 	 	 		 			 if(duty<24){counter =7;
		 	 	 		 }}
		 	 	 		 else if(counter ==7){
		 	 	 			 	 	 duty= duty-3;
		 	 	 			 	 	 if(duty==0){counter=0;
		 	 	 		 }}
	}

	else if(mode==4){
		counter=0;
		if(counter == 0){				////sawtooth
			 duty= duty+1;
		 	 if(duty == 9900){ counter =1;}}
		 else if(counter ==1){ duty=0;
		 	 if (duty==0){counter=0;}
		  }
	}
	else if(mode==5){
		counter=0;
		 if(counter == 0 ){			////white noise
		 	 	 duty= +99;
		 	 	 if(duty>100){counter=1;}}
	 	 	 else if(counter==1){ duty=-198;
	 	 	 	 if(duty>=100){counter=0; }}

	}}

void TIM1_BRK_UP_TRG_COM_IRQHandler(void) {

	 mode_arranger();

	 TIM2 -> CCR2 = duty;

	 TIM1 -> SR &= ~(1U << 0);
}

void TIM2_IRQHandler(void) {

 TIM2 -> SR &= ~(1U << 0);
}








