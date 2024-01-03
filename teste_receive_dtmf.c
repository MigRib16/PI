/*
 * teste_receive_dtmf.c
 *
 *  Created on: 27 Dec 2023
 *
 *      compile with "gcc -o teste_receive_dtmf teste_receive_dtmf.c -lpigpio -lrt -lpthread -lm"
 *      run with "sudo ./teste_receive_dtmf"
 */

#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
#include <math.h>

#define D3 25
#define D2 24
#define D1 23
#define D0 3

#define RD 15
#define WR 0
#define RSO 1
#define IRQ 5

void Reset() {
	gpioWrite(RD, 1);	
	gpioWrite(WR, 1);
	gpioDelay(5000); 
}

void readStatus() {

	gpioWrite(RSO, 1);		//Read Status
	Reset();

	gpioWrite(RD, 0);	

	int stateIRQ = gpioRead(IRQ);
	printf("Temos: IRQ = %d\n", stateIRQ);
	gpioDelay(500);

	Reset();	

	return; }

void Inicialization() {

	gpioDelay(100000);

	readStatus();

	Reset();

	// Write to Control Register -> CRA -> WR 0 0 0 0 -> CRA -> WR 1 0 0 0 -> Change to CRB -> CRB -> WR 0 0 0 0
	gpioWrite(D0, 0);
	gpioWrite(D1, 0);
	gpioWrite(D2, 0);
	gpioWrite(D3, 0);
	gpioDelay(500);

	gpioWrite(WR, 0);		// CRA: Escrever 0000
	gpioWrite(RD, 1);
	gpioDelay(5000);

	Reset();

	gpioWrite(WR, 0);		// CRA: Escrever 0000
	gpioWrite(RD, 1);
	gpioDelay(5000);

	Reset();

	gpioWrite(D3, 1);
	gpioDelay(500);

	gpioWrite(WR, 0);		// CRA: Escrever 1000 To Change To CRB
	gpioWrite(RD, 1);
	gpioDelay(5000);

	Reset();

	gpioWrite(D3, 0);		// CRB: Escrever 0000
	gpioDelay(500);

	gpioWrite(WR, 0);	
	gpioWrite(RD, 1);
	gpioDelay(5000);

	Reset();

	readStatus();

	return; }

void SetMode() {

	gpioWrite(RSO, 1);			// Write to Control A '1101'
	Reset();

	gpioWrite(D0, 1);
	gpioWrite(D1, 0);
	gpioWrite(D2, 1);
	gpioWrite(D3, 1);

	gpioWrite(RD, 1);
	gpioWrite(WR, 0);
	gpioDelay(5000);

	//gpioWrite(RSO, 1);			// Write to Control B '0000'
	Reset();

	gpioWrite(D0, 1);
	gpioWrite(D1, 0);
	gpioWrite(D2, 0);
	gpioWrite(D3, 0);

	gpioWrite(RD, 1);			
	gpioWrite(WR, 0);
	gpioDelay(5000);

	Reset();

}

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

	SetMode();

	while(1)
	{
	//define state of input

	gpioWrite(RSO, 0);		// Write 0000 on Transmit Data  
	gpioDelay(5000);
	Reset();

	gpioWrite(D0, 0);
	gpioWrite(D1, 1);
	gpioWrite(D2, 0);
	gpioWrite(D3, 1);
	gpioDelay(500);

	gpioWrite(RD, 1);
	gpioWrite(WR, 0);
	gpioDelay(50000);

	Reset(); }

	gpioTerminate();

	return EXIT_SUCCESS;
}