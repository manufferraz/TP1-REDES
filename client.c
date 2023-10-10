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

char* TrataInstall(char *instrucao[]) 
{
    if (strcmp(instrucao[1], "file") == 0) {
        // Abre o arquivo e lê os parâmetros
        FILE *arquivo;
        int sensorId, corrente, tensao, eficiencia;

        if (instrucao[2][strlen(instrucao[2]) - 1] == '0') {
            instrucao[2][strlen(instrucao[2]) - 1] = '\0';
        }
        
        arquivo = fopen(instrucao[2], "r");

        // Verifica se o arquivo foi aberto com sucesso
        if (arquivo == NULL) {
            printf("Erro ao abrir o arquivo.\n");
        }

        // Lê os quatro parâmetros do arquivo
        if (fscanf(arquivo, "%d %d %d %d", &sensorId, &corrente, &tensao, &eficiencia) == 4) {
            // Verifica se os parâmetros estão dentro das especificações
            if (corrente >= 0 && corrente <= 10 && tensao >= 0 && tensao <= 150 && eficiencia >= 0 && eficiencia <= 100) {
                // Cria o comando INS_REQ com base nos parâmetros lidos
                char *comando = (char *)malloc(BUFSZ); 
                snprintf(comando, BUFSZ, "INS_REQ %d %d %d %d", sensorId, corrente, tensao, eficiencia);
                fclose(arquivo);
                return comando;
            } else {
                printf("invalid sensor.\n");
            }
        } else {
            printf("invalid sensor.\n");
        }
        fclose(arquivo);

    } else if (strcmp(instrucao[1], "param") == 0) {
        int sensorId, cor, ten, efic_energ;
        sscanf(instrucao[2], "%d", &sensorId);
        sscanf(instrucao[3], "%d", &cor);
        sscanf(instrucao[4], "%d", &ten);
        sscanf(instrucao[5], "%d", &efic_energ);

        // Verifica se os parâmetros estão dentro das especificações
        if (cor >= 0 && cor <= 10 && ten >= 0 && ten <= 150 && efic_energ >= 0 && efic_energ <= 100) {
            // Cria o comando INS_REQ com base nos parâmetros
            char *comando = (char *)malloc(BUFSZ); 
            snprintf(comando, BUFSZ, "INS_REQ %d %d %d %d", sensorId, cor, ten, efic_energ);
            return comando;
        } else {
            printf("invalid sensor\n");
        }
    } 

    return NULL;
}


char* TrataRemove(char *instrucao[])
{
        int sensorId;
        sscanf(instrucao[1], "%d", &sensorId);

        // Cria o comando REM_REQ com base nos parâmetros
        char *comando = (char *)malloc(BUFSZ); 
        snprintf(comando, BUFSZ, "REM_REQ %d", sensorId);
        return comando;
}

char* TrataShowSensor(char *instrucao[])
{
        int sensorId;
        sscanf(instrucao[2], "%d", &sensorId);

        // Cria o comando SEN_REQ com base nos parâmetros
        char *comando = (char *)malloc(BUFSZ); 
        snprintf(comando, BUFSZ, "SEN_REQ %d", sensorId);
        return comando;
}

char* TrataShowValues(char *instrucao[])
{
        char *comando = (char *)malloc(BUFSZ); 
        snprintf(comando, BUFSZ, "VAL_REQ");
        return comando;
}

char* TrataChange(char *instrucao[]) 
{
    if (strcmp(instrucao[1], "file") == 0) {
        // Abre o arquivo e lê os parâmetros
        FILE *arquivo;
        int sensorId, corrente, tensao, eficiencia;

        if (instrucao[2][strlen(instrucao[2]) - 1] == '0') {
            instrucao[2][strlen(instrucao[2]) - 1] = '\0';
        }
        
        arquivo = fopen(instrucao[2], "r");

        // Verifica se o arquivo foi aberto com sucesso
        if (arquivo == NULL) {
            printf("Erro ao abrir o arquivo.\n");
            return NULL;
        }

        // Lê os quatro parâmetros do arquivo
        if (fscanf(arquivo, "%d %d %d %d", &sensorId, &corrente, &tensao, &eficiencia) == 4) {
            // Verifica se os parâmetros estão dentro das especificações
            if (corrente >= 0 && corrente <= 10 && tensao >= 0 && tensao <= 150 && eficiencia >= 0 && eficiencia <= 100) {
                // Cria o comando CH_REQ com base nos parâmetros lidos
                char *comando = (char *)malloc(BUFSZ); 
                snprintf(comando, BUFSZ, "CH_REQ %d %d %d %d", sensorId, corrente, tensao, eficiencia);
                fclose(arquivo);
                return comando;
            } else {
                printf("invalid sensor.\n");
            }
        } else {
            printf("Erro ao ler os parametros do arquivo.\n");
        }
        fclose(arquivo);

    } else if (strcmp(instrucao[1], "param") == 0) {
        int sensorId, corrente, tensao, eficiencia;

        // Verifique se os argumentos têm o formato correto
        if (sscanf(instrucao[2], "%d", &sensorId) != 1 ||
            sscanf(instrucao[3], "%d", &corrente) != 1 ||
            sscanf(instrucao[4], "%d", &tensao) != 1 ||
            sscanf(instrucao[5], "%d", &eficiencia) != 1) {
            printf("invalid sensor\n");
            return NULL;
        }

        // Cria o comando CH_REQ com base nos parâmetros
        char *comando = (char *)malloc(BUFSZ); 
        if (comando != NULL) {
            snprintf(comando, BUFSZ, "CH_REQ %d %d %d %d", sensorId, corrente, tensao, eficiencia);
        }

        return comando;
        }
    return NULL;
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
    char string[BUFSZ]; // Variável para armazenar a string recebida
    char mensagem[BUFSZ]; // Variável para armazenar a mensagem recebida

    memset(buf, 0, BUFSZ);

    // Recebe o instrução do terminal
    fgets(buf, sizeof(buf), stdin);
    strncpy(string, buf, sizeof(string) - 1);

    bzero(buf, strlen(buf)); // Limpa o buffer

    // Remova a quebra de linha do final da string
    string[strcspn(string, "\n")] = '\0';

    // Processa a instrução e cria um array de elementos
    char *instrucao[BUFSZ];
    int numTokens = 0;

    char temp[BUFSZ];
    strcpy(temp, string); 
    char *token = strtok(temp, " ");
    while (token != NULL) {
        instrucao[numTokens++] = token;
        token = strtok(NULL, " ");
    }

    // Verifica se a primeira posição do array é igual a "install"
    if (numTokens > 0 && strcmp(instrucao[0], "install") == 0) {
        if (strcmp(instrucao[1], "param") != 0 && strcmp(instrucao[1], "file") != 0) {
            close(s);
            printf("Invalid command.\n");
            exit(EXIT_SUCCESS);
        } else {
            char *comando = TrataInstall(instrucao);
                if (comando != NULL) {
                    send(s, comando, strlen(comando), 0);
                    free(comando);
            }
        }
    } else if (numTokens > 0 && strcmp(instrucao[0], "remove") == 0) {
       char *comando = TrataRemove(instrucao);
        if (comando != NULL) {
            send(s, comando, strlen(comando), 0);
            free(comando);
        }
    } else if (numTokens > 0 && strcmp(instrucao[0], "show") == 0) {
        if (!strcmp(instrucao[1], "values")) {
            char *comando = TrataShowValues(instrucao);
            if (comando != NULL) {
                send(s, comando, strlen(comando), 0);
                free(comando);
            }
        } else if (!strcmp(instrucao[1], "value")) {
            char *comando = TrataShowSensor(instrucao);
            if (comando != NULL) {
                send(s, comando, strlen(comando), 0);
                free(comando);
            }
        } else {
            close(s);
            printf("Invalid command.\n");
            exit(EXIT_SUCCESS);
        }
    } else if (numTokens > 0 && strcmp(instrucao[0], "change") == 0) {
       if (strcmp(instrucao[1], "param") != 0 && strcmp(instrucao[1], "file") != 0) {
            close(s);
            printf("Invalid command.\n");
            exit(EXIT_SUCCESS);
        } else {
            char *comando = TrataChange(instrucao);
                if (comando != NULL) {
                    send(s, comando, strlen(comando), 0);
                    free(comando);
            }
        }
    } else if (strcmp(string, "kill") == 0) {
        send(s, string, strlen(string), 0);
        close(s);
        printf("Servidor encerrado pelo cliente.\n");
        exit(EXIT_SUCCESS);
    } else {
        close(s);
        printf("Invalid command.\n");
        exit(EXIT_SUCCESS);
    }

    bzero(buf, strlen(buf));

    // Recebe a mensagem do servidor
    ssize_t bytes_received = recv(s, buf, BUFSZ, 0);

    if (bytes_received > 0) {
        // Copia o conteúdo recebido para 'mensagem'
        strncpy(mensagem, buf, sizeof(mensagem) - 1);
        mensagem[strcspn(mensagem, "\n")] = '\0';

        printf("mensagem recebida: %s\n", mensagem);
    }

}
	close(s);
    return 0;
    }




