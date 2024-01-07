#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
#include <unistd.h>
#include <mpg123.h>
#include "../../ABElectronics_C_Libraries/ADCDACPi/ABE_ADCDACPi.h"

//sudo apt-get install libmpg123-dev
//gcc ../../ABElectronics_C_Libraries/ADCDACPi/ABE_ADCDACPi.c BoTemMel.c -o BoTemMel -lmpg123 -lpigpio -lrt -lpthread -lm
//sudo ./BoTemMel BoTemMel.mp3

#define BUFFER_SIZE 4096
#define DELAY_MICROSECONDS 1000 // Ajuste conforme necessário

void error(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo.mp3>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];

    int err = mpg123_init();
    if (err != MPG123_OK) {
        fprintf(stderr, "Falha ao inicializar o mpg123: %s\n", mpg123_plain_strerror(err));
        return EXIT_FAILURE;
    }

    mpg123_handle *mh = mpg123_new(NULL, &err);
    if (mh == NULL) {
        fprintf(stderr, "Falha ao criar o manipulador mpg123: %s\n", mpg123_plain_strerror(err));
        return EXIT_FAILURE;
    }

    err = mpg123_open(mh, filename);
    if (err != MPG123_OK) {
        fprintf(stderr, "Falha ao abrir o arquivo: %s\n", mpg123_plain_strerror(err));
        return EXIT_FAILURE;
    }

    long rate;
    int channels, encoding;
    mpg123_getformat(mh, &rate, &channels, &encoding);

    size_t buffer_size = BUFFER_SIZE;
    unsigned char *buffer = (unsigned char *)malloc(buffer_size);

    if (open_dac() != 1){ // open the DAC SPI channel
		exit(1); // if the SPI bus fails to open exit the program
	}

    set_dac_gain(1); // set the DAC gain to 2 which will give a voltage range of 0 to 2.048V

    size_t done;
    while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK) {
        for (size_t i = 0; i < done; i += 2) {
            float voltage = ((float)(buffer[i] << 8 | buffer[i + 1]) / 32768.0) * 2.048;
            set_dac_voltage(voltage, 1); // set the voltage on channel 1 to 1.2V
            usleep(DELAY_MICROSECONDS);
        }
    }

	
    free(buffer);
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();

    gpioTerminate();
    return 0;
}