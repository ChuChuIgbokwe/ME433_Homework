#include <xc.h>
void PWM_init(){
    //Setting up Enable pins
    //RPB15 as OC1
    ANSELBbits.ANSB15 = 0;
    T2CONbits.TCKPS = 0;     // Timer3 prescaler N=2 (1:2)
	PR1 = 39999;              // period = (PR2+1) * N * 12.5 ns = 100 us, 20 kHz 9999
	TMR2 = 0;                // initial TMR2 count is 0 
	OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
	OC1CONbits.OCTSEL = 0;   // set Output compare timer select bit to use timer 2 as clock source 
	OC1RS = 2000;            // duty cycle = OC1RS/(PR2+1) = 75% 5000
	OC1R = 2000;             // initialize before turning OC1 on; afterward it is read-only 5000
	T2CONbits.ON = 1;        // turn on Timer2
  	OC1CONbits.ON = 1;       // turn on OC1}
    RPB15Rbits.RPB15R = 0b0101;// set B15 to OC1
    
    //RPB5 as OC2
	PR2 = 39999;              // period = (PR2+1) * N * 12.5 ns = 100 us, 20 kHz 9999
	OC2CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
	OC2CONbits.OCTSEL = 0;   // set Output compare timer select bit to use timer 2 as clock source 
	OC1RS = 2000;            // duty cycle = OC1RS/(PR2+1) = 75% 5000
	OC1R = 2000;             // initialize before turning OC1 on; afterward it is read-only 5000
  	OC2CONbits.ON = 1;       // turn on OC1}
    RPB5Rbits.RPB5R = 0b0101;// set B5 to OC2
    
    //Setting up Phase Pins
    //B14
    ANSELBbits.ANSB14 = 0;
    TRISBbits.TRISB14 = 0; // 0 for output, 1 for input
    LATBbits.LATB14 = 1;
    //B7
    TRISBbits.TRISB7 = 0;
    LATBbits.LATB7 = 1;
}
