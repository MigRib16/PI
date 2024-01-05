#include <stdio.h>
#include <pigpio.h>
#include <alsa/asoundlib.h>
//sudo apt-get install libasound2-dev
//gcc -o BoTemMel BoTemMel.c -lpigpio -lasound

#define PWM_PIN 12

void playWav(const char *fileName) {
    FILE *file;
    long fileSize;
    char *buffer;

    file = fopen(fileName, "rb");
    if (!file) {
        perror("Erro ao abrir arquivo WAV");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer = (char *)malloc(fileSize);
    if (!buffer) {
        perror("Erro ao alocar memória");
        exit(EXIT_FAILURE);
    }

    fread(buffer, 1, fileSize, file);

    fclose(file);

    gpioSetPWMfrequency(PWM_PIN, 44100);  // Frequência de amostragem do áudio
    gpioSetPWMrange(PWM_PIN, 255);  // Resolução do PWM

    for (int i = 0; i < fileSize; i++) {
        gpioPWM(PWM_PIN, buffer[i]);  // Modula o sinal de áudio para o pino PWM
        time_sleep(0.0001);  // Ajuste conforme necessário para evitar distorção
    }

    free(buffer);
}

int main() {
    if (gpioInitialise() < 0) {
        fprintf(stderr, "Erro ao inicializar pigpio\n");
        return 1;
    }

    playWav("BoTemMel.wav");

    gpioPWM(PWM_PIN, 0);  // Desliga o PWM no final

    gpioTerminate();

    return 0;
}
