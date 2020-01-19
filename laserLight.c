#include "gpiolib_addr.h"
#include "gpiolib_reg.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

//This function will initialize the GPIO pins and handle any error checking
//for the initialization
GPIO_Handle initializeGPIO()
{
	//This is the same initialization that was done in Lab 2
	GPIO_Handle gpio;
	gpio = gpiolib_init_gpio();
	if(gpio == NULL)
	{
		perror("Could not initialize GPIO");
	}
	return gpio;
}

//This function will change the appropriate pins value in the select register
//so that the pin can function as an output
void setToOutput(GPIO_Handle gpio, int pinNumber)
{
	//Check that the gpio is functional
	if(gpio == NULL)
	{
		printf("The GPIO has not been intitialized properly \n");
		return;
	}

	//Check that we are trying to set a valid pin number
	if(pinNumber < 2 || pinNumber > 27)
	{
		printf("Not a valid pinNumer \n");
		return;
	}

	//This will create a variable that has the appropriate select
	//register number. For more information about the registers
	//look up BCM 2835.
	int registerNum = pinNumber / 10;

	//This will create a variable that is the appropriate amount that 
	//the 1 will need to be shifted by to set the pin to be an output
	int bitShift = (pinNumber % 10) * 3;

	//This is the same code that was used in Lab 2, except that it uses
	//variables for the register number and the bit shift
	uint32_t sel_reg = gpiolib_read_reg(gpio, GPFSEL(registerNum));
	sel_reg |= 1  << bitShift;
	gpiolib_write_reg(gpio, GPFSEL(1), sel_reg);
}

//This function gets the current state of the laser in the same way
//that we got it in Lab 2 however it takes the diode number as a parameter
//instead of a pin number.
//For your version of this function, you will need to add a case when diode
//number is equal to 2 and check the appropriate pin of your choosing.

#define LASER1_PIN_NUM 4 //This will replace LASER1_PIN_NUM with 4 when compiled
int laserDiodeStatus(GPIO_Handle gpio, int diodeNumber)
{
	if(gpio == NULL)
	{
		return -1;
	}

	if(diodeNumber == 1)
	{
		uint32_t level_reg = gpiolib_read_reg(gpio, GPLEV(0));

		if(level_reg & (1 << LASER1_PIN_NUM))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return -1;
	}
}

//This function will make an output pin output 3.3V. It is the same
//as what was done in Lab 2 to make the pin output 3.3V
void outputOn(GPIO_Handle gpio, int pinNumber)
{
	gpiolib_write_reg(gpio, GPSET(0), 1 << pinNumber);
}

//This function will make an output pin turn off. It is the same
//as what was done in Lab 2 to make the pin turn off
void outputOff(GPIO_Handle gpio, int pinNumber)
{
	gpiolib_write_reg(gpio, GPCLR(0), 1 << pinNumber);
}

int main(const int argc, const char* const argv[])
{
	//We want to accept a command line argument that will be the number
	//of seconds that the program will run for, so we need to ensure that
	//the user actually gives us a time to run the program for
	if(argc < 2)
	{
		printf("Error, no time given: exitting\n");
		return -1;
	}
	
	//Initialize the GPIO pins
	GPIO_Handle gpio = initializeGPIO();

	//Set pin 17 as an output. 
	//setToOutput(gpio, 17);

	//Create a variable that represents the initial start time
	//time_t is a special variable type used by the time library for calendar times
	//the time() function will return the current calendar time. You will need to put
	//NULL as the argument for the time() function
	time_t startTime = time(NULL);

	//This variable will represent the amount of time the program should run for
	int timeLimit = atoi(argv[1]);

	
	//In the while condition, we check the current time minus the start time and
	//see if it is less than the number of seconds that was given from the 
	//command line.

	while((time(NULL) - startTime) < timeLimit)
	{
		//The code in this while loop is identical to the code from buttonBlink
		//in lab 2. You can think of the laser/photodiode setup as a type of switch

		int pin_state = laserDiodeStatus(gpio, 1);

		if(!pin_state) //if pin_state is FALSE
		{	
			//Turn the LED on
			//outputOn(gpio, 17);
		}
		if(pin_state) //if pin_state is TRUE
		{
			//Turn the LED off
			//outputOff(gpio, 17);
		}
		printf("%d\n", pin_state);
		usleep(500000);
	}


	//Turn off the LED before the program ends
	//outputOff(gpio, 17);

	//Free the GPIO now that the program is over.
	gpiolib_free_gpio(gpio);
	return 0;
}

