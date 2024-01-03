/*
 * teste_adc.c
 *
 *  Created on: 27 Dec 2023
 *
 *      compile with "gcc -o teste_dial teste_dial.c -lpigpio -lrt -lpthread -lfftw3 -lfftw3f -lm"
 *      run with "sudo ./teste_dial"
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include <complex.h>
#include <fftw3.h>
//#include "../ABE_ADCDACPi.h"  //alterar

#define NUM_SAMPLES 200000
#define SAMPLING_RATE 20000 // Substitua pelo valor correto

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

void performFFT(double *input, double complex *output, int size) {
    fftw_plan plan = fftw_plan_r2r_1d(size, input, (fftw_complex *)output, FFTW_R2HC, FFTW_ESTIMATE);
    fftw_execute(plan);
    for (int i = 0; i < N; ++i) {
        output[i] /= sqrt(2.0 * size);
    }
    
    fftw_destroy_plan(plan);
}

void clearscreen ()
{
    printf("\033[2J\033[1;1H");
}

int main(int argc, char **argv){
	setvbuf (stdout, NULL, _IONBF, 0); // needed to print to the command line
    int size=NUM_SAMPLES/2+1;
	double samplearray[NUM_SAMPLES];
    double complex fftResult[size];
    FILE *fp;
	fp=fopen("output.txt", "r");
    if (fp == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        return 1;
    }

    // Ler os valores do arquivo
    for (int i = 0; i < NUM_SAMPLES; i++) {
        if (fscanf(fp, "%lf", &samplearray[i]) != 1) {
            fprintf(stderr, "Erro ao ler valor do arquivo.\n");
            fclose(fp);
            return 1;
        }
    }

    // Fechar o arquivo
    fclose(fp);
	// Tempo total de gravação
    double totalTime = NUM_SAMPLES / SAMPLING_RATE;

    // Tempo de amostragem
    double time[NUM_SAMPLES];
    for (int i = 0; i < NUM_SAMPLES; i++) {
        time[i] = i / SAMPLING_RATE;
    }

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
        printf("Freq: %.2f e mag: %.4f\n",frequencies[i],magnitude[i]);
         //usleep(050000);
    }

	// Calcule a magnitude do espectro
    
    FILE *f = fopen("magnitude.txt", "wb");
    for (int i = 0; i < size; i++) {
        fprintf(f, "%lf\n", magnitude[i]);
    }
    fclose(f);

    double maxValue;
    int maxIndex;
    int aux;
    findMax(magnitude, size/2, &maxValue, &maxIndex);



    printf("Max value: %f e max index: %d\n",maxValue,maxIndex);

    double freq = frequencies[maxIndex];
    printf("Sinal detetado com Frquência: %f\n", freq);

    // // Imprimir o sinal no domínio do tempo
    // printf("Sinal no Domínio do Tempo:\n");
    // for (int i = 0; i < NUM_SAMPLES; i++) {
    //     printf("%f %f\n", creal(samplearray[i]), cimag(samplearray[i]));
    // }

    // // Imprimir a magnitude do espectro no domínio da frequência
    // printf("\nMagnitude do Espectro de Frequência:\n");
    // for (int i = 0; i <= NUM_SAMPLES / 2; i++) {
    //     printf("%f %f\n", frequencies[i], magnitude[i]);
    // }

	(void)argc;
	(void)argv;
	return (0);
}