#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "../ABE_ADCDACPi.h"  //alterar

#define numberofsamples 100000

void clearscreen ()
{
    printf("\033[2J\033[1;1H");
}

int main(int argc, char **argv){
	setvbuf (stdout, NULL, _IONBF, 0); // needed to print to the command line

	if (open_adc() != 1){ // open the ADC SPI channel
			printf("SPI Error");
			exit(1); // if the SPI bus fails to open exit the program
		}

	double samplearray[numberofsamples];

	int x;
	double aux;

	while (1){
		clearscreen();
		aux = read_adc_voltage(1, 0);
		if(aux)
	
		usleep(200000); // sleep 0.2 seconds

	}

	(void)argc;
	(void)argv;
	return (0);
}