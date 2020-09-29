/* 
Title:  	led.c
Purpose:	This program will control and LED.
Name:   	Dan Popp
Date:   	9/10/20
*/

#include "led.h"
#include "MK64F12.h"

#define INCREMENT_DELAY 2000000

#define GPIO_PDDR_INPUT 0
#define GPIO_PDDR_OUTPUT 1

void LED_Init(void){
	// Enable clocks on Ports B and E for LED timing
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
	 
	// Configure the Signal Multiplexer for GPIO
	PORTB_PCR22 |= PORT_PCR_MUX(1);
	PORTB_PCR21 |= PORT_PCR_MUX(1);
	PORTE_PCR26 |= PORT_PCR_MUX(1);
	
	// Switch the GPIO pins to output mode
	GPIOB_PDDR |= (GPIO_PDDR_OUTPUT << LED_RED_PIN_NUM) | (GPIO_PDDR_OUTPUT << LED_BLUE_PIN_NUM);
	GPIOE_PDDR |= (GPIO_PDDR_OUTPUT << LED_GREEN_PIN_NUM);
	
	// Set LED pins for output
	 

	// Turn off the LEDs
	clear_LED_states();
   
}

void set_LED_states(int red_set, int blue_set, int green_set) {
	// 0 on GPIO pin turns LED on.
	GPIOB_PCOR = (red_set << LED_RED_PIN_NUM) | (blue_set << LED_BLUE_PIN_NUM);
	GPIOE_PCOR = (green_set << LED_GREEN_PIN_NUM);
}

void toggle_LED_states(int red_t, int blue_t, int green_t){
	GPIOB_PTOR = (red_t << LED_RED_PIN_NUM) | (blue_t << LED_BLUE_PIN_NUM);
	GPIOE_PTOR = (green_t << LED_GREEN_PIN_NUM);
}

void clear_LED_states() {
	// 1 on GPIO pin turns LED off.
	GPIOB_PDOR = (1 << LED_RED_PIN_NUM) | (1 << LED_BLUE_PIN_NUM);
	GPIOE_PDOR = (1 << LED_GREEN_PIN_NUM);
}

void delay(void) {
	for (int i = 0; i < INCREMENT_DELAY; i++);
}
