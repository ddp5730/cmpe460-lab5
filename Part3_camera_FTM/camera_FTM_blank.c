/*
 * Freescale Cup linescan camera code
 *
 *	This method of capturing data from the line
 *	scan cameras uses a flex timer module, periodic
 *	interrupt timer, an ADC, and some GPIOs.
 *	CLK and SI are driven with GPIO because the FTM2
 *	module used doesn't have any output pins on the
 * 	development board. The PIT timer is used to 
 *  control the integration period. When it overflows
 * 	it enables interrupts from the FTM2 module and then
 *	the FTM2 and ADC are active for 128 clock cycles to
 *	generate the camera signals and read the camera 
 *  output.
 *
 *	PTB8			- camera CLK
 *	PTB23 		- camera SI
 *  ADC0_DP1 	- camera AOut
 *
 * Author:  Alex Avery
 * Created:  11/20/15
 * Modified:  11/23/15
 */

#include "MK64F12.h"
#include "uart.h"
#include "stdio.h"
#include "led.h"

// Default System clock value
// period = 1/20485760  = 4.8814395e-8
#define DEFAULT_SYSTEM_CLOCK 20485760u 
// Integration time (seconds)
// Determines how high the camera values are
// Don't exceed 100ms or the caps will saturate
// Must be above 1.25 ms based on camera clk 
//	(camera clk is the mod value set in FTM2)
#define INTEGRATION_TIME .0075f

#define CLK_PIN_NUM 9
#define SI_PIN_NUM 23

#define NL "\r\n"
#define RESET "Camera Program Started"

void init_FTM2(void);
void init_GPIO(void);
void init_PIT(void);
void init_ADC0(void);
void FTM2_IRQHandler(void);
void PIT1_IRQHandler(void);
void ADC0_IRQHandler(void);

// Pixel counter for camera logic
// Starts at -2 so that the SI pulse occurs
//		ADC reads start
int pixcnt = -2;
// clkval toggles with each FTM interrupt
int clkval = 0;
// line stores the current array of camera data
uint16_t line[128];

// These variables are for streaming the camera
//	 data over UART
int debugcamdata = 1;
int capcnt = 0;
char str[100];

// ADC0VAL holds the current ADC value
uint16_t ADC0VAL;

int main(void)
{
	int i;
	
	uart_init();
	init_GPIO(); // For CLK and SI output on GPIO
	init_ADC0();
	init_FTM2(); // To generate CLK, SI, and trigger ADC
	init_PIT();	// To trigger camera read based on integration time
	
	uart_put(NL);
	uart_put(RESET);
	uart_put(NL);
	
	for(;;) {

#if 0 // Human Friendly Output
		if (debugcamdata) {
			// Every 2 seconds
			//if (capcnt >= (2/INTEGRATION_TIME)) {
			if (capcnt >= (20)) {
				GPIOB_PCOR |= (1 << 22);
				// send the array over uart
				sprintf(str,"%i\n\r",-1); // start value
				uart_put(str);
				for (i = 0; i < 127; i++) {
					sprintf(str,"Pixel: %d\tValue(raw ADC)%i\n\r",i, line[i]);
					uart_put(str);
				}
				sprintf(str,"---------------------------------------------------"); // end value
				uart_put(str);
				capcnt = 0;
				GPIOB_PSOR |= (1 << 22);
			}
		}
#else	// MATLAB output
		if (debugcamdata) {
			// Every 2 seconds
			//if (capcnt >= (2/INTEGRATION_TIME)) {
			if (capcnt >= (20)) {
				GPIOB_PCOR |= (1 << 22);
				// send the array over uart
				sprintf(str,"%i\n\r",-1); // start value
				uart_put(str);
				for (i = 0; i < 127; i++) {
					sprintf(str,"%i\n", line[i]);
					uart_put(str);
				}
				sprintf(str,"%i\n\r",-2); // end value
				uart_put(str);
				capcnt = 0;
				GPIOB_PSOR |= (1 << 22);
			}
		}
		
#endif
		
	} //for
} //main


/* ADC0 Conversion Complete ISR  */
void ADC0_IRQHandler(void) {
	// Reading ADC0_RA clears the conversion complete flag
	ADC0VAL = ADC0_RA;
}

/* 
* FTM2 handles the camera driving logic
*	This ISR gets called once every integration period
*		by the periodic interrupt timer 0 (PIT0)
*	When it is triggered it gives the SI pulse,
*		toggles clk for 128 cycles, and stores the line
*		data from the ADC into the line variable
*/
void FTM2_IRQHandler(void){ //For FTM timer
	// Clear interrupt
	FTM2_SC &= ~FTM_SC_TOF_MASK;
	
	// Toggle clk
	if (clkval) {
		// Clock is high; set low
		GPIOB_PCOR = (1 << CLK_PIN_NUM);
		clkval = !clkval;
	}
	else {
		// Clock is low; set high
		GPIOB_PSOR = (1 << CLK_PIN_NUM);
		clkval = !clkval;
	}
	
	// Line capture logic
	if ((pixcnt >= 2) && (pixcnt < 256)) {
		if (!clkval) {	// check for falling edge
			// ADC read (note that integer division is 
			//  occurring here for indexing the array)
			
			//we polling now boiz
			ADC0_SC1A = ADC0_SC1A;
			while(!(ADC0_SC1A & ADC_SC1_COCO_MASK));
			line[pixcnt/2] = ADC0_RA;
		}
		pixcnt += 1;
	} else if (pixcnt < 2) {
		if (pixcnt == -1) {
			GPIOB_PSOR |= (1 << SI_PIN_NUM); // SI = 1
		} else if (pixcnt == 1) {
			GPIOB_PCOR |= (1 << SI_PIN_NUM); // SI = 0
			// ADC read
			ADC0_SC1A = ADC0_SC1A;
			while(!(ADC0_SC1A & ADC_SC1_COCO_MASK));
			line[0] = ADC0_RA;
		} 
		pixcnt += 1;
	} else {
		GPIOB_PCOR |= (1 << CLK_PIN_NUM); // CLK = 0
		clkval = 0; // make sure clock variable = 0
		pixcnt = -2; // reset counter
		// Disable FTM2 interrupts (until PIT0 overflows
		//   again and triggers another line capture)
		FTM2_SC &= ~FTM_SC_TOIE_MASK;
	}
	return;
}

/* PIT0 determines the integration period
*		When it overflows, it triggers the clock logic from
*		FTM2. Note the requirement to set the MOD register
* 	to reset the FTM counter because the FTM counter is 
*		always counting, I am just enabling/disabling FTM2 
*		interrupts to control when the line capture occurs
*/
void PIT0_IRQHandler(void){
	if (debugcamdata) {
		// Increment capture counter so that we can only 
		//	send line data once every ~2 seconds
		capcnt += 1;
	}
	// Clear interrupt
	PIT_TFLG0 |= PIT_TFLG_TIF_MASK;
	
	
	// Setting mod resets the FTM counter
	FTM2_CNT = FTM_CNT_COUNT(0);
	
	
	// Enable FTM2 interrupts (camera)
	FTM2_SC |= FTM_SC_TOIE_MASK;
	
	return;
}


/* Initialization of FTM2 for camera */
void init_FTM2(){
	// Enable clock
	SIM_SCGC6 |= SIM_SCGC6_FTM2_MASK;
	SIM_SCGC3 |= SIM_SCGC3_FTM2_MASK;

	// Disable Write Protection
	FTM2_MODE |= FTM_MODE_WPDIS_MASK;
	
	//divide the input clock by 1
	FTM2_SC |= FTM_SC_PS(0);
	
	// Initialize the CNT to 0 before writing to MOD
	FTM2_CNT = FTM_CNT_COUNT(0);
	
	//Set the overflow rate
	// Period of 10 us required toggling of 5 us
	FTM2->MOD = 0.000005*DEFAULT_SYSTEM_CLOCK;
	
	// No prescalar, system clock
	FTM2_SC &= ~FTM_SC_CLKS_MASK;
	FTM2_SC |= FTM_SC_CLKS(1);
	FTM2_SC &= ~FTM_SC_PS_MASK;
	FTM2_SC |= FTM_SC_PS(0);
	
	// Set up interrupt
	NVIC_EnableIRQ(FTM2_IRQn);
	FTM2_SC |= FTM_SC_TOIE_MASK;
	
	return;
}

/* Initialization of PIT timer to control 
* 		integration period
*/
void init_PIT(void){
	// Setup periodic interrupt timer (PIT)
	
	// Enable clock for timers
	SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;
	PIT_MCR &= ~PIT_MCR_MDIS_MASK;
	
	// Enable timers to continue in debug mode
	PIT_MCR &= ~PIT_MCR_FRZ_MASK; // In case you need to debug
	
	// PIT clock frequency is the system clock
	// Load the value that the timer will count down from
	PIT_LDVAL0 = PIT_LDVAL_TSV((80.0 / 1000) * DEFAULT_SYSTEM_CLOCK);
	//PIT_LDVAL0 = PIT_LDVAL_TSV(1638861);
	
	// Enable timer interrupts
	PIT_TCTRL0 |= PIT_TCTRL_TIE_MASK;
	
	// Enable the timer
	PIT_TCTRL0 |= PIT_TCTRL_TEN_MASK;

	// Clear interrupt flag
	PIT_TFLG0 |= PIT_TFLG_TIF_MASK;

	// Enable PIT interrupt in the interrupt controller
	NVIC_EnableIRQ(PIT0_IRQn);
	return;
}


/* Set up pins for GPIO
* 	PTB9 		- camera clk
*		PTB23		- camera SI
*		PTB22		- red LED
*/
void init_GPIO(void){
	// Enable LED and GPIO so we can see results
	
	// Init LEDs
	LED_Init();
	
	// Init GPIO for CLK, SI signals
	// Enable Clock
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	// Set MUX
	PORTB_PCR9 |= PORT_PCR_MUX(1);
	PORTB_PCR23 |= PORT_PCR_MUX(1);
	
	// Set GPIO for output
	GPIOB_PDDR |= (1 << CLK_PIN_NUM) | (1 << SI_PIN_NUM);
	
	return;
}

/* Set up ADC for capturing camera data */
void init_ADC0(void) {
	unsigned int calib;
	// Turn on ADC0
	//INSERT CODE HERE
	SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;
	
	// Single ended 16 bit conversion, no clock divider
	ADC0_CFG1 = ADC_CFG1_MODE(0x3);
    
	// Do ADC Calibration for Singled Ended ADC. Do not touch.
	ADC0_SC3 = ADC_SC3_CAL_MASK;
	while ( (ADC0_SC3 & ADC_SC3_CAL_MASK) != 0 );
	calib = ADC0_CLP0; calib += ADC0_CLP1; calib += ADC0_CLP2;
	calib += ADC0_CLP3; calib += ADC0_CLP4; calib += ADC0_CLPS;
	calib = calib >> 1; calib |= 0x8000;
	ADC0_PG = calib;
	
	//no Select hardware trigger.
	//software trigger
	ADC0_SC2 = 0;
	
	// Set to single ended mode	
	ADC0_SC1A = ADC_SC1_ADCH(0x0);
	
	//no Set up FTM2 trigger on ADC0
	
	// Enable NVIC interrupt
	//NVIC_EnableIRQ(ADC0_IRQn);
}