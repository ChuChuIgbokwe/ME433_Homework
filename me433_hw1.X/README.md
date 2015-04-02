Design a circuit to:

* Get 5V and ground from the USB breakout board
* Regulate the 5V to 3.3V using the LD1117, power an LED with the 3.3V, and check for noise with the nScope
* Provide power to the PIC32MX250, and add bypass capacitors to the power and Vcap pins (Vcap should get 10uF!)
* Add a button and pull-up resistor to MCLR
* Add a button and pull-up resistor to some digital pin (call this the USER button)
* Add an LED to ground on a digital pin (call this LED1)
* Add an LED to ground on an OC1-capable pin (call this LED2)
* Add a potentiometer and read the output voltage on some analog input pin
I recommend A0 for AN0, B7 for LED1, B13 for USER, and B15 for OC1.

Write code to:

* Set the Primary Oscillator to 40MHz using PLL
* Set LED1 to digital output
* Set LED2 to output compare at 1kHz
* Set USER to digital input
* Read the analog input

Demonstrate:

* Toggle LED1 every 1/2 second
* When USER is pushed, toggle LED1 as fast as possible
* Set LED2 brightness proportional to the potentiometer voltage
* Set LED2 frequency to 1kHz using Timer 2 and verify with the nScope or Tek scope
