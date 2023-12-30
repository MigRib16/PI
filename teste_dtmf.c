/*
 * teste_adc.c
 *
 *  Created on: 27 Dec 2023
 *
 *      compile with "gcc -o teste_dtmf teste_dtmf.c -lpigpio -lrt -lpthread"
 *      run with "sudo ./teste_dtmf"
 */

#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>

#define D3 25
#define D2 24
#define D1 23
#define D0 4

#define RD 15
#define WR 0
#define RSO 1
#define CS 14
#define IRQ 5

void readStatus() {

	gpioWrite(RSO, 1);		//Read Status
	gpioWrite(RD, 0);	
	gpioWrite(WR, 1);

	int stateIRQ = gpioRead(D0);
	printf("Temos: IRQ = %d\n", stateIRQ);

	return; }

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

void SetMode() {
	gpioDelay(50000);

	gpioWrite(RSO, 1);			// Write to Control A '1101'

	gpioWrite(D0, 1);
	gpioWrite(D1, 0);
	gpioWrite(D2, 1);
	gpioWrite(D3, 1);

	gpioWrite(RD, 1);
	gpioWrite(WR, 0);
	gpioDelay(50000);

	gpioWrite(RSO, 1);			// Write to Control B '0000'

	gpioWrite(D0, 0);
	gpioWrite(D1, 0);
	gpioWrite(D2, 0);
	gpioWrite(D3, 0);

	gpioWrite(RD, 1);			
	gpioWrite(WR, 0);
	gpioDelay(50000);

}

void DataBusRD() {
	int state0 = gpioRead(D0);
	int state1 = gpioRead(D1);
	int state2 = gpioRead(D2);
	int state3 = gpioRead(D3);
	printf("O DTMF recebido foi %d %d %d %d\n", state3, state2, state1, state0); }

void StateIRQ() {
	int stateIRQ = gpioRead(IRQ);
	printf("Temos: IRQ = %d\n", stateIRQ); }

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
	gpioSetMode(CS, PI_OUTPUT);
	gpioSetMode(IRQ, PI_INPUT);

	Inicialization();

	ReseTone();

	while(1)
	{
	//define state of input
	
    printf("Quer enviar um tom DTMF? Enviar(1) Terminar(2)\n");
	scanf("%d", &escolha);
	
	if (escolha==2)
		break;
	
	int number[5];
	int dtmf[5];

	for(int k = 0; k < 5; k++) {

		printf("Que Dígito que marcar?: ");
		scanf("%d", &num);
		number[k] = num;
		dtmf[k] = num;
	
		//Decimal To Binary
		int binary[4];
	
		if(number[k] == 0) 
			number[k] = 10;

		// counter for binary array 
		if(number[k] > 0) {
			for(int i = 0; i < 4; i++)   {    
				binary[i] = number[k] % 2;    
				number[k] =  number[k] / 2;  }  }

		SetMode();

		gpioDelay(50000);

		gpioWrite(RSO, 0);			// Write to Transmit Data Register 'INPUT'

		gpioWrite(D0, binary[0]);
		gpioWrite(D1, binary[1]);
		gpioWrite(D2, binary[2]);
		gpioWrite(D3, binary[3]);
		StateIRQ();	

		gpioDelay(50000); 
		gpioWrite(RD, 1);
		gpioWrite(WR, 0);

		//readStatus();
		//gpioDelay(5000);
		//StateIRQ();	
		gpioDelay(500000); } 

		printf("Você ligou para o número %d %d %d %d\n", dtmf[1], dtmf[2], dtmf[3], dtmf[4]);
	 }

	gpioTerminate();

	return EXIT_SUCCESS;
}