/*
 * teste_receive_dtmf.c
 *
 *  Created on: 27 Dec 2023
 *
 *      compile with "gcc -o teste_receive_dtmf teste_receive_dtmf.c -lpigpio -lrt -lpthread"
 *      run with "sudo ./teste_receive_dtmf"
 */

#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
#include <math.h>

#define D3 25
#define D2 24
#define D1 23
#define D0 4

#define RD 15
#define WR 0
#define RSO 1
#define CS 14
#define IRQ 5

void Inicialization() {

	gpioDelay(100000);

	readStatus();
	// Write to Control Register -> CRA -> WR 0 0 0 0 -> CRA -> WR 1 0 0 0 -> Change to CRB -> CRB -> WR 0 0 0 0
	gpioWrite(D0, 0);
	gpioWrite(D1, 0);
	gpioWrite(D2, 0);
	gpioWrite(D3, 0);

	gpioWrite(WR, 0);		// CRA: Escrever 0000
	gpioWrite(RD, 1);
	gpioDelay(500);

	gpioWrite(WR, 0);		// CRA: Escrever 0000
	gpioWrite(RD, 1);
	gpioDelay(500);

	gpioWrite(D3, 1);
	gpioWrite(WR, 0);		// CRA: Escrever 1000 To Change To CRB
	gpioWrite(RD, 1);
	gpioDelay(500);

	gpioWrite(D3, 0);		// CRB: Escrever 0000
	gpioWrite(WR, 0);	
	gpioWrite(RD, 1);
	gpioDelay(500);

	readStatus();

	return; }

void ReseTone() {

	gpioWrite(RSO, 0);		// Write 0000 on Transmit Data 

	gpioWrite(D0, 0);
	gpioWrite(D1, 0);
	gpioWrite(D2, 0);
	gpioWrite(D3, 0);

	gpioDelay(50000); 
	gpioWrite(RD, 1);
	gpioWrite(WR, 0);
	gpioDelay(50000); }

int main(){
	int escolha;
	int num;
	
	if(gpioInitialise() < 1)
		{
			fprintf(stderr,"Falha ao inicializar o pigpio\n");
			return EXIT_FAILURE;
		}
		
	//define inputs outputs
	gpioSetMode(D3, PI_OUTPUT);
	gpioSetMode(D2, PI_OUTPUT);
	gpioSetMode(D1, PI_OUTPUT);
	gpioSetMode(D0, PI_OUTPUT);

	gpioSetMode(RD, PI_OUTPUT);
	gpioSetMode(WR, PI_OUTPUT);
	gpioSetMode(RSO, PI_OUTPUT);
	gpioSetMode(IRQ, PI_INPUT);

	Inicialization();

	ReseTone();

	while(1)
	{
	//define state of input
    int number, resto;
    int decimal = 0, expoente = 0;
    int binario[4]; 

    gpioWrite(RSO, 0);		// Read on Receive Data Register
    gpioDelay(5000); 
	gpioWrite(RD, 0);
	gpioWrite(WR, 1);
	gpioDelay(5000);

    binario[0] = gpioRead(D0);
	binario[1] = gpioRead(D1);
	binario[2] = gpioRead(D2);
	binario[3] = gpioRead(D3);

    for (int i = 0; i < 4; ++i) {
        decimal += binario[i] * pow(2, 4 - 1 - i);
    }

	printf("Você recebeu o tom %d\n", decimal); }

	gpioTerminate();

	return EXIT_SUCCESS;
}