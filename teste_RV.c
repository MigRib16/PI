/*
 * teste_adc.c
 *
 *  Created on: 27 Dec 2023
 *
 *      compile with "gcc -o teste_RV teste_RV.c -lpigpio -lrt -lpthread"
 *      run with "sudo ./teste_RV"
 */

#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
#include <unistd.h>
#include <sys/time.h>

#define RV 5
#define LRC 17
#define LRD 18

void colocar_off_hook() {
        int escolha;

        gpioSetMode(LRC, PI_OUTPUT);
        gpioSetMode(LRD, PI_INPUT);


        gpioWrite(LRC, 0);
        gpioDelay(1000000);
        int state = gpioRead(LRD);

        if(state == 0)
        {
                gpioDelay(1000000);
                gpioWrite(LRC, 1);
                printf("Pino SHK em nivel alto e estado off-hook\n");
        }

        else
        {
                printf("Pino SHK em nivel baixo e estado on-hook\n");
                gpioDelay(1000000);
                gpioWrite(LRC, 1);
                printf("Pino SHK em nivel alto e estado off-hook\n");
        }

        gpioDelay(500000);
}

int main(){

	if(gpioInitialise() < 1)
		{
			fprintf(stderr,"Falha ao inicializar o pigpio\n");
			return EXIT_FAILURE;
		}

	//define inputs outputs
        gpioSetMode(RV, PI_INPUT);
        printf("Á espera de pedido de Chamada...\n");  

        struct timeval t1, t2;
        // start timer
        gettimeofday(&t1, NULL);

	while(1)
	{

	//define state of input
	int state = gpioRead(RV); 
	
	if(state == 0) {
		printf("Ringing Voltage detetado\n");
        	gpioDelay(500000);
                colocar_off_hook(); }
             
        // stop timer
	gettimeofday(&t2, NULL);
        if (t2.tv_sec - t1.tv_sec > 30) {
                printf("Não foi recebido nenhum pedido de chamada!\n");  
                break; } 

        }

	gpioTerminate();
	
	return EXIT_SUCCESS;
}
