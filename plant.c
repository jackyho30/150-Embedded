#include "gpiolib_addr.h"
#include "gpiolib_reg.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>


/*
Group 44
Group Members: Tianyi Zhang, Ted Liu, Jacky Ho
Date: December 3, 2018
Description: 

*/

//This function should initialize the GPIO pins
GPIO_Handle initializeGPIO()
{
	GPIO_Handle gpio;
	gpio = gpiolib_init_gpio();
	if(gpio == NULL){
		perror("Could not initialize GPIO");
	}
	return gpio;
}


//Checking the status of the moisture sensor
#define SENSOR_PIN 4
int getMoisture(GPIO_Handle gpio){
	if (gpio == NULL){
		return -1;
	}
	
	uint32_t level_reg = gpiolib_read_reg(gpio, GPLEV(0));
	return level_reg & (1 << SENSOR_PIN);
}


#define MOTOR_PIN 17
#define PIN17_BIT 21
void turnMotorOn(GPIO_Handle gpio){
	uint32_t sel_reg = gpiolib_read_reg(gpio, GPFSEL(1));
	sel_reg |= 1  << PIN17_BIT;
	gpiolib_write_reg(gpio, GPFSEL(1), sel_reg);
	gpiolib_write_reg(gpio, GPSET(0), 1 << MOTOR_PIN);
}

void turnMotorOff(GPIO_Handle gpio){
	uint32_t sel_reg = gpiolib_read_reg(gpio, GPFSEL(1));
	sel_reg |= 1  << PIN17_BIT;
	gpiolib_write_reg(gpio, GPFSEL(1), sel_reg);
	gpiolib_write_reg(gpio, GPCLR(0), 1 << MOTOR_PIN);
}



int main(void){
	enum State{START, DRY, WET, DONE};
	enum State s = START;
	
	GPIO_Handle gpio = initializeGPIO();
	
	/*
	while(1){	
		moisture = level_reg & (1 << 4);
		printf("%d",moisture);
	}
	*/
	
	while(s != DONE){
		int isDry = getMoisture(gpio);
		switch(s){
			case START:
				s = WET;
				break;

			case DRY:
				turnMotorOn(gpio);
				
				if (!isDry){
					s = WET;
				}			
				break;
				
			case WET:
				turnMotorOff(gpio);
				
				if (isDry){
					s = DRY;
				}			
				break;
				
			case DONE:
				break;
				
			default:
				break;
		}
		
		//delay by 1 second
		usleep(1000000);
	}
	return 0;
}
