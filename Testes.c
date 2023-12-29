#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pigpio.h>
#include <complex.h>
#include <fftw3.h>
//gcc ../../ABElectronics_C_Libraries/ADCDACPi/ABE_ADCDACPi.c Testes.c -o Testes -lpigpio -lrt -lpthread -lfftw3 -lfftw3f -lm
#include "../../ABElectronics_C_Libraries/ADCDACPi/ABE_ADCDACPi.h"  //alterar

#define LRC 17
#define LRD 18
#define RV 2
#define numberofsamples 200000 //Aproximadamente 200 seg

void performFFT(double *input, double *output, int size) {
    fftw_plan plan = fftw_plan_r2r_1d(size, input, output, FFTW_R2HC, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
}

void colocar_off_hook(){
    gpioSetMode(LRC, PI_OUTPUT);
	gpioSetMode(LRD, PI_INPUT);
   
   int escolha;
    while(1)
	{
	    //define state of input
	    printf("Efetuar off-hook? Sim(1), Não(0), Terminar(2)\n");
	    scanf("%d", &escolha);
	
	    if (escolha==2)
		    return EXIT_SUCCESS;
		
	    gpioWrite(LRC, escolha);  // 1 = OnHook | 0 = OffHook
	
	    gpioDelay(1000000);
	    int state = gpioRead(LRD);
	
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

void verificar_RV(){
    gpioSetMode(RV, PI_INPUT);
    int state = gpioRead(RV);
	
    while(1){
	    if(state == 0)
	    {
		    printf("Ringing Voltage detetado\n");
            gpioDelay(7000000);
            break;
	    }
    }
}

void verificar_dial_tone(){
    
    double samplearray[numberofsamples];

	int x;
	double aux;
    double output[numberofsamples];
	while (1){
		clearscreen();

		for (x = 0; x < numberofsamples; x++)
	    {
		    samplearray[x] = read_adc_voltage(1, 0); // read from adc channel 1
	    }
        //Como detetar o dial tone?
        performFFT(samplearray, output, numberofsamples);
        for (int i = 0; i < numberofsamples / 2 + 1; i++) {
            double magnitude = output[i];
            // Frequência correspondente ao índice i: frequencia = i * (taxa de amostragem / numSamples)
            double frequency = i * (double)numberofsamples / (double)numberofsamples;
            printf("Frequencia: %.2f Hz, Magnitude: %.4f\n", frequency, magnitude);
        }
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
    usleep(2000000); //Pausa 2 seg
    //verificar_dial_tone();
    usleep(10000000); //Pausa 10 seg para marcar o tom
    verificar_RV();
	

	(void)argc;
	(void)argv;

    //gpioWrite(LRC, 0); //colocar relé a 0 na terminação
    gpioTerminate();
    
	return (0);
}