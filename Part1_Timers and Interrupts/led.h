/*
 * File:        led.h
 * Purpose:     Header file for led.h; contains functions for LED control
 *
 * Notes:		
 *
 */
 
#ifndef LED
#define LED

#define LED_ON 0
#define LED_OFF 1

#define LED_RED_PIN_NUM 22
#define LED_BLUE_PIN_NUM 21
#define LED_GREEN_PIN_NUM 26


// Initialize the LEDs
void LED_Init(void);

/*
	Sets the state for all LEDs based on boolean parameters.  Must be called
	after LED_init()

	red_set: int (0 or 1); value to set Red LED state to
	blue_set: int (0 or 1); value to set blue LED state to
	green_set: int (0 or 1); value to set green LED state to
*/
void set_LED_states(int red_set, int blue_set, int green_set);

/*
	Toggles the state for all LEDs based on boolean parameters.  Must be called
	after LED_init()

	red_t: int (0 or 1); if set -> toggle LED.  if clear -> do nothing
	etc.
*/
void toggle_LED_states(int red_t, int blue_t, int green_t);

void clear_LED_states(void);
/*
	Function that adds an integer to the INCREMENT_DELAY value to add
	a delay into LED displays
*/
void delay(void);

#endif
