#include "gpiolib_addr.h"
#include "gpiolib_reg.h"
#include "gpiolib_reg.c"

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

int main(void){
	//The following code will initialize the GPIO pins
	GPIO_Handle gpio;
	gpio = gpiolib_init_gpio();
	if(gpio == NULL){
		perror("Could not initialize GPIO");
	}

	int buttonCount = 0;

	uint32_t sel_reg = gpiolib_read_reg(gpio, GPFSEL(1));
	sel_reg |= 1  << 21;
	gpiolib_write_reg(gpio, GPFSEL(1), sel_reg);
	//Turn on the LED by changing the 17th bit in the set register to 1
	gpiolib_write_reg(gpio, GPSET(0), 1 << 17);
	
	
	while(1){
		uint32_t level_reg = gpiolib_read_reg(gpio, GPLEV(0));
		int pin_state = level_reg & (1 << 4);
		
		if(pin_state) {
			//Turn the first LED off
			gpiolib_write_reg(gpio, GPCLR(0), 1 << 17);
			pin_state = 0;
			
			for (int i = 0; i<1000; i++){
				if (pin_state){
					buttonCount += 1;
					pin_state = 0;
				}
				usleep(10000);
			}
			int counter = 0;
			while(counter < buttonCount){
				//Turn on the LED2 by changing the 12th bit in the set register to 1
				gpiolib_write_reg(gpio, GPSET(0), 1 << 12);

				//Pause the program for 1000000 microseconds, i.e. 1 second
				usleep(1000000);

				//Turn the LED2 off by setting the 12th bit in the clear register to 1
				gpiolib_write_reg(gpio, GPCLR(0), 1 << 12);

				//Pause the program for 1000000 microseconds, i.e. 1 second
				usleep(1000000);

				counter ++;
			}
			usleep(5000000);		//pause for 5 seconds
			//Turn on the LED1 by changing the 17th bit in the set register to 1
			gpiolib_write_reg(gpio, GPSET(0), 1 << 17);
			
		}
	}
}









