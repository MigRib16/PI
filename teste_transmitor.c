/*
 * teste_transmitor.c
 *
 *  Created on: 27 Dec 2023
 *
 *      compile with "gcc -o teste_transmitor teste_transmitor.c -lpigpio -lrt -lpthread -lm"
 *      run with "sudo ./teste_transmitor"
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

#define LRC 17
#define LRD 18

void colocar_off_hook() {
	int escolha;

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

void Reset() {
	gpioWrite(RD, 1);	
	gpioWrite(WR, 1);
	gpioDelay(5000); 
}

void Print_RSO() {
	int stateRSO = gpioRead(RSO);
	printf("O RSO está a %d \n", stateRSO);
}

void High_RSO() {

	int stateRSO = gpioRead(RSO);
	//Print_RSO();

	while(stateRSO == 0) {
		gpioWrite(RSO, 1);
		gpioDelay(5000);
		//Print_RSO();
		break; }

	return;
}

void DataBusRD() {
	int state0 = gpioRead(D0);
	int state1 = gpioRead(D1);
	int state2 = gpioRead(D2);
	int state3 = gpioRead(D3);
	int stateIRQ = gpioRead(IRQ);
	//printf("Temos: IRQ = %d\n", stateIRQ);
	printf("O DTMF enviado foi %d %d %d %d\n", state3, state2, state1, state0); }

void ReadStatusRD() {
	int state0 = gpioRead(D0);
	int state1 = gpioRead(D1);
	int state2 = gpioRead(D2);
	int state3 = gpioRead(D3);
	printf("O Read Status é %d %d %d %d\n", state3, state2, state1, state0); }

void readStatus() {

	gpioWrite(RSO, 1);		//Read Status
	Reset();

	gpioWrite(RD, 0);	
	gpioWrite(WR, 1);
	gpioDelay(50000); 

	ReadStatusRD();
	gpioDelay(5000); 

	Reset();	

	return; }

void Inicialization() {

	gpioDelay(100000);

	High_RSO();

	readStatus();
	// Write to Control Register -> CRA -> WR 0 0 0 0 -> CRA -> WR 1 0 0 0 -> Change to CRB -> CRB -> WR 0 0 0 0
	
	Reset();

	gpioWrite(D0, 0);
	gpioWrite(D1, 0);
	gpioWrite(D2, 0);
	gpioWrite(D3, 0);
	gpioDelay(5000);

	gpioWrite(WR, 0);		// CRA: Escrever 0000
	gpioWrite(RD, 1);
	gpioDelay(50000);

	Reset();

	gpioWrite(WR, 0);		// CRA: Escrever 0000
	gpioWrite(RD, 1);
	gpioDelay(50000);

	Reset();
	gpioDelay(50000); 

	gpioWrite(D3, 1);
	gpioDelay(5000);
	
	gpioWrite(WR, 0);		// CRA: Escrever 1000 To Change To CRB
	gpioWrite(RD, 1);
	gpioDelay(50000);

	Reset(); 
	gpioDelay(50000); 

	gpioWrite(D3, 0);		// CRB: Escrever 0000
	gpioDelay(5000);

	gpioWrite(WR, 0);	
	gpioWrite(RD, 1);
	gpioDelay(50000);

	Reset();

	readStatus();

	return; }

void SetMode() {

	gpioDelay(5000);
	High_RSO();

	gpioWrite(RSO, 1);			// Write to Control A '1101'
	gpioDelay(5000);
	Reset();

	gpioWrite(D0, 1);
	gpioWrite(D1, 0);
	gpioWrite(D2, 1);
	gpioWrite(D3, 1);
	gpioDelay(5000);

	gpioWrite(RD, 1);
	gpioWrite(WR, 0);
	gpioDelay(50000);

	Reset();

	gpioWrite(RSO, 1);			// Write to Control B '0000'
	gpioDelay(50000);

	gpioWrite(D0, 0);
	gpioWrite(D1, 0);
	gpioWrite(D2, 0);
	gpioWrite(D3, 0);
	gpioDelay(5000);

	gpioWrite(RD, 1);			
	gpioWrite(WR, 0);
	gpioDelay(50000);

	Reset();

}

void ReseTone() {

	gpioWrite(RSO, 0);		// Write 0000 on Transmit Data 
	gpioDelay(5000);

	Print_RSO();

	gpioWrite(D0, 0);
	gpioWrite(D1, 0);
	gpioWrite(D2, 0);
	gpioWrite(D3, 0);
	gpioDelay(5000);

	gpioWrite(RD, 1);
	gpioWrite(WR, 0);
	gpioDelay(50000); 

	Reset();
	
}

void mandar_dtmf() {

	while(1)
	{
	//define state of input
	int close;
	
	printf("Quer Enviar Tons DTMF? Sim(1), Não(2)\n");
	scanf("%d", &close);
	
	if (close == 2)
		break;


	int escolha;
	int bin[4];
	int i = 0;

	printf("Que Tom DTMF quer enviar?\n");
	scanf("%d", &escolha);

	while(escolha > 0)
	{
		// obtém o resto da divisão de num por 2
		bin[i] = escolha % 2;
		i++;
		escolha = escolha / 2;
	}

	gpioWrite(RSO, 0);		// Write 0010 on Transmit Data  
	gpioDelay(50000);

	Reset();

	gpioWrite(D0, bin[0]);
	gpioWrite(D1, bin[1]);
	gpioWrite(D2, bin[2]);
	gpioWrite(D3, bin[3]);
	gpioDelay(5000);

	gpioWrite(RD, 1);
	gpioWrite(WR, 0);
	gpioDelay(500000);

	DataBusRD();
	Reset();

	readStatus(); }

}

int main(){
	int num;
	
	if(gpioInitialise() < 1)
		{
			fprintf(stderr,"Falha ao inicializar o pigpio\n");
			return EXIT_FAILURE;
		}
		
	//define inputs outputs
	gpioSetMode(LRC, PI_OUTPUT);
	gpioSetMode(LRD, PI_INPUT);

	gpioSetMode(D3, PI_OUTPUT);
	gpioSetMode(D2, PI_OUTPUT);
	gpioSetMode(D1, PI_OUTPUT);
	gpioSetMode(D0, PI_OUTPUT);

	gpioSetMode(RD, PI_OUTPUT);
	gpioSetMode(WR, PI_OUTPUT);
	gpioSetMode(RSO, PI_OUTPUT);
	gpioSetMode(IRQ, PI_INPUT);

	colocar_off_hook();

	gpioDelay(500000);

	Inicialization();

	SetMode();

	//ReseTone();

	//while(1)
	//{
	//define state of input

	gpioWrite(RSO, 0);		// Write 0001 on Transmit Data  
	gpioDelay(500000);

	gpioWrite(D0, 1);
	gpioWrite(D1, 0);
	gpioWrite(D2, 0);
	gpioWrite(D3, 0);
	gpioDelay(5000);

	gpioWrite(RD, 1);
	gpioWrite(WR, 0);
	gpioDelay(500000);

	DataBusRD();
	
	Reset();  
	
	readStatus(); 

	gpioWrite(RSO, 0);		// Write 1010 on Transmit Data  
	gpioDelay(500000);

	Reset();

	gpioWrite(D0, 0);
	gpioWrite(D1, 1);
	gpioWrite(D2, 0);
	gpioWrite(D3, 1);
	gpioDelay(5000);

	gpioWrite(RD, 1);
	gpioWrite(WR, 0);
	gpioDelay(500000);

	DataBusRD();
	Reset();

	readStatus(); 

	gpioWrite(RSO, 0);		// Write 1010 on Transmit Data  
	gpioDelay(500000);

	Reset();

	gpioWrite(D0, 0);
	gpioWrite(D1, 1);
	gpioWrite(D2, 0);
	gpioWrite(D3, 1);
	gpioDelay(5000);

	gpioWrite(RD, 1);
	gpioWrite(WR, 0);
	gpioDelay(500000);

	DataBusRD();
	Reset();

	readStatus(); 

	gpioWrite(RSO, 0);		// Write 1010 on Transmit Data  
	gpioDelay(500000);

	Reset();

	gpioWrite(D0, 0);
	gpioWrite(D1, 1);
	gpioWrite(D2, 0);
	gpioWrite(D3, 0);
	gpioDelay(5000);

	gpioWrite(RD, 1);
	gpioWrite(WR, 0);
	gpioDelay(500000);

	DataBusRD();
	Reset();

	readStatus(); 

	gpioDelay(10000000);

	//mandar_dtmf();

	gpioDelay(5000000);

	//}

	gpioTerminate();

	return EXIT_SUCCESS;
}
