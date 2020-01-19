#include "gpiolib_addr.h"
#include "gpiolib_reg.h"

#include <stdlib.h>
#include <stdio.h>

/*
NOTE TO STUDENT:

The Application Program Interface (API) for writing and reading the register 
from the Raspberry Pi is summarized below:

	GPIO_Handle gpiolib_init_gpio(void);
	void gpiolib_free_gpio(GPIO_Handle handle);

	void gpiolib_write_reg(GPIO_Handle handle, uint32_t offst, uint32_t data);
	uint32_t gpiolib_read_reg(GPIO_Handle handle, uint32_t offst);

The code below uses the API above. More information on these functions 
can be found in the GPIO reference documentation.
*/

int main(void) //The void in brackets specifies that the main function is not accepting any inputs
{
	//The following code will initialize the GPIO pins
	GPIO_Handle gpio;
	gpio = gpiolib_init_gpio();
	if(gpio == NULL)
	{
		perror("Could not initialize GPIO");
	}

	/********            How to set pin 17 as an output.      *********/

	/*
	  We want to set pin 17 as an output. To do this we must change the bits
	  that correspond to pin 17. The bits for pin 17 are located in 
	  Select register 1. The bits that correspond to pin 17 are 23-21. 
	  By changing bit 21 from 0 to 1, we will have set pin 17 to output.

	  uint32_t sel_ref = gpiolib_read_reg(gpio, GPFSEL(1)), 
	  this line is creating a variable that contains exactly 32 bits 
	  named sel_reg. The 32 bits are taken from whatever the 32 bits of 
	  select register 1 are. The GPFSEL(1) corresponds to select register 1.
	*/
	uint32_t sel_reg = gpiolib_read_reg(gpio, GPFSEL(1));
	
	/*
	  The line "sel_reg |= 1" below is using a bitwise OR operation to change 
	  the 21st bit in sel_reg to 1. It will not change the values of 
	  any other bit in sel_reg.
	*/
	sel_reg |= 1  << 21;

	/*
	  The line "gpiolib_write_reg(gpio, GPFSEL(1), sel_reg)" below is writing 
	  sel_reg into select register 1. This means that select register 1 will be 
	  unchanged except that bit 21 will now be a 1.
	*/
	gpiolib_write_reg(gpio, GPFSEL(1), sel_reg);

	/*
	  This variable is used as a counter for the number of times 
	  the LED will blink
	*/
	int counter = 0; 


	//Loop to blink the LED 5 times
	while(counter < 5)
	{
		//Turn on the LED by changing the 17th bit in the set register to 1
		gpiolib_write_reg(gpio, GPSET(0), 1 << 17);

		//Pause the program for 1000000 microseconds, i.e. 1 second
		usleep(1000000);

		//Turn the LED off by setting the 17th bit in the clear register to 1
		gpiolib_write_reg(gpio, GPCLR(0), 1 << 17);

		//Pause the program for 1000000 microseconds, i.e. 1 second
		usleep(1000000);

		counter ++;
	}

	//Clean up the GPIO pins before the program finishes
	gpiolib_free_gpio(gpio);

}
