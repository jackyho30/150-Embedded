#include "gpiolib_addr.h"
#include "gpiolib_reg.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

//HARDWARE DEPENDENT CODE BELOW
#ifndef MARMOSET_TESTING

/* You may want to create helper functions for the Hardware Dependent functions*/

//This function should initialize the GPIO pins
GPIO_Handle initializeGPIO()
{
	GPIO_Handle gpio;
	gpio = gpiolib_init_gpio();
	if(gpio == NULL)
	{
		perror("Could not initialize GPIO");
	}
	return gpio;
	
}


//This function should accept the diode number (1 or 2) and output
//a 0 if the laser beam is not reaching the diode, a 1 if the laser
//beam is reaching the diode or -1 if an error occurs.
#define LASER1_PIN_NUM 4
#define LASER2_PIN_NUM 17
int laserDiodeStatus(GPIO_Handle gpio, int diodeNumber)
{
	if(gpio == NULL){
		return -1;
	}

	if(diodeNumber == 1){
		uint32_t level_reg = gpiolib_read_reg(gpio, GPLEV(0));

		if(level_reg & (1 << LASER1_PIN_NUM)){
			return 1;
		}
		else{
			return 0;
		}
	}
	
	else if(diodeNumber == 2){
		uint32_t level_reg = gpiolib_read_reg(gpio, GPLEV(0));
		
		if(level_reg & (1 << LASER2_PIN_NUM)){
			return 1;
		}
		else{
			return 0;
		}
	}
	else
	{
		return -1;
	}
}

#endif
//END OF HARDWARE DEPENDENT CODE

//This function will output the number of times each laser was broken
//and it will output how many objects have moved into and out of the room.

//laser1Count will be how many times laser 1 is broken (the left laser).
//laser2Count will be how many times laser 2 is broken (the right laser).
//numberIn will be the number  of objects that moved into the room.
//numberOut will be the number of objects that moved out of the room.
void outputMessage(int laser1Count, int laser2Count, int numberIn, int numberOut)
{
	printf("Laser 1 was broken %d times \n", laser1Count);
	printf("Laser 2 was broken %d times \n", laser2Count);
	printf("%d objects entered the room \n", numberIn);
	printf("%d objects exitted the room \n", numberOut);
}

//This function accepts an errorCode. You can define what the corresponding error code
//will be for each type of error that may occur.
void errorMessage(int errorCode)
{
	fprintf(stderr, "An error occured; the error code was %d \n", errorCode);
}


#ifndef MARMOSET_TESTING

int main(const int argc, const char* const argv[]){
	//error if no time is given
	if(argc < 2)
	{
		printf("Error, no time given: exitting\n");
		return -1;
	}
	//Initialize the GPIO pins
	GPIO_Handle gpio = initializeGPIO();
	//variables that we're keeping track of
	int laserLCount = 0;
	int laserRCount = 0;
	int numberIn = 0;
	int numberOut = 0;
	//timeLimit from input
	int timeLimit = atoi(argv[1]);
	//start time from the beginning of the program
	time_t startTime = time(NULL); 
	
	//in is left then right
	//out is right then left
	enum State{START, NONE_BROKEN, IN1, IN2, IN3, OUT1, OUT2, OUT3, DONE};
	enum State s = START;
	while (s != DONE){
		int laser1state = laserDiodeStatus(gpio, 1);
		int laser2state = laserDiodeStatus(gpio, 2);
		switch(s){
			case START:
				if ((time(NULL) - startTime) < timeLimit){
					s = NONE_BROKEN;	
				}
				else{
					s = DONE;
				}
				break;
				
			case NONE_BROKEN:
				//printf("%d", laserLCount);
				if ((time(NULL) - startTime) < timeLimit){
					if (!laser1state){
						s = IN1;
						laserLCount++;
					}
					else if (!laser2state){
						s = OUT1;
						laserRCount++;
					}
				}
				else{
					s = DONE;
				}
				break;
				
			case IN1:
				if ((time(NULL) - startTime) < timeLimit){
					if (!laser2state){
						s = IN2;
						laserRCount++;
					}
					else if (laser1state){
						s = NONE_BROKEN;
					}
				}
				else{
					s = DONE;
				}
				break;
			
			case IN2:
				if ((time(NULL) - startTime) < timeLimit){
					if (laser1state){
						s = IN3;
					}
					else if (laser2state){
						s = IN1;
					}
				}
				else{
					s = DONE;
				}
				break;
				
			case IN3:
				if ((time(NULL) - startTime) < timeLimit){
					if (laser2state){
						s = NONE_BROKEN;
						numberIn++;
					}
					else if (!laser1state){
						s = IN2;
						laserLCount++;
					}
				}
				else{
					s = DONE;
				}
				break;
							
			case OUT1:
				if ((time(NULL) - startTime) < timeLimit){
					if (!laser1state){
						s = OUT2;	
						laserLCount++;
					}
					else if (laser2state){
						s = NONE_BROKEN;
					}
				}
				else{
					s = DONE;
				} 
				break;
				
			case OUT2:
				if ((time(NULL) - startTime) < timeLimit){
					if (laser2state){
						s = OUT3;
					}
					else if (laser1state){
						s = OUT1;
					}
				}
				else{
					s = DONE;
				}
				break;
				
			case OUT3:
				if ((time(NULL) - startTime) < timeLimit){
					if (laser1state){
						s = NONE_BROKEN;
						numberOut++;
					}
					else if(!laser2state){
						s = OUT2;
						laserRCount++;
					}
				}
				else{
					s = DONE;
				}
				break;
				
			case DONE:
				break; 
				
			default:
				break;
		}
		
		usleep(10000);	
	}
	
	
	outputMessage(laserLCount, laserRCount, numberIn, numberOut);
	gpiolib_free_gpio(gpio);
	return 0;
}

#endif
