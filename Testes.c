#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pigpio.h>
#include <complex.h>
#include <fftw3.h>
#include <math.h>
#include "../../ABElectronics_C_Libraries/ADCDACPi/ABE_ADCDACPi.h"  
//gcc ../../ABElectronics_C_Libraries/ADCDACPi/ABE_ADCDACPi.c Testes.c -o Testes -lpigpio -lrt -lpthread -lfftw3 -lfftw3f -lm

//sudo apt-get update
//sudo apt-get install ffmpeg
//ffmpeg -i seu_arquivo.mp4 -vn -acodec pcm_s16le -ar 44100 -ac 2 output.wav


#define D3 25
#define D2 24
#define D1 23
#define D0 3

#define SINAL 12

#define RD 15
#define WR 0
#define RSO 1
#define IRQ 16

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
        gpioSetMode(LRC, PI_OUTPUT);
        gpioSetMode(LRD, PI_INPUT);
        gpioWrite(LRC, 1);
        printf("O Emulador encontra-se no estado: off-hook\n");
        gpioDelay(500000);
}

void colocar_on_hook() {
        gpioSetMode(LRC, PI_OUTPUT);
        gpioSetMode(LRD, PI_INPUT);
        gpioWrite(LRC, 0);
        printf("\nO Emulador encontra-se no estado: on-hook\n");
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

	//printf("%d samples in %G ms.\nThe sample rate was %G samples per second\nThe average voltage was %Gv\n", NUM_SAMPLES, elapsedTime, samplerate, average);
    
	// Calcule a FFT
    performFFT(samplearray, fftResult, NUM_SAMPLES);

    // Frequências correspondentes
    double frequencies[size];
    double magnitude[size];
    double step = samplerate / (NUM_SAMPLES);
    for(int i=0;i<=NUM_SAMPLES/2;i++){
        frequencies[i]=i*step;
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
    freq=freq*2;
	if(freq>350 && freq<450){
    	printf("Dial-tone detetado com frequência: %f Hz\n", freq);
		printf("Teste dial-tone: Sucesso\n");
	}
	else{
		printf("Sinal com frequência: %f Hz detetado\n",freq);
		printf("Teste dial-tone: Falhado\n");
	}
}

// DTMF
void Reset() {
	gpioWrite(RD, 1);	
	gpioWrite(WR, 1);
	gpioDelay(5000); 
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

int DataBusRD() {
	int decimal;
	int state0 = gpioRead(D0);
	int state1 = gpioRead(D1);
	int state2 = gpioRead(D2);
	int state3 = gpioRead(D3);

	decimal = state0*1+state1*2+state2*4+state3*8;
	if(decimal == 10)
		decimal = 0;

	//int stateIRQ = gpioRead(IRQ);
	//printf("Temos: IRQ = %d\n", stateIRQ);
	//printf("O DTMF enviado foi %d %d %d %d\n", state3, state2, state1, state0); 
	printf("O DTMF enviado foi %d\n", decimal); 
	
	return decimal; }

void readStatus() {

	gpioWrite(RSO, 1);		//Read Status
	gpioDelay(5000);
	Reset();

	gpioWrite(RD, 0);	
	gpioWrite(WR, 1);
	gpioDelay(50000); 

	Reset();	
}

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

void mandar_dtmf() {

	int bin[4];

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

    gpioWrite(RSO, 0);		// Write 0001 on Transmit Data  
	gpioDelay(50000);

	gpioWrite(D0, 1);
	gpioWrite(D1, 0);
	gpioWrite(D2, 0);
	gpioWrite(D3, 0);
	gpioDelay(50000);

	gpioWrite(RD, 1);
	gpioWrite(WR, 0);
	gpioDelay(500000);

	bin[0] = DataBusRD();
	
	Reset();  
	
	readStatus(); 

	gpioWrite(RSO, 0);		// Write 1010 on Transmit Data  
	gpioDelay(50000);

	Reset();

	gpioWrite(D0, 0);
	gpioWrite(D1, 1);
	gpioWrite(D2, 0);
	gpioWrite(D3, 1);
	gpioDelay(50000);

	gpioWrite(RD, 1);
	gpioWrite(WR, 0);
	gpioDelay(500000);

	bin[1] = DataBusRD();
	Reset();

	readStatus(); 

	gpioWrite(RSO, 0);		// Write 1010 on Transmit Data  
	gpioDelay(500000);

	Reset();

	gpioWrite(D0, 0);
	gpioWrite(D1, 1);
	gpioWrite(D2, 0);
	gpioWrite(D3, 1);
	gpioDelay(50000);

	gpioWrite(RD, 1);
	gpioWrite(WR, 0);
	gpioDelay(500000);

	bin[2] = DataBusRD();
	Reset();

	readStatus(); 

	gpioWrite(RSO, 0);		// Write 0010 on Transmit Data  
	gpioDelay(50000);

	Reset();

	gpioWrite(D0, 0);
	gpioWrite(D1, 1);
	gpioWrite(D2, 0);
	gpioWrite(D3, 0);
	gpioDelay(5000);

	gpioWrite(RD, 1);
	gpioWrite(WR, 0);
	gpioDelay(500000);

	bin[3] = DataBusRD();
	Reset();

	readStatus();

	int num = bin[0]*1000 + bin[1]*100 + bin [2]*10 + bin[3];

	if(num == 1002){
    	printf("Chamada enviada para o número: %d\n", num);
		printf("Envio de Tons DTMF: Sucesso\n");
	}
	else{
		printf("Chamada enviada para o número: %d\n", num);
		printf("Envio de Tons DTMF: Falhado\n");
	}

}

void mandar_dtmf_canal() {

	//define state of input
	int close;
	
	printf("\nQuer Enviar Tons DTMF? Sim(0), Não(1): ");
	scanf("%d", &close);
	
	if (close >= 1) {
		return; }


	int escolha;
	int bin[3];
	int i = 0;

	printf("A enviar Tons DTMF para o canal...\n");
	usleep(1000000);

	gpioWrite(RSO, 0);		// Write 0001 on Transmit Data  
	gpioDelay(50000);

	gpioWrite(D0, 1);
	gpioWrite(D1, 0);
	gpioWrite(D2, 0);
	gpioWrite(D3, 0);
	gpioDelay(5000);

	gpioWrite(RD, 1);
	gpioWrite(WR, 0);
	gpioDelay(500000);

	bin[0] = DataBusRD();
	
	Reset();  
	
	readStatus();  
	
	gpioWrite(RSO, 0);		// Write 0001 on Transmit Data  
	gpioDelay(50000);

	gpioWrite(D0, 0);
	gpioWrite(D1, 1);
	gpioWrite(D2, 0);
	gpioWrite(D3, 0);
	gpioDelay(5000);

	gpioWrite(RD, 1);
	gpioWrite(WR, 0);
	gpioDelay(500000);

	bin[1] = DataBusRD();
	
	Reset();  
	
	readStatus();
	
	gpioWrite(RSO, 0);		// Write 0001 on Transmit Data  
	gpioDelay(50000);

	gpioWrite(D0, 1);
	gpioWrite(D1, 1);
	gpioWrite(D2, 0);
	gpioWrite(D3, 0);
	gpioDelay(5000);

	gpioWrite(RD, 1);
	gpioWrite(WR, 0);
	gpioDelay(500000);

	bin[2] = DataBusRD();
	
	Reset();  
	
	readStatus();
	usleep(500000);

	int num = bin[0]*100 + bin [1]*10 + bin[2];

    printf("Tons Enviados para o Canal: %d\n", num);
	usleep(3000000);
	
}

void mandar_sinal() {
    
    gpioSetMode(SINAL, PI_OUTPUT);

    double Duration = 10;
    double StartTime = time_time();
    double CurrentTime;

    while((CurrentTime = time_time()) - StartTime < Duration ) {
        double freq = 2000;
        double amp = 0.02;

        double time = CurrentTime - StartTime;
        double value = amp * sin(2*3.14159*freq*time);

        gpioSetPWMfrequency(SINAL, 500);
        gpioPWM(SINAL, (int)((value + 1.0)* 127.5));
        time_sleep(0.01);
    }
}

void receive_call() {

	gpioSetMode(RV, PI_INPUT);

        colocar_on_hook();
        printf("Á espera de pedido de Chamada...\n");  

        struct timeval t1, t2;
        // start timer
        gettimeofday(&t1, NULL);

	while(1)
	{

	//define state of input
	int state = gpioRead(RV); 
	
	if(state == 0) {
		printf("Ringing Voltage detetado\n");
        	gpioDelay(500000);
                colocar_off_hook(); 
                break; }
             
        // stop timer
		gettimeofday(&t2, NULL);
        if (t2.tv_sec - t1.tv_sec > 30) {
                printf("Não foi recebido nenhum pedido de chamada!\n");  
                break; } 

        }
}

int main(int argc, char **argv){
	setvbuf (stdout, NULL, _IONBF, 0); // needed to print to the command line

	if(gpioInitialise() < 1)
		{
			fprintf(stderr,"Falha ao inicializar o pigpio\n");
			return EXIT_FAILURE;
		}    

	int funcao = 3;

	while (funcao > 2) {

	printf("Recetor ou emissor? Recetor(0), Emissor(1), Terminar(2): ");
    scanf("%d",&funcao);

    if(funcao == 1) {
		printf("\nInício dos Testes\n");
		usleep(2000000);
		colocar_on_hook();
		usleep(2000000);
		colocar_off_hook();
		usleep(4000000);
		printf("\nA começar o teste de dial-tone...\n");
        verificar_dial_tone();
        usleep(2000000); //Pausa 10 seg para marcar o tom
    //  Marcar dtmf 
		printf("\nA enviar tons DTMF...\n");
    	mandar_dtmf();
	    usleep(1000000);
		printf("\nA Estabelecer Chamada...\n");
		usleep(10000000);
		mandar_dtmf_canal();
		usleep(1000000);

    }
    else if(funcao == 0) {
        receive_call();
		usleep(1000000);
        mandar_dtmf_canal();
		usleep(1000000);
    }
	else if(funcao == 2) {
        printf("Terminou o programa.\n");
		break;
    }
	else {
		printf("Resposta inválida!\n");
	} }
	
	(void)argc;
	(void)argv;

    gpioWrite(LRC, 0); //colocar relé a 0 na terminação

    gpioTerminate();
    
	return (0);
}
