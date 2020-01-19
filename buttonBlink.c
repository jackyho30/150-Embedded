#include "gpiolib_addr.h"
#include "gpiolib_reg.h"

#include <stdlib.h>
#include <stdio.h>

int main(void)
{
	//Initialize the GPIO pins and check if initialized correctly
	GPIO_Handle gpio;
	gpio = gpiolib_init_gpio();
	if(gpio == NULL)
	{
		perror("Could not initialize GPIO");
	}

	//Set pin 17 as an output pin, same as what was done in the blink program
	uint32_t sel_reg = gpiolib_read_reg(gpio, GPFSEL(1));
	sel_reg |= 1  << 21;
	gpiolib_write_reg(gpio, GPFSEL(1), sel_reg);


	while(1)
	{
		/*
		  Declare a variable called level_reg of 32-bit size and 
		  assigned the value of the level register
		*/ 	
		uint32_t level_reg = gpiolib_read_reg(gpio, GPLEV(0));

		/*
		  Declare a variable called pin_state and assign the value of bit 4
		  from the level_reg
	    	*/ 
		int pin_state = level_reg & (1 << 4);

		//if pin_state is TRUE
		if(pin_state) 
		{
			//Turn the LED off
			gpiolib_write_reg(gpio, GPCLR(0), 1 << 17);
		}
		else //otherwise
		{
			//Turn the LED on
			gpiolib_write_reg(gpio, GPSET(0), 1 << 17);
		}
		
	}
}
