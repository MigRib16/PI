#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <complex.h>

//#include "../ABE_ADCDACPi.h"  //alterar

#define NUM_SAMPLES 200000
#define SAMPLING_RATE 20000 // Substitua pelo valor correto

void performFFT(double *input, double *output, int size) {
    fftw_plan plan = fftw_plan_r2r_1d(size, input, output, FFTW_R2HC, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
}

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

	double samplearray[NUM_SAMPLES]
    double complex fftResult[NUM_SAMPLES];
	file = fopen("output.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        return 1;
    }

    // Ler os valores do arquivo
    for (int i = 0; i < NUM_SAMPLES; i++) {
        if (fscanf(file, "%lf", &adcValues[i]) != 1) {
            fprintf(stderr, "Erro ao ler valor do arquivo.\n");
            fclose(file);
            return 1;
        }
    }

    // Fechar o arquivo
    fclose(file);
	// Tempo total de gravação
    double totalTime = NUM_SAMPLES / SAMPLING_RATE;

    // Tempo de amostragem
    double time[NUM_SAMPLES];
    for (int i = 0; i < NUM_SAMPLES; i++) {
        time[i] = i / SAMPLING_RATE;
    }

	for (x = 0; x < numberofsamples; x++)
	    {
		    samplearray[x] = read_adc_voltage(1, 0); // read from adc channel 1
	    }

    // Calcule a FFT
    performFFT(samplearray, fftResult, NUM_SAMPLES);

    // Frequências correspondentes
    double frequencies[NUM_SAMPLES / 2 + 1];
    for (int i = 0; i <= NUM_SAMPLES / 2; i++) {
        frequencies[i] = i * SAMPLING_RATE / NUM_SAMPLES;
    }

	// Calcule a magnitude do espectro
    double magnitude[NUM_SAMPLES / 2 + 1];
    for (int i = 0; i <= NUM_SAMPLES / 2; i++) {
        magnitude[i] = cabs(fftResult[i]);
    }

    // Imprimir o sinal no domínio do tempo
    printf("Sinal no Domínio do Tempo:\n");
    for (int i = 0; i < NUM_SAMPLES; i++) {
        printf("%f %f\n", creal(samplearray[i]), cimag(samplearray[i]));
    }

    // Imprimir a magnitude do espectro no domínio da frequência
    printf("\nMagnitude do Espectro de Frequência:\n");
    for (int i = 0; i <= NUM_SAMPLES / 2; i++) {
        printf("%f %f\n", frequencies[i], magnitude[i]);
    }

	(void)argc;
	(void)argv;
	return (0);
}