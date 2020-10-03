/*
 * Main module for testing the PWM Code for the K64F
 * 
 * Author:  
 * Created:  
 * Modified: Carson Clarke-Magrab <ctc7359@rit.edu> 
 */

#include "MK64F12.h"
#include "uart.h"
#include "PWM.h"

#define COIL_A_PIN 0
#define COIL_B_PIN 1
#define COIL_C_PIN 2
#define COIL_D_PIN 3

void delay(int del);
void init_GPIO(void);
void set_coil_states(int on, int off1, int off2, int off3);

int main(void) {
	// Initialize UART and PWM
	FTM0_init();
	uart_init();

	// Print welcome over serial
	uart0_put("Running... \n\r");
	
	/* Part 1 - UNCOMMENT THIS
	// Generate 20% duty cycle at 10kHz
	FTM0_set_duty_cycle(60, 10000, 0);
	
	for(;;) ;  //then loop forever
	*/
	
	/*
	// Part 2 - UNCOMMENT THIS
	for(;;)  //loop forever
	{
		uint16_t dc = 0;
		uint16_t freq = 10000; // Frequency = 10 kHz 
		uint16_t dir = 0;
		char c = 48;
		int i=0;
		
		// 0 to 100% duty cycle in forward direction
		for (i=0; i<100; i++) {
		    FTM0_set_duty_cycle(i, freq, dir);
			
			delay(10);
		}
		
		// 100% down to 0% duty cycle in the forward direction
		for (i=100; i>=0; i--) {
		    FTM0_set_duty_cycle(i, freq, dir);
			
			delay(10);
		}
		
		dir = !dir;
		
		// 0 to 100% duty cycle in reverse direction
		for (i=0; i<100; i++) {
		    FTM0_set_duty_cycle(i, freq, dir);
			
			delay(10);
		}
		
		// 100% down to 0% duty cycle in the reverse direction
		for (i=100; i>=0; i--) {
		    FTM0_set_duty_cycle(i, freq, dir);
			
			delay(10);
		}

	} */
	
	// Part 3
	init_GPIO();
	
	int forward = 1;
	int phase = 0;
	
	while(1) {
			// Turn off all coils; set GPIO pins to 0
			GPIOD_PCOR = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);
		
		// Set one pin high ata  time
		if (forward) {
			if (phase == 0) {
				// Turn on coil A
				set_coil_states(COIL_A_PIN, COIL_B_PIN, COIL_C_PIN, COIL_D_PIN);
				phase++;
			}
			else if (phase == 1) {
				// Turn on coil B
				set_coil_states(COIL_B_PIN, COIL_A_PIN, COIL_C_PIN, COIL_D_PIN);
				phase++;
			}
			else if (phase ==2) {
				// Turn on coil C
				set_coil_states(COIL_C_PIN, COIL_A_PIN, COIL_B_PIN, COIL_D_PIN);
				phase++;
			}
			else {
				// Turn on coil D
				set_coil_states(COIL_D_PIN, COIL_A_PIN, COIL_B_PIN, COIL_C_PIN);
				phase = 0;
			}
		}
		else {
			// Reverse
			if (phase == 0) {
				// Turn on coil D
				set_coil_states(COIL_D_PIN, COIL_A_PIN, COIL_B_PIN, COIL_C_PIN);
				phase++;
			}
			else if (phase == 1) {
				// Turn on coil C
				set_coil_states(COIL_C_PIN, COIL_A_PIN, COIL_B_PIN, COIL_D_PIN);
				phase++;
			}
			else if (phase ==2) {
				// Turn on coil B
				set_coil_states(COIL_B_PIN, COIL_A_PIN, COIL_C_PIN, COIL_D_PIN);
				phase++;
			}
			else {
				// Turn on coil A
				set_coil_states(COIL_A_PIN, COIL_B_PIN, COIL_C_PIN, COIL_D_PIN);
				phase = 0;
			}
		}
		
		delay(10);
		
	}	

	return 0;
}

void init_GPIO() {
	// Enable clocks to port D
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
	
	// Configure Signal Mux
  PORTD_PCR0 |= PORT_PCR_MUX(1);
	PORTD_PCR1 |= PORT_PCR_MUX(1);
	PORTD_PCR2 |= PORT_PCR_MUX(1);
	PORTD_PCR3 |= PORT_PCR_MUX(1);
	
	// Configure GPIO pins for output
	GPIOD_PDDR |= (1 << COIL_A_PIN);
	GPIOD_PDDR |= (1 << COIL_B_PIN);
	GPIOD_PDDR |= (1 << COIL_C_PIN);
	GPIOD_PDDR |= (1 << COIL_D_PIN);
	
}

void set_coil_states(int on, int off1, int off2, int off3) {
	GPIOD_PSOR = (1 << on);
	GPIOD_PCOR = (1 << off1) | (1 << off2) | (1 << off3);
}


/**
 * Waits for a delay (in milliseconds)
 * 
 * del - The delay in milliseconds
 */
void delay(int del){
	int i;
	for (i=0; i<del*50000; i++){
		// Do nothing
	}
}
