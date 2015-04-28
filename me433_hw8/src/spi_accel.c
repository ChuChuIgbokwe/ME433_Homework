#include "accel.h"
#include <xc.h>
// interface with the LSM303D accelerometer/magnetometer using spi
// Wire GND to GND, VDD to 3.3V, 
// SDO1             -> SDI (labeled SDA),
// SDI1             -> SDO
// SCK1 (B14)       -> SCL
// some digital pin -> CS

#define CS LATBbits.LATB4 // replace x with some digital pin

// send a byte via spi and return the response
unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}


// read data from the accelerometer, given the starting register address.
// return the data in data
void acc_read_register(unsigned char reg, unsigned char data[], unsigned int len) {
  unsigned int i;
  reg |= 0x80; // set the read bit (as per the accelerometer's protocol)
  if(len > 1) {
    reg |= 0x40; // set the address auto increment bit (as per the accelerometer's protocol)
  }
  CS = 0;
  spi_io(reg);
  for(i = 0; i != len; ++i) {
    data[i] = spi_io(0); // read data from spi
  }
  CS = 1;
}


void acc_write_register(unsigned char reg, unsigned char data) {
  CS = 0;               // bring CS low to activate SPI
  spi_io(reg);
  spi_io(data);
  CS = 1;               // complete the command
}


void acc_setup() {

  // turn off AN10 to be able to use SCK1 pin
//  ANSELBbits.ANSB14 = 0;     // 0 for digital, 1 for analog // set up USER pin as input. 0 for digital
  TRISBbits.TRISB4 = 0; // set CS to output and digital if necessary
  CS = 1;

  // select a pin for SDI1
  //SDI1 is a peripheral pin select so you have to choose it yourself
  ANSELAbits.ANSA1 = 0;      // 0 for digital, 1 for analog might be ANSELBbits.ANSB1 = 0;      
  SDI1Rbits.SDI1R = 0b0000;//RPA1/RA1 or pin 3 to SDO

  // select a pin for SD01
  ANSELBbits.ANSB2 = 0;      // 0 for digital, 1 for analog
  RPB2Rbits.RPB2R = 0b0011;//RPB2 or pin 6 to SDA. setting RPB2 to 0b011 sets it to the SDO1 peripheral section

  // Setup the master Master - SPI1
  // we manually control SS as a digital output 
  // since the pic is just starting, we know that spi is off. We rely on defaults here
 
  // setup spi1
  SPI1CON = 0;              // turn off the spi module and reset it
  SPI1BUF;                  // clear the rx buffer by reading from it
  SPI1BRG = 0x3;            // baud rate to 5MHz [SPI1BRG = (40000000/(2*desired))-1]
  SPI1STATbits.SPIROV = 0;  // clear the overflow bit
  SPI1CONbits.CKE = 1;      // data changes when clock goes from active to inactive
                            //    (high to low since CKP is 0)
  SPI1CONbits.MSTEN = 1;    // master operation
  SPI1CONbits.ON = 1;       // turn on spi
 
  // set the accelerometer data rate to 1600 Hz. Do not update until we read values
  acc_write_register(CTRL1, 0xAF); 

  // 50 Hz magnetometer, high resolution, temperature sensor on
  acc_write_register(CTRL5, 0xF0); 

  // enable continuous reading of the magnetometer
  acc_write_register(CTRL7, 0x0);
  
  //set accelerometer sensitivity to +- 2g
  // CTRL2 register: AFS default value: 0
  // address: 0x21 or 0100001
  // register bits: ABW1, ABW0, AFS2, AFS1, AFS0, 0, AST, SIM (default 00000000)
  // acceleration +/- 2g -> AFS2=0, AFS1=0, AFS0=0
  acc_write_register(CTRL2, 0x00);
}
