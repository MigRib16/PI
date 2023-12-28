#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pigpio.h>

#include "../ABE_ADCDACPi.h"  //alterar

#define LRC 17
#define SHK 27
#define RV 2
#define numberofsamples 200000 //Aproximadamente 200 seg

void colocar_off_hook(){
    gpioSetMode(LRC, PI_OUTPUT);
	gpioSetMode(SHK, PI_INPUT);
   
   int escolha;
    while(1)
	{
	    //define state of input
	    printf("Efetuar off-hook? Sim(1), Não(0), Terminar(2)\n");
	    scanf("%d", &escolha);
	
	    if (escolha==2)
		    return (0);
		
	    gpioWrite(LRC, escolha);  // 1 = OnHook | 0 = OffHook
	
	    gpioDelay(1000000);
	    int state = gpioRead(SHK);
	
	    if(state == 1)
	    {
		    printf("Pino SHK em nivel baixo e estado on-hook\n");
	    }
	
	    else 
	    {
		    printf("Pino SHK em nivel alto e estado off-hook\n");
             break;
	    }
	
	    gpioDelay(500000);
	}
}

void clearscreen ()
{
    printf("\033[2J\033[1;1H");
}

void verificar_dial_tone(){
    
    double samplearray[numberofsamples];

	int x;
	double aux;

	while (1){
		clearscreen();

		for (x = 0; x < numberofsamples; x++)
	    {
		    samplearray[x] = read_adc_voltage(1, 0); // read from adc channel 1
	    }
        //Como detetar o dial tone?
	}
}
int main(int argc, char **argv){
	setvbuf (stdout, NULL, _IONBF, 0); // needed to print to the command line

	if (open_adc() != 1){ // open the ADC SPI channel
			printf("SPI Error");
			exit(1); // if the SPI bus fails to open exit the program
		}
    //Relé
    colocar_off_hook();
    usleep(2000000) //Pausa 2 seg
    verificar_dial_tone();
    //dial-tone
	

	(void)argc;
	(void)argv;

    gpioWrite(LRC, 0); //colocar relé a 0 na terminação

	return (0);
}