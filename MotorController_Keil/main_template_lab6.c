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
	uart_init();
	
	uart0_put("yolo\r\n");
	
	char buf[10];
	int i = 0;
	while(1)
	{
		char c;
		if(!uart0_getchar(&c))
		{
			
			if(i != sizeof(buf) - 2) 
			{
				uart0_putchar(c);
				buf[i++] = c;
			}
			if(c == '\r') 
			{
				uart0_putchar('\n');
				buf[i] = 0;
				uart0_put(buf);
				i = 0;
				uart0_putchar('\n');
			}
		}
	}
}
void delay(int del){
	int i;
	for (i=0; i<del*50000; i++){
		// Do nothing
	}
}
