#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORTA_COMUNICACAO 12345
#define PORTA_DESCOBERTA 12347

void simular_off_hook(int estado) {
    digitalWrite(PORTA_FXS_PINO, estado);
    printf("Simulando %s-hook\n", estado ? "off" : "on");
}

void simular_dtmf(char digit) {
    printf("Simulando discagem de DTMF: %c\n", digit);
}

void receber_dtmf() {
    // Lógica para receber DTMF do PC de Controle
    printf("Recebendo DTMF do PC de Controle\n");
    // Adicione a lógica específica para a recepção de DTMF
}

int main(void) {
    // Inicializar socket para comunicação
    int socket_desc;
    struct sockaddr_in server;

    // Criar o socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        fprintf(stderr, "Erro ao criar o socket\n");
        exit(EXIT_FAILURE);
    }

    // Configurar o endereço do servidor para descoberta
    struct sockaddr_in server_descoberta;
    server_descoberta.sin_family = AF_INET;
    server_descoberta.sin_addr.s_addr = INADDR_ANY;
    server_descoberta.sin_port = htons(PORTA_DESCOBERTA);

    // Criar socket para descoberta
    int socket_descoberta = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_descoberta == -1) {
        fprintf(stderr, "Erro ao criar o socket de descoberta\n");
        exit(EXIT_FAILURE);
    }

    // Associar o socket de descoberta ao endereço do servidor
    if (bind(socket_descoberta, (struct sockaddr *)&server_descoberta, sizeof(server_descoberta)) < 0) {
        fprintf(stderr, "Erro ao associar o socket de descoberta\n");
        exit(EXIT_FAILURE);
    }

    printf("Aguardando mensagem de descoberta...\n");

    char buffer[1024];
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);

    ssize_t bytes_received = recvfrom(socket_descoberta, buffer, sizeof(buffer), 0, (struct sockaddr *)&client, &client_len);

    if (bytes_received > 0) {
        // Enviar o IP de volta para o PC de Controle
        sendto(socket_descoberta, inet_ntoa(client.sin_addr), strlen(inet_ntoa(client.sin_addr)), 0, (struct sockaddr *)&client, client_len);
    }

    close(socket_descoberta);

    // Convertendo o buffer para o IP do PC de Controle
    char ip_pc_controle[16];
    strncpy(ip_pc_controle, buffer, sizeof(ip_pc_controle));
    ip_pc_controle[sizeof(ip_pc_controle) - 1] = '\0';

    // Preparar a estrutura sockaddr_in
    server.sin_addr.s_addr = inet_addr(ip_pc_controle);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORTA_COMUNICACAO);

    // Conectar ao PC de Controle
    if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        fprintf(stderr, "Erro ao conectar ao PC de Controle\n");
        exit(EXIT_FAILURE);
    }

    printf("Conectado ao PC de Controle\n");

    // Receber a função atribuída pelo PC de Controle
    char funcao[20];
    recv(socket_desc, funcao, sizeof(funcao), 0);
    printf("Função atribuída pelo PC de Controle: %s\n", funcao);

    // Lógica específica para o recetor ou transmissor
    if (strcmp(funcao, "recetor") == 0) {
        receber_dtmf();  // Lógica para receber DTMF
    } else if (strcmp(funcao, "transmissor") == 0) {
        // Lógica específica do transmissor
        simular_off_hook(1);  // Simular off-hook
        sleep(1);             // Aguardar um tempo para estabilização

        // Simular a discagem de números DTMF
        char numeros[] = "1234567890";
        for (int i = 0; numeros[i] != '\0'; ++i) {
            simular_dtmf(numeros[i]);
            sleep(1);  // Aguardar um tempo entre dígitos
        }

        // Simular o retorno ao estado "on-hook"
        simular_off_hook(0);
    }

    // Fechar a conexão com o PC de Controle
    close(socket_desc);

    return 0;
}

