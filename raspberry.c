#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORTA_FXS_PINO 0  // Substitua pelo número do pino GPIO conectado à porta FXS
#define PORTA_COMUNICACAO 12345  // Número da porta para a comunicação

void simular_off_hook(int estado) {
    // Simular o levantamento ou desligamento do telefone (off-hook/on-hook)
    digitalWrite(PORTA_FXS_PINO, estado);
}

void simular_dtmf(char digit) {
    // Simular a discagem de um dígito DTMF
    // Pode envolver enviar os tons DTMF correspondentes
    printf("Enviando DTMF: %c\n", digit);
    // Lógica para enviar DTMF ao COIC aqui (substitua pelo seu código)
}

void receber_dtmf() {
    // Lógica para receber DTMF do COIC aqui (substitua pelo seu código)
    // Exemplo de leitura do pino GPIO conectado à porta FXS
    printf("Recebendo DTMF\n");
}

int main(void) {
    // Inicializar WiringPi
    if (wiringPiSetup() == -1) {
        fprintf(stderr, "Erro ao inicializar o WiringPi\n");
        exit(EXIT_FAILURE);
    }

    // Configurar o pino como saída/início ou entrada
    pinMode(PORTA_FXS_PINO, INPUT);

    // Inicializar socket para comunicação
    int socket_desc, client_sock, c;
    struct sockaddr_in server, client;
    
    // Criar o socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        fprintf(stderr, "Erro ao criar o socket\n");
        exit(EXIT_FAILURE);
    }

    // Preparar a estrutura sockaddr_in
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORTA_COMUNICACAO);

    // Associar
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        fprintf(stderr, "Erro ao associar o socket\n");
        exit(EXIT_FAILURE);
    }

    // Ouvir
    listen(socket_desc, 3);

    // Aguardar pela conexão do PC de controle
    printf("Aguardando conexão do PC de controle...\n");
    c = sizeof(struct sockaddr_in);
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0) {
        fprintf(stderr, "Erro ao aceitar a conexão do PC de controle\n");
        exit(EXIT_FAILURE);
    }

    printf("Conexão do PC de controle aceita\n");

    // Receber o comando do PC de controle
    char comando[256];
    recv(client_sock, comando, sizeof(comando), 0);

    // Lógica para interpretar o comando remoto e definir o papel (transmissor ou recetor)
    if (strcmp(comando, "definir_transmissor") == 0) {
        // Lógica específica para atuar como transmissor
        // ...

        // Exemplo: Simular o off-hook
        simular_off_hook(HIGH);
        sleep(1);  // Aguardar um tempo para a estabilização

        // Exemplo: Simular a discagem de números DTMF
        char numeros[] = "1234567890";
        for (int i = 0; numeros[i] != '\0'; ++i) {
            simular_dtmf(numeros[i]);
            sleep(1);  // Aguardar um tempo entre dígitos
        }

        // Exemplo: Simular o retorno ao estado "on-hook"
        simular_off_hook(LOW);
    } else if (strcmp(comando, "definir_recetor") == 0) {
        // Lógica específica para atuar como recetor
        // ...

        // Exemplo: Lógica para receber DTMF
        receber_dtmf();
    } else {
        fprintf(stderr, "Comando inválido do PC de controle\n");
    }

    // Fechar a conexão com o PC de controle
    close(client_sock);
    close(socket_desc);

    return 0;
}
