/*
 * File:        uart.c
 * Purpose:     Provide UART routines for serial IO
 *
 * Notes:		
 *
 */

#include "MK64F12.h"
#include "fifo.h"
#include "uart.h"
#define BAUD_RATE 9600      //default baud rate 
#define SYS_CLOCK 20485760 //default system clock (see DEFAULT_SYSTEM_CLOCK  in system_MK64F12.c)
#define RXB_SIZE 64
#define TXB_SIZE 64

char rx_buf[RXB_SIZE];
char tx_buf[TXB_SIZE];

struct Q rxQ = {rx_buf, RXB_SIZE, 0, 0};
struct Q txQ = {tx_buf, TXB_SIZE, 0, 0};

void uart_init()
{
	//define variables for baud rate and baud rate fine adjust
	uint16_t ubd, brfa;
	
	//Enable clock for UART
	SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;

	//Configure the port control register to alternative 3 (which is UART mode for K64)
	PORTB_PCR16 |= PORT_PCR_MUX(3);
	PORTB_PCR17 |= PORT_PCR_MUX(3);

	/*Configure the UART for establishing serial communication*/

	//Disable transmitter and receiver until proper settings are chosen for the UART module
	UART0_C2 &= ~UART_C2_TE_MASK;
	UART0_C2 &= ~UART_C2_RE_MASK;

	//Select default transmission/reception settings for serial communication of UART by clearing the control register 1
	UART0_C1 = 0x00;	// Clear Register to set defaults


	//UART Baud rate is calculated by: baud rate = UART module clock / (16 × (SBR[12:0] + BRFD))
	//13 bits of SBR are shared by the 8 bits of UART3_BDL and the lower 5 bits of UART3_BDH 
	//BRFD is dependent on BRFA, refer Table 52-234 in K64 reference manual
	//BRFA is defined by the lower 4 bits of control register, UART0_C4 

	//calculate baud rate settings: ubd = UART module clock/16* baud rate
	ubd = (uint16_t)((SYS_CLOCK)/(BAUD_RATE * 16));  // 133.371; truncates to 133

	//clear SBR bits of BDH
	UART0_BDH &= ~UART_BDH_SBR_MASK;
	UART0_BDL &= ~UART_BDL_SBR_MASK;

	//distribute this ubd in BDH and BDL
	UART0_BDH |= UART_BDH_SBR(ubd>>8);	// Only want 5 MSB of ubd
	UART0_BDL |= UART_BDL_SBR(ubd); // Only want 8 LSB of ubd


	//BRFD = (1/32)*BRFA 
	//make the baud rate closer to the desired value by using BRFA
	brfa = (((SYS_CLOCK*32)/(BAUD_RATE * 16)) - (ubd * 32));	// 11.87; truncates to 11

	//write the value of brfa in UART0_C4
	UART0_C4 &= ~UART_C4_BRFA_MASK;
	UART0_C4 |= UART_C4_BRFA(brfa);
		
	//Enable transmitter and receiver of UART
	UART0_C2 |= UART_C2_TE_MASK | UART_C2_RE_MASK | UART_C2_TIE_MASK | UART_C2_RIE_MASK;
	
	NVIC_EnableIRQ(UART0_RX_TX_IRQn);
}

int uart0_put(char *str)
{
	int i = (int)str;
	while(*str != 0) 
	{
		if(uart0_putchar(*(str++))) return (int)str - i;
	}
	return 0;
}
int uart0_get(char **str, int n)
{
	int i;
	for(i = 0; i < n; i++)
	{
		if(uart0_getchar(*str + i)) return i + 1;
	}
	return 0;
}

int uart0_putchar(char c)
{
	int r = enQ(&txQ, c);
	UART0_C2 |= UART_C2_TIE_MASK;
	return r;
}

int uart0_getchar(char *c)
{
	return deQ(&rxQ, c);
}

void UART0_RX_TX_IRQHandler()
{
	if(UART0_S1 & UART_S1_TDRE_MASK)
	{
		if(deQ(&txQ, &UART0_D)) UART0_C2 &= ~UART_C2_TIE_MASK;
	}
	if(UART0_S1 & UART_S1_RDRF_MASK)
	{
		if(enQ(&rxQ, UART0_D)) UART0_C2 &= ~UART_C2_RIE_MASK;
	}
}