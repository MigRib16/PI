// shared_code.c
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <stdio.h>

#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fftw3.h>
#include <math.h>
#define PORTA_COMUNICACAO 12345
#define TAMANHO_MAX_VARIAVEL 100
#define LRC 17
#define SHK 27
#define CHANNEL 0       // SPI channel
#define SPEED 1000000    // SPI speed

// Define ADC input channel
#define ADC_CHANNEL 0    // Assuming AIN0 is used

// FIR filter coefficients for a bandpass filter
const double fir_coeffs[] = {0.000678, 0.000504, -0.001055, -0.000000, 0.002536, -0.000000, -0.005715, -0.000000, 0.010381, -0.000000, -0.016616, -0.000000, 0.025228, -0.000000, -0.038748, -0.000000, 0.065030, 0.000000, -0.176141, 0.000000, 0.360000, 0.500000, 0.360000, 0.000000, -0.176141, 0.000000, 0.065030, -0.000000, -0.038748, -0.000000, 0.025228, -0.000000, -0.016616, -0.000000, 0.010381, -0.000000, -0.005715, -0.000000, 0.002536, -0.000000, -0.001055, 0.000504, 0.000678};

#define FIR_ORDER ((sizeof(fir_coeffs)/sizeof(fir_coeffs[0])) - 1)

double fir_buffer[FIR_ORDER + 1] = {0};

// Function to apply a bandpass FIR filter
double bandpass_filter(double input) {
    double output = 0;

    // Shift the input values in the buffer
    for (int i = FIR_ORDER; i > 0; --i) {
        fir_buffer[i] = fir_buffer[i - 1];
    }

    fir_buffer[0] = input;

    // Apply the FIR filter
    for (int i = 0; i <= FIR_ORDER; ++i) {
        output += fir_coeffs[i] * fir_buffer[i];
    }

    return output;
}

int read_adc(int channel) {
    int value;
    unsigned char buffer[2];

    buffer[0] = 0b11000000 | (channel << 3);
    buffer[1] = 0x00;

    wiringPiSPIDataRW(CHANNEL, buffer, 2);

    value = ((buffer[0] & 0x01) << 9) | (buffer[1] << 1);
    return value;
}

void simular_off_hook() {
    pinMode(LRC, OUTPUT);
    pinMode(SHK, INPUT);
    digitalWrite(LRC, HIGH);
    if(SHK==HIGH)
    {
        printf("Telefone fora do gancho (Off-hook)\n");
    }
    else if(SHK==LOW)
    {
        printf("Telefone no gancho (On-hook)\n");
    }

}

void simular_on_hook() {
    pinMode(LRC, OUTPUT);
    pinMode(SHK, INPUT);
    digitalWrite(LRC, LOW);
    if(SHK==LOW)
    {
        printf("Telefone no gancho (On-hook)\n");
    }
    if(SHK==HIGH)
    {
        printf("Telefone fora do gancho (Off-hook)\n");
    }
}

void simular_dial_tone() {
     int adc_value = read_adc(ADC_CHANNEL);
        double filtered_value = bandpass_filter((double)adc_value);

        printf("ADC Value: %d, Filtered Value: %.2f\n", adc_value, filtered_value);
        delay(1000); // 1 second delay

    printf("Dial tone presente\n");
}

//void simular_dtmf_transmissor(char dtmf_digit) {
    //pinMode(D0,INPUT);
    //printf("Transmitindo DTMF: %c\n", dtmf_digit);
//}

void simular_dtmf_receptor(char dtmf_digit) {
    printf("Recebendo DTMF: %c\n", dtmf_digit);
}

void transmissor(int argc, char *argv[]) {
    printf("Função: Transmissor\n");
    printf("Variáveis recebidas:\n");
    for (int i = 2; i < argc; ++i) {
        printf("- %s\n", argv[i]);
    }

    // Lógica específica do transmissor aqui
}

void recetor() {
    printf("Função: Recetor\n");
    // Lógica específica do recetor aqui
}

int main(int argc, char *argv[]) {
    // Se não foi especificado se é transmissor ou recetor, mostrar uma mensagem de erro
    //if (argc < 2) {
        //fprintf(stderr, "Uso: %s <transmissor ou recetor> [variaveis...]\n", argv[0]);
        //exit(EXIT_FAILURE);
    //}

    //if (strcmp(argv[1], "transmissor") == 0) {
        //transmissor(argc, argv);
    //} else if (strcmp(argv[1], "recetor") == 0) {
        //recetor();
    //} else {
        //fprintf(stderr, "Argumento inválido. Use 'transmissor' ou 'recetor'.\n");
        //exit(EXIT_FAILURE);
    //}
    wiringPiSetup();
    wiringPiSetupGpio();
simular_off_hook() ;
    return 0;
}
