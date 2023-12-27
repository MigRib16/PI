#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>

#define D3 25
#define D2 24
#define D1 23
#define D0 22

int main(){
	int escolha;
	int num;
	int dtmf;
	
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
	
	while(1)
	{
	//define state of input
	
    //printf("Quer enviar um tom DTMF? Enviar(1) Terminar(2)\n");
	//scanf("%d", &escolha);
	
	//if (escolha==2)
	//	break;
	
	printf("Qual o tom que quer Enviar?: ");
	scanf("%d", &num);
	
	//Decimal To Binary
	int binary[5];
	int i = 0;
	
	if(num == 0){
		for(int j = 0; j < 6; j++) {
			binary[j] = 0; } }
		
	while(num > 0) {
		binary[i] = num % 2; 
		printf("%d",binary[i]);
		num = num / 2;
		i++; }

	//gpioWrite(D0, binary[0]);
	//gpioWrite(D1, binary[1]);
	//gpioWrite(D2, binary[2]);
	//gpioWrite(D3, binary[3]);
	
	//gpioDelay(1000000);
	//int state0 = gpioRead(D0);
	//int state1 = gpioRead(D1);
	//int state2 = gpioRead(D2);
	//int state3 = gpioRead(D3);

	//printf("O DTMF enviado foi %d %d %d %d\n", binary[3], binary[2], binary[1], binary[0]);
	//printf("O DTMF enviado foi %d %d %d %d\n", state3, state2, state1, state0);
	
	//gpioDelay(500000);
	//}
	//gpioTerminate();
}
	return EXIT_SUCCESS;
}

//gcc -o teste_dtmf teste_dtmf.c -lpigpio -lrt -lpthread

//sudo ./teste_dtmf
