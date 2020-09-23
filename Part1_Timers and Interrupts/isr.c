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

#define SW3_INTERRUPT "SW3 Interrupt triggered\r\n"
#define SW2_PRESSED "SW2 button pressed\r\n"


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
	
	uart_put(SW3_INTERRUPT);
	
	
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
		
	if(PORTC_ISFR & (1 << SW2_PIN_NUM))
	{
		
		PORTC_ISFR |= 1 << SW2_PIN_NUM;
		if(FTM0_SC & FTM_SC_CLKS_MASK)
		{
			
			// Device was enabled
			FTM0_SC &= ~FTM_SC_CLKS_MASK;
			FTM0_SC |= FTM_SC_CLKS(0);
			sw2en = 0;
			
			// Print out result to display (in milliseconds)
			char out_str[100];
			
			sprintf(out_str, "Button SW2 was held down for %d ms\r\n", sw2_counter);
			uart_put(out_str);
		}
		else
		{
			// Device was disabled
			
			uart_put(SW2_PRESSED);
			
			FTM0_SC &= ~FTM_SC_CLKS_MASK;
			FTM0_SC |= FTM_SC_CLKS(1);
			sw2en = 1;
			sw2_counter = 0;
		}
	}
	
	
	return;
}
