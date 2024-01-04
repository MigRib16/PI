#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pigpio.h>
#include <complex.h>
#include <fftw3.h>
#include "../../ABElectronics_C_Libraries/ADCDACPi/ABE_ADCDACPi.h"  
//gcc ../../ABElectronics_C_Libraries/ADCDACPi/ABE_ADCDACPi.c Testes.c -o Testes -lpigpio -lrt -lpthread -lfftw3 -lfftw3f -lm

#define LRC 17
#define LRD 18
#define RV 5
#define NUM_SAMPLES 100000 //Aproximadamente 10 seg

void performFFT(double *input, double complex *output, int size) {
    fftw_plan plan = fftw_plan_r2r_1d(size, input, (fftw_complex *)output, FFTW_R2HC, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
}


void colocar_off_hook() {
	int escolha;

	gpioWrite(LRC, 0);
	gpioDelay(1000000);
	int state = gpioRead(LRD);
	
	if(state == 1)
	{
		gpioDelay(1000000);
		gpioWrite(LRC, 1);
		printf("Pino SHK em nivel alto e estado off-hook\n");
	}
	
	else 
	{
		printf("Pino SHK em nivel alto e estado off-hook\n");
	}
	
	gpioDelay(500000);
}

void clearscreen ()
{
    printf("\033[2J\033[1;1H");
}

void verificar_RV(){
    gpioSetMode(RV, PI_INPUT);
    int state = gpioRead(RV);
	
    do{
	    if(state == 0)
	    {
		    printf("Ringing Voltage detetado\n");
            gpioDelay(7000000);
            break;
	    }
    }while(state!=0);
    colocar_off_hook();
    printf("Chamada estabelecida\n");
}

void findMax(const double *array, int size, double *maxValue, int *maxIndex){
    if(size<=0){
        printf("Array vazio\n");
        return;
    }

    *maxValue=0;
    *maxIndex=0;

    for(int i=1; i<size;i++){
        if(array[i]>*maxValue){
            *maxValue=array[i];
            *maxIndex=i;
        }
    }
}

void verificar_dial_tone(){
    
    int size=NUM_SAMPLES/2+1;
	double samplearray[NUM_SAMPLES];
    double complex fftResult[size];

	if (open_adc() != 1){ // open the ADC SPI channel
			exit(1); // if the SPI bus fails to open exit the program
		}

	struct timeval t1, t2;
	double elapsedTime;

	printf("Starting dial test...");

	// start timer
	gettimeofday(&t1, NULL);

	for (int x = 0; x < NUM_SAMPLES; x++)
	{
		samplearray[x] = read_adc_voltage(1, 0); // read from adc channel 1
	}

	// stop timer
	gettimeofday(&t2, NULL);

	// calculate the average value
	double average = 0;
	double sum = 0;

	for (int x = 0; x < NUM_SAMPLES; x++)
	{
		sum += samplearray[x];
	}

	average = sum / NUM_SAMPLES;

	// compute and print the elapsed time in millisec
	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;	// sec to ms
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms

	// compute the sample rate
	double samplerate = (NUM_SAMPLES / elapsedTime) * 1000;

	printf("%d samples in %G ms.\nThe sample rate was %G samples per second\nThe average voltage was %Gv\n", NUM_SAMPLES, elapsedTime, samplerate, average);
    
	// Calcule a FFT
    performFFT(samplearray, fftResult, NUM_SAMPLES);

    // Frequências correspondentes
    double frequencies[size];
    double magnitude[size];

    for(int i=0;i<size;i++){
        frequencies[i]=i*0.1;
    }
	//Ao analisar a magnitude do espectro, é possível identificar quais frequências têm maior influência ou contribuição no sinal.
    for (int i = 0; i <= size; i++) {
        magnitude[i]=cabs(fftResult[i]);
        magnitude[0]=0;
        //printf("Freq: %.2f e mag: %.4f\n",frequencies[i],magnitude[i]);
    }

	//Code to save magnitudes to .txt
    // FILE *f = fopen("magnitude.txt", "wb");
    // for (int i = 0; i < size; i++) {
    //     fprintf(f, "%lf\n", magnitude[i]);
    // }
    // fclose(f);

    double maxValue;
    int maxIndex;
    int aux;
    findMax(magnitude, size/2, &maxValue, &maxIndex);



    //printf("Max value: %f e max index: %d\n",maxValue,maxIndex);

    double freq = frequencies[maxIndex];
    //freq=freq*2;
    printf("Dial-tone detetado com Frequência: %f\n", freq);
}

int main(int argc, char **argv){
	setvbuf (stdout, NULL, _IONBF, 0); // needed to print to the command line

	if(gpioInitialise() < 1)
		{
			fprintf(stderr,"Falha ao inicializar o pigpio\n");
			return EXIT_FAILURE;
		}    

/*   int funcao;
    printf("Recetor ou emissor? Recetor(1) Emissor(0)\n");
    scanf("%d",&funcao);

    if(funcao==0){
    colocar_off_hook();
    usleep(2000000); //Pausa 2 seg
    verificar_dial_tone();
    usleep(10000000); //Pausa 10 seg para marcar o tom
    //Marcar dtmf 
    }
    else{
        verificar_RV();
    }
	

	(void)argc;
	(void)argv;

    //gpioWrite(LRC, 0); //colocar relé a 0 na terminação*/ 

    colocar_off_hook();

    verificar_dial_tone();

    gpioTerminate();
    
	return (0);
}