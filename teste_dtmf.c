/*
 * teste_adc.c
 *
 *  Created on: 27 Dec 2023
 *
 *      compile with "gcc -o teste_dtmf teste_dtmf.c -lpigpio -lrt -lpthread -lm"
 *      run with "sudo ./teste_dtmf"
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
	while(1)
	{
	//define state of input
	
	printf("Efetuar off-hook? Sim(1), Não(0), Terminar(2)\n");
	scanf("%d", &escolha);
	
	if (escolha==2)
		break;
		
	gpioWrite(LRC, escolha);  // 0 = OnHook | 1 = OffHook
	
	gpioDelay(1000000);
	int state = gpioRead(LRD);
	
	if(state == 1)
	{
		printf("Pino SHK em nivel baixo e estado on-hook\n");
	}
	
	else 
	{
		printf("Pino SHK em nivel alto e estado off-hook\n");
	}
	
	gpioDelay(500000);
	}
}

void DataBusRD() {
	int state0 = gpioRead(D0);
	int state1 = gpioRead(D1);
	int state2 = gpioRead(D2);
	int state3 = gpioRead(D3);
	printf("O DTMF recebido foi %d %d %d %d\n", state3, state2, state1, state0); }

void Reset() {
	gpioWrite(RD, 1);	
	gpioWrite(WR, 1);
	gpioDelay(50000); 
}

void Print_RSO() {
	int stateRSO = gpioRead(RSO);
	printf("O RSO está a %d \n", stateRSO);
}

void High_RSO() {

	int stateRSO = gpioRead(RSO);
	Print_RSO();

	while(stateRSO == 0) {
		gpioWrite(RSO, 1);
		gpioDelay(5000);
		Print_RSO();
		break; }

	return;
}

void readStatus() {

	gpioWrite(RSO, 1);		//Read Status
	Reset();

	gpioWrite(RD, 0);	
	//gpioWrite(WR, 1);
	gpioDelay(50000); 

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
	gpioDelay(50000);
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

	gpioWrite(D0, 0);
	gpioWrite(D1, 0);
	gpioWrite(D2, 0);
	gpioWrite(D3, 0);
	gpioDelay(5000);

	gpioWrite(RD, 1);
	gpioWrite(WR, 0);
	gpioDelay(50000);
	
	Reset(); }

void ReadOut() {

	gpioWrite(RD, 0);
	gpioWrite(WR, 1);
	gpioDelay(500000); 

    int decimal = 0, rem = 0;
    int binario[4]; 

	binario[0] = gpioRead(D0);
	binario[1] = gpioRead(D1);
	binario[2] = gpioRead(D2);
	binario[3] = gpioRead(D3);

    for (int i = 0; i < 4; ++i) {
        decimal += binario[i] * pow(2, i);
    }

	printf("Binário é %d %d %d %d\n", binario[3], binario[2], binario[1], binario[0]); 
	printf("Você recebeu o tom %d\n", decimal);
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

	colocar_off_hook()

	Inicialization();

	SetMode();

	while(1) {
	//define state of input
	
    printf("Quer enviar um tom DTMF? Enviar(1) Terminar(2)\n");
	scanf("%d", &escolha);
	
	if (escolha==2)
		break;
	
	int number[4];
	int dtmf[4];

	for(int k = 0; k < 4; k++) {

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

		printf("O DTMF recebido foi %d %d %d %d\n", binary[3], binary[2], binary[1], binary[0]); 

		gpioDelay(10000); 
		gpioWrite(RSO, 0);			// Write to Transmit Data Register 'INPUT'
		gpioDelay(50000); 

		gpioWrite(D0, binary[0]);
		gpioWrite(D1, binary[1]);
		gpioWrite(D2, binary[2]);
		gpioWrite(D3, binary[3]);
		gpioDelay(5000); 

		gpioWrite(RD, 1);
		gpioWrite(WR, 0);
		gpioDelay(500000); 

		DataBusRD();

		Reset();  
	
		readStatus(); 
	
		} 

		printf("Você ligou para o número %d %d %d %d\n", dtmf[0], dtmf[1], dtmf[2], dtmf[3]);
 	}

	gpioTerminate();

	return EXIT_SUCCESS;
}