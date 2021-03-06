#include "gpiolib_addr.h"
#include "gpiolib_reg.h"

#include <stdlib.h>
#include <stdio.h>

int main(void){
	//Initialize the GPIO pins and check if initialized correctly
	GPIO_Handle gpio;
	gpio = gpiolib_init_gpio();
	if(gpio == NULL){
		perror("Could not initialize GPIO");
	}

	//Set pin 17 as an output pin
	uint32_t sel_reg = gpiolib_read_reg(gpio, GPFSEL(1));
	sel_reg |= 1  << 21;
	gpiolib_write_reg(gpio, GPFSEL(1), sel_reg);

	//Set pin 12 as an output pin
	uint32_t sel_reg2 = gpiolib_read_reg(gpio, GPFSEL(1));
	sel_reg2 |= 1  << 12;
	gpiolib_write_reg(gpio, GPFSEL(1), sel_reg2);
	

	while(1){
		int buttonCount = 0;

		//Turn on the LED by changing the 17th bit in the set register to 1
		gpiolib_write_reg(gpio, GPSET(0), 1 << 17);

		/*
		  Declare a variable called level_reg of 32-bit size and 
		  assigned the value of the level register
		*/ 	
		uint32_t level_reg = gpiolib_read_reg(gpio, GPLEV(0));

		/*
		  Declare a variable called pin_state and assign the value of bit 4
		  from the level_reg
	    	*/ 
		uint32_t  pin_state = level_reg & (1 << 4);

		//if pin_state is FALSE
		if(!pin_state) {
			//Turn the LED off
			gpiolib_write_reg(gpio, GPCLR(0), 1 << 17);
			
			int prev_state = 1; //1 for up, 0 for down
			for (int i = 0; i<1000; i++){

				//for reading the state of the button
				level_reg = gpiolib_read_reg(gpio, GPLEV(0));
				pin_state = level_reg & (1 << 4);

				// if the button is down
				if (!pin_state){
					//increment buttonCount if the button was previously up on last loop
					if (prev_state == 1){
						buttonCount += 1;
					}
					//do not increment if button was already registered as down in last loop
					//set previous state to down
					prev_state = 0;
				}
				//set previous state of button to up
				if (pin_state){
					prev_state = 1;
				}

				usleep(10000); // 10 miliseconds looped 1000 times adds up to 10 seconds
			}
			
			//set counter for number of times led blinks and stop when it reaches number of times button is pressed
			int counter = 1;
			while(counter < buttonCount){
			
				//Turn on the LED2 by changing the 12th bit in the set register to 1
				gpiolib_write_reg(gpio, GPSET(0), 1 << 14);

				//Pause the program for 1000000 microseconds, i.e. 1 second
				usleep(1000000);

				//Turn the LED2 off by setting the 12th bit in the clear register to 1
				gpiolib_write_reg(gpio, GPCLR(0), 1 << 14);

				//Pause the program for 1000000 microseconds, i.e. 1 second
				usleep(1000000);

				counter ++;
			}
			usleep(4000000);	//pause for 5 seconds (4 here plus the one second at the end of the above loop)
			
			//Turn on the LED1 by changing the 17th bit in the set register to 1
			gpiolib_write_reg(gpio, GPSET(0), 1 << 17);
		}
		
	}
	//use ctrl+c to exit as program is supposed to continuously repeat
}
