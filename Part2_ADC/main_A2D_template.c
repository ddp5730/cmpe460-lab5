/*
* Rochester Institute of Technology
* Department of Computer Engineering
* CMPE 460  Interfacing Digital Electronics
* Spring 2016
*
* Filename: main_A2D_template.c
*/
 
#include "MK64F12.h"
#include "uart.h"
#include "stdio.h"

 
void PDB_INIT(void) {
    //Enable PDB Clock
    SIM_SCGC6 |= SIM_SCGC6_PDB_MASK;
    //PDB0_CNT = 0x0000;
    PDB0_MOD = 50000; // 50,000,000 / 50,000 = 1000

    PDB0_SC = PDB_SC_PDBEN_MASK | PDB_SC_CONT_MASK | PDB_SC_TRGSEL(0xf)
                                    | PDB_SC_LDOK_MASK;
    PDB0_CH1C1 = PDB_C1_EN(0x01) | PDB_C1_TOS(0x01);
}
 
void ADC1_INIT(void) {
    unsigned int calib;
 
    // Turn on ADC1
    SIM_SCGC3 |= SIM_SCGC3_ADC1_MASK;
	
		NVIC_EnableIRQ(ADC1_IRQn);
 
    // Configure CFG Registers
    // Configure ADC to divide 50 MHz down to 6.25 MHz AD Clock, 16-bit single ended
    //(Insert your code here.)
		ADC1_CFG1 = ADC_CFG1_ADIV(0x3) | ADC_CFG1_MODE(0x3);
 
    // Do ADC Calibration for Singled Ended ADC. Do not touch.
    ADC1_SC3 = ADC_SC3_CAL_MASK;
    while ( (ADC1_SC3 & ADC_SC3_CAL_MASK) != 0 );

    calib = ADC1_CLP0;
    calib += ADC1_CLP1;
    calib += ADC1_CLP2;
    calib += ADC1_CLP3;
    calib += ADC1_CLP4;
    calib += ADC1_CLPS;
    calib = calib >> 1;
    calib |= 0x8000;
    ADC1_PG = calib;
 
    // Configure SC registers.
    // Select hardware trigger.
    //(Insert your code here.)
		ADC1_SC2 = ADC_SC2_ADTRG_MASK;
 
    // Configure SC1A register.
    // Select ADC Channel and enable interrupts. Use ADC1 channel DADP3  in single ended mode.
    //(Insert your code here.)
		
		//ADC1_SC1A = ADC_SC1_ADCH(0x03) | ADC_SC1_AIEN_MASK;	 //lab manual wants DADP0, but comments want DADP3??
		ADC1_SC1A = ADC_SC1_ADCH(0x00) | ADC_SC1_AIEN_MASK;
 
    // Enable NVIC interrupt
    //(Insert your code here.)
		NVIC_EnableIRQ(ADC1_IRQn);
}
 
// ADC1 Conversion Complete ISR
void ADC1_IRQHandler(void) {
    // Read the result (upper 12-bits). This also clears the Conversion complete flag.
    unsigned short i = ADC1_RA >> 4;

    //Set DAC output value (12bit)
    //(Insert your code here.)
		DAC0_DAT0H = i >> 8;
		DAC0_DAT0L = i;
}

void DAC0_INIT(void) {
    //enable DAC clock
    SIM_SCGC2 |= SIM_SCGC2_DAC0_MASK;
    DAC0_C0 = DAC_C0_DACEN_MASK | DAC_C0_DACRFS_MASK;
    DAC0_C1 = 0;
}

int main(void) {
    int i; char str[100];
   
    // Enable UART Pins
    //(Insert your code here.)
   
    // Initialize UART
    //(Insert your code here.)
		uart_init();
               
    DAC0_INIT();
    ADC1_INIT();
    PDB_INIT();
 
    // Start the PDB (ADC Conversions)
    PDB0_SC |= PDB_SC_SWTRIG_MASK;
 
    for(;;) {
#if 1
		sprintf(str,"\n Decimal: %d Hexadecimal: %x \n\r",ADC1_RA,ADC1_RA);
#else
		float raw = ADC1_RA;
		float c = ((raw * (3.3/0xffff)) - 0.5) / 0.01;
		float f = c * 9/5 + 32;
		sprintf(str,"\n C: %f F: %f\n\r",c,f);	
#endif
		uart_put(str);
		for( i=0; i < 5000000; ++i ){
                       
        }
    }
 
    return 0;
}
