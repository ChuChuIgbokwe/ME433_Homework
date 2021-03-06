

#include<xc.h> // processor SFR definitions
#include<sys/attribs.h> // __ISR macro
#include"accel.h"

// DEVCFGs here

// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // not boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // free up secondary osc pins by turning sosc off
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // slowest wdt
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 40MHz
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_20 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL
#pragma config UPLLIDIV = DIV_2 // divide 8MHz input clock, then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0x1234 // some 16 bit userid
#pragma config PMDL1WAY = ON // not multiple reconfiguration, check this
#pragma config IOL1WAY = ON // not multiple reconfiguration, check this
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // controlled by USB module

int readADC(void);

int main() {

    // startup

	__builtin_disable_interrupts();

	// set the CP0 CONFIG register to indicate that
	// kseg0 is cacheable (0x3) or uncacheable (0x2)
	// see Chapter 2 "CPU for Devices with M4K Core"
	// of the PIC32 reference manual
	__builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

	// no cache on this chip!

	// 0 data RAM access wait states
	BMXCONbits.BMXWSDRM = 0x0;

	// enable multi vector interrupts
	INTCONbits.MVEC = 0x1;

	// disable JTAG to be able to use TDI, TDO, TCK, TMS as digital
	DDPCONbits.JTAGEN = 0;

	__builtin_enable_interrupts();

	ANSELAbits.ANSA0 = 0;

    // set up USER pin as input
	ANSELBbits.ANSB13 = 0;
	TRISBbits.TRISB13 = 1;

    // set up LED1 pin as a digital output
	RPB7Rbits.RPB7R = 0b0001;
    TRISBbits.TRISB7 = 0;
	LATBbits.LATB7 = 1;

    // set up LED2 as OC1 using Timer2 at 1kHz
    ANSELBbits.ANSB15 = 0; // 0 for digital
    RPB15Rbits.RPB15R = 0b0101; //set B15 as output compare 1

//    __builtin_disable_interrupts();
	T2CONbits.TCKPS = 0;     // Timer2 prescaler N=1 (1:4)
	PR2 = 39999;             // period = (PR2+1) * N * 12.5 ns = 1 ms, 1 kHz
	TMR2 = 0;                // initial TMR2 count is 0

 	OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
	T2CONbits.ON = 1;        // turn on Timer2
	OC1CONbits.ON = 1;       // turn on OC1

        OC1RS = 0;
        OC1R = 0;

    // set up A0 as AN0
        ANSELAbits.ANSA0 = 1;
        AD1CON3bits.ADCS = 3;
        AD1CHSbits.CH0SA = 0;
        AD1CON1bits.ADON = 1;

//        // Turn on I2C_1 and initialize display
//        I2C1CONbits.ON = 1;
        //New part for printing message to screen
        display_init();             //initialize display
        display_clear();            //clear screen
        char message[20];
        char message1[20];
//        sprintf(message,"batman");
//        display_ascii(message,10,10);
        
        acc_setup();
        // initialize variables to store the acceleration, magnetometer, and temperature data
        short accels[3]; // accelerations for the 3 axes
        short mags[3]; // magnetometer readings for the 3 axes
        short temp;
        
	while (1) {
           
            // invert pin every 0.5s, set PWM duty cycle % to the pot voltage output
            //Use the core timer to double check your CPU clock settings
            _CP0_SET_COUNT(0); // set core timer to 0, remember it counts at half the CPU clock
            LATBINV = 0x0080; // invert a pin

            // wait for half a second, setting LED brightness to pot angle while waiting


            while (_CP0_GET_COUNT() < 10000000) {
            
             
                int val = readADC();
                OC1RS = (val * PR2)/1024;

                if (PORTBbits.RB13 == 1) {
                    // nothing
                }
                else {
                    LATBINV = 0x0080;
                }
                     //To read the values from the chip, call:
            // read the accelerometer from all three axes
            // the accelerometer and the pic32 are both little endian by default (the lowest address has the LSB)
            // the accelerations are 16-bit twos compliment numbers, the same as a short
            acc_read_register(OUT_X_L_A, (unsigned char *) accels, 6);
//            sprintf(message1, "accel x: %d", accels[0]);     //~ takes the two's compliment
//            display_ascii(message1, 10, 10);
//            display_clear();
            
            // need to read all 6 bytes in one transaction to get an update.
            acc_read_register(OUT_X_L_M, (unsigned char *) mags, 6);

            // read the temperature data. Its a right justified 12 bit two's compliment number
            acc_read_register(TEMP_OUT_L, (unsigned char *) &temp, 2);
            
            display_clear();            //clear screen
            
            sprintf(message,"y : %d",accels[1]);
            display_ascii(message, 5, 5);      //writes message to screen
            
            
//             display_arrow(accels[0], accels[1]);        // display_arrow will draw line on screen
//             display_clear();
//            accel_draw_axis(accels[0], accels[1]);
            
            }
        }
}

int readADC(void) {
    int count = 0;
    int end = 0;
    int samp_time = 20;
    int a = 0;

    AD1CON1bits.SAMP = 1;
    count = _CP0_GET_COUNT();
    end = count + samp_time;
    while (_CP0_GET_COUNT() < end) {
    }
    AD1CON1bits.SAMP = 0;
    while (!AD1CON1bits.DONE) {
    }
    a = ADC1BUF0;
    return a;
}

