#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>

#define LRC 17
#define LRD 18

int main(){
	int escolha;
	if(gpioInitialise() < 1)
		{
			fprintf(stderr,"Falha ao inicializar o pigpio\n");
			return EXIT_FAILURE;
		}
		
	//define inputs outputs
	gpioSetMode(LRC, PI_OUTPUT);
	gpioSetMode(LRD, PI_INPUT);
	
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
	gpioTerminate();
	
	return EXIT_SUCCESS;
}

//gcc -o teste_rele teste_rele.c -lpigpio -lrt -lpthread

//sudo ./teste_rele
