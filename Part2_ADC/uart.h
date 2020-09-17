/*
 * File:        uart.h
 * Purpose:     Header file for uart.c
 *
 * Notes:		
 *
 */
 
#ifndef UART
#define UART

void uart_init(void);

void uart_put(char *ptr_str);
uint8_t uart_getchar(void);
void uart_putchar(char ch);
 
 
#endif
