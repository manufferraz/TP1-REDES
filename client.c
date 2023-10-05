#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSZ 1024

void usage(int argc, char **argv)
{
    printf("usage: %s <server ip> <server port>\n", argv[0]);
    printf("example: %s 127.0.0.1 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

void TrataInstall(char *string) 
{
    if (string[1] == "file") {
        // Abre o arquivo e lê os parâmetros
        FILE *arquivo;
        int sensorID, corrente, tensao, eficiencia;

        arquivo = fopen(string[2], "r");

        // Verifica se o arquivo foi aberto com sucesso
        if (arquivo == NULL) {
            printf("Erro ao abrir o arquivo.\n");
            return 1;
        }

        // Lê os três parâmetros do arquivo
        if (fscanf(arquivo, "%d %d %d %d", &sensorID, &corrente, &tensao, &eficiencia) == 4) {
            printf("O arquivo está completo. \n");
            // monta string para mandar para o server
            snprintf(instrucao, BUFSZ, "INS_REQ %d %d %d %d", sensorID, corrente, tensao, eficiencia);

            return instrucao;
        } else {
            printf("Erro ao ler os parâmetros do arquivo.\n");
        }

        fclose(arquivo); // Fecha o arquivo após a leitura
    } else if (string[1] == "param") {
        int num1 = 0;

        if (sscanf(string[2], "%d", &num1) == 1) {
            int num2, num3, num4;
            if (sscanf(string[3], "%d %d %d %d", &num2, &num3, &num4) == 4) {
                int sensorID = num1;
                int corrente = num2;
                int tensao = num3;
                int eficiencia = num4;

                sprintf(instrucao, "INS_REQ %d %d %d %d", sensorID, corrente, tensao, eficiencia);

                //printf("%s", instrucao);
                return instrucao;
            } else { // client kill
                printf("A instrução não contém quatro números válidos após comando[1].\n");
            }
        }
    }
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;                // estrutura de armazenamento p/ ipv6 ou ipv4
    if (0 != addrParse(argv[1], argv[2], &storage)) // parsing criada em common.c (útil tbm para server) do endereço para dentro da estrutura
    {
        usage(argc, argv);
    }

    // criação do socket
    int s = socket(storage.ss_family, SOCK_STREAM, 0); // CRIA SOCKET CONEXÃO INTERNET COM TCP (lib types e socket)
    if (s == -1)
    {
        logExit("socket");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage); // instanciação do endereço
    if (0 != connect(s, addr, sizeof(storage)))            // CONECTA NO SOCKET
    {
        logExit("connect");
    }

    char addrstr[BUFSZ];
    // Configurar o socket 's' e conectar ao servidor

    int selected = 0;

    while (1) {
        char buf[BUFSZ];
        char instrucao[BUFSZ]; // Variável para armazenar a instrução recebida

        memset(buf, 0, BUFSZ);

        // Recebe o comando do cliente
        ssize_t bytes_received = recv(csock, buf, BUFSZ, 0);
        if (bytes_received > 0) {

            strncpy(instrucao, buf, sizeof(instrucao) - 1);
            instrucao[sizeof(instrucao) - 1] = '\0'; // Certifique-se de terminar a string

            printf("Instrução recebida: %s\n", instrucao);

            if (strcmp(instrucao, "kill") == 0) {
                close(csock);
                printf("Servidor encerrado pelo cliente.\n");
                exit(EXIT_SUCCESS);
            }

            // Processa a instrução e cria um array de elementos
            char *string[BUFSZ];
            int numTokens = 0;

            // Usa strtok para dividir a instrução em tokens com base nos espaços
            char *token = strtok(instrucao, " ");
            while (token != NULL) {
                string[numTokens++] = token;
                token = strtok(NULL, " ");
            }

            // Agora 'tokens' contém os elementos separados da instrução
            char *comando;
			if (strcmp(string[0], "install") == 0) {
				snprintf(comando, strlen(comando), TrataInstall(string)); //OLHAR ISSO
				send(s, comando, strlen(comando), 0);
				}
        }
    }

	close(s);
    return 0;
}