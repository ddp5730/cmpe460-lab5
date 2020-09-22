/*
 * isr.c
 */

#include "isr.h"
#include "MK64F12.h"
#include <stdio.h>
#include "uart.h"
#include "led.h"

#define SW2_PIN_NUM 6
#define SW3_PIN_NUM 4

#define TEST "SW3 Interrupt triggered\r\n"

//variables global to the IRQ handlers which dictates if timer is enabled &  timer counter
int sw2en;
int sw2_counter;

void PDB0_IRQHandler(void){ //For PDB timer
	// Clear interrupt flag
	PDB0_SC &= ~PDB_SC_PDBIF_MASK;
	
	// Toggle LED output state
	toggle_LED_states(1, 0, 0);

	return;
}
	
void FTM0_IRQHandler(void){ //For FTM timer
	FTM0_SC &= ~FTM_SC_TOF_MASK;
	if(sw2en)
	{
		sw2_counter++;
	}
	
	return;
}
	
// SW3 toggles whether or not the blinking LED is toggled
void PORTA_IRQHandler(void){ //For switch 3
	
	
	
	uart_put(TEST);
	
	
	// Check if interrupt is detected
	if (PORTA_ISFR & (1 << SW3_PIN_NUM)) {
		// Clear interrupt
		PORTA_ISFR |= (1 << SW3_PIN_NUM);		// What idiot decided 
																				// writing a 1 clears the flag
		
		if (PDB0_SC & PDB_SC_PDBEN_MASK) {
			// Disable timer
			PDB0_SC &= ~PDB_SC_PDBEN_MASK;
		}
		else {
			// Enable timer and trigger counter
			PDB0_SC |= PDB_SC_PDBEN_MASK;
			PDB0_SC |= PDB_SC_SWTRIG_MASK;
		}
	}
	
	return;
}
	
// SW2 times how long it is depressed for and prints the results over UART.
void PORTC_IRQHandler(void){ //For switch 2
	
	/*
	if(PORTA_ISFR & (1 << SW3_PIN_NUM))
	{
		
		PORTA_ISFR |= 1 << SW3_PIN_NUM;
		if(FTM0_SC & FTM_SC_CLKS)
		{
			FTM0_SC &= ~FTM_SC_CLKS_MASK;
			sw2en = 0;
		}
		else
		{
			FTM0_SC |= FTM_SC_CLKS(0b01);
			FTM0_SYNC |= 
			sw2en = 1;
			sw2_counter = 0;
		}
	}
	*/
	
	return;
}
