/*
 * teste_adc.c
 *
 *  Created on: 27 Dec 2023
 *
 *      compile with "gcc -o teste_RV teste_RV.c -lpigpio -lrt -lpthread"
 *      run with ".teste_RV"
 */

#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>

#define RV 2

int main(){

	if(gpioInitialise() < 1)
		{
			fprintf(stderr,"Falha ao inicializar o pigpio\n");
			return EXIT_FAILURE;
		}
		
	//define inputs outputs
	gpioSetMode(RV, PI_INPUT);
	
	while(1)
	{
	//define state of input
	
	int state = gpioRead(RV);
	
	if(state == 0)
	{
		printf("Ringing Voltage detetado\n");
        gpioDelay(7000000);
	}
	}
	gpioTerminate();
	
	return EXIT_SUCCESS;
}