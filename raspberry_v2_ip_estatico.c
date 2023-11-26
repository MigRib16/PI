// shared_code.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORTA_COMUNICACAO 12345
#define TAMANHO_MAX_VARIAVEL 100

void simular_off_hook() {
    printf("Telefone fora do gancho (Off-hook)\n");
}

void simular_on_hook() {
    printf("Telefone no gancho (On-hook)\n");
}

void simular_dial_tone() {
    printf("Dial tone presente\n");
}

void simular_dtmf_transmissor(char dtmf_digit) {
    printf("Transmitindo DTMF: %c\n", dtmf_digit);
}

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
    (argc < 2) {
        fprintf(stderr, "Uso: %s <transmissor ou recetor> [variaveis...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "transmissor") == 0) {
        transmissor(argc, argv);
    } else if (strcmp(argv[1], "recetor") == 0) {
        recetor();
    } else {
        fprintf(stderr, "Argumento inválido. Use 'transmissor' ou 'recetor'.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
