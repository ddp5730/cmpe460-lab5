/*
* Rochester Institute of Technology
* Department of Computer Engineering
* CMPE 460  Interfacing Digital Electronics
* Spring 2016
*
* Filename: main_timer_template.c
*/

#include "MK64F12.h"
#include "uart.h"
#include "isr.h"
#include <stdio.h>

/*From clock setup 0 in system_MK64f12.c*/
#define DEFAULT_SYSTEM_CLOCK 20485760u /* Default System clock value */

void initPDB(void);
void initGPIO(void);
void initFTM(void);
void initInterrupts(void);

int main(void){
	//initializations
	initPDB();
	initGPIO();
	initFTM();
	uart_init();
	initInterrupts();

	for(;;){
		//To infinity and beyond
	}
}

void initPDB(void){
	//Enable clock for PDB module


	// Set continuous mode, prescaler of 128, multiplication factor of 20,
	// software triggering, and PDB enabled



	//Set the mod field to get a 1 second period.
	//There is a division by 2 to make the LED blinking period 1 second.
	//This translates to two mod counts in one second (one for on, one for off)


	//Configure the Interrupt Delay register.
	PDB0_IDLY = 10;

	//Enable the interrupt mask.


	//Enable LDOK to have PDB0_SC register changes loaded.


	return;
}

void initFTM(void){
	//Enable clock for FTM module (use FTM0)


	//turn off FTM Mode to  write protection;


	//divide the input clock down by 128,


	//reset the counter to zero


	//Set the overflow rate
	//(Sysclock/128)- clock after prescaler
	//(Sysclock/128)/1000- slow down by a factor of 1000 to go from
	//Mhz to Khz, then 1/KHz = msec
	//Every 1msec, the FTM counter will set the overflow flag (TOF) and
	FTM0->MOD = (DEFAULT_SYSTEM_CLOCK/(1<<7))/1000;

	//Select the System Clock


	//Enable the interrupt mask. Timer overflow Interrupt enable


	return;
}

void initGPIO(void){
	//initialize push buttons and LEDs

	//initialize clocks for each different port used.


	//Configure Port Control Register for Inputs with pull enable and pull up resistor

	// Configure mux for Outputs


	// Switch the GPIO pins to output mode (Red and Blue LEDs)


	// Turn off the LEDs

	// Set the push buttons as an input


	// interrupt configuration for SW3(Rising Edge) and SW2 (Either)


	return;
}

void initInterrupts(void){
	/*Can find these in MK64F12.h*/
	// Enable NVIC for portA,portC, PDB0,FTM0


	return;
}
