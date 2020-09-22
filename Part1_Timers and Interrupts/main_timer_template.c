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
#include "led.h"
#include "isr.h"
#include <stdio.h>

/*From clock setup 0 in system_MK64f12.c*/
#define DEFAULT_SYSTEM_CLOCK 20485760u /* Default System clock value */

#define SW2_PIN_NUM 6
#define SW3_PIN_NUM 4

#define SW_DEPRESSED 0

#define GPIO_PDDR_INPUT 0
#define GPIO_PDDR_OUTPUT 1

void initPDB(void);
void initGPIO(void);
void initFTM(void);
void initInterrupts(void);
void Button_Init(void);

int main(void){
	//initializations
	initGPIO();
	//initPDB();
	//initFTM();
	//uart_init();
	//initInterrupts();

	toggle_LED_states(1, 0, 0);
	delay();
	toggle_LED_states(1, 0, 0);
	delay();
	toggle_LED_states(1, 0, 0);

	
	for(;;){
		//To infinity and beyond
	}
}

void initPDB(void){
	//Enable clock for PDB module
	SIM_SCGC6 |= SIM_SCGC6_PDB_MASK;
	
	// Set continuous mode, prescaler of 128, multiplication factor of 20,
	// software triggering, and PDB enabled
	
	// Contiuous mode set
	PDB0_SC |= PDB_SC_CONT_MASK;
	// Prescalar Set to 128 (111)
	PDB0_SC &= !(PDB_SC_PRESCALER_MASK);
	PDB0_SC |= PDB_SC_PRESCALER(7);
	// Multiplication factor set (10)
	PDB0_SC &= !(PDB_SC_MULT_MASK);
	PDB0_SC |= PDB_SC_MULT(2);
	// Software Triggering Reset
	PDB0_SC &= ~PDB_SC_TRGSEL_MASK;
	PDB0_SC |= PDB_SC_TRGSEL(15); // Software trigger is selected
	// Enable PDB
	PDB0_SC |= PDB_SC_PDBEN_MASK;


	//Set the mod field to get a 1 second period.
	//There is a division by 2 to make the LED blinking period 1 second.
	//This translates to two mod counts in one second (one for on, one for off)
	PDB0_MOD &= !(PDB_MOD_MOD_MASK);
	PDB0_MOD |= PDB_MOD_MOD(4001);

	//Configure the Interrupt Delay register.
	PDB0_IDLY = 10;
	
	//Enable the interrupt mask.
	PDB0_SC |= PDB_SC_PDBIE_MASK;
	
	// Enable LDOK to have PDB0_SC register changes loaded.
	PDB0_SC |= PDB_SC_LDOK_MASK;
	
	// Initialize PDB0 Interrupts
	NVIC_EnableIRQ(PDB0_IRQn);
	
	// Start the timer with a software trigger
	PDB0_SC |= PDB_SC_SWTRIG_MASK;
	
	return;
}

void initFTM(void){
	//Enable clock for FTM module (use FTM0)
	SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK;
	
	//turn off FTM Mode to  write protection;
	FTM0_MODE |= FTM_MODE_WPDIS_MASK;

	//divide the input clock down by 128,
	FTM0_SC |= FTM_SC_PS(1);

	//reset the counter to zero
	FTM0_CNT = 0;

	//Set the overflow rate
	//(Sysclock/128)- clock after prescaler
	//(Sysclock/128)/1000- slow down by a factor of 1000 to go from
	//Mhz to Khz, then 1/KHz = msec
	//Every 1msec, the FTM counter will set the overflow flag (TOF) and
	FTM0->MOD = (DEFAULT_SYSTEM_CLOCK/(1<<7))/1000;

	//Select the System Clock
	FTM0_SC |= FTM_SC_CLKS(1);

	//Enable the interrupt mask. Timer overflow Interrupt enable
	NVIC_EnableIRQ(FTM0_IRQn);
	FTM0_SC |= FTM_SC_TOIE_MASK;

	return;
}

void initGPIO(void){
	//initialize push buttons and LEDs

	//initialize clocks for each different port used.
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;

	// Initialize LEDs
	LED_Init();
	Button_Init();
	uart_init();

	// interrupt configuration for SW3(Rising Edge) and SW2 (Either)
	PORTA_PCR4 &= !PORT_PCR_IRQC_MASK;
	PORTA_PCR4 |= PORT_PCR_IRQC(9);	// Rising edge interrupt
	
	PORTC_PCR6 &= !PORT_PCR_IRQC_MASK;
	PORTC_PCR6 |= PORT_PCR_IRQC(11); // Either edge interrupt

	return;
}

void initInterrupts(void){
	/*Can find these in MK64F12.h*/
	// Enable NVIC for portA,portC, PDB0,FTM0
	//NVIC_EnableIRQ(PORTA_IRQn);
	//NVIC_EnableIRQ(PORTC_IRQn);
	
	//NVIC_EnableIRQ(FTM0_IRQn);

	return;
}

void Button_Init(void){
	// Enable clock for SW2 -> PTC6 and SW3 -> PTA4
	 SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
	 SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	// Configure the Mux for the button
	 PORTC_PCR6 |= PORT_PCR_MUX(1);
	 PORTA_PCR4 |= PORT_PCR_MUX(1);

	// Set the push button as an input
	GPIOC_PDDR |= (GPIO_PDDR_INPUT << SW2_PIN_NUM);
	GPIOA_PDDR |= (GPIO_PDDR_INPUT << SW3_PIN_NUM);
	 
}
