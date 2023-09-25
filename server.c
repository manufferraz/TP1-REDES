#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 500

struct Sensor {
    int id;
    int potencia;
    int eficiencia;
};

static struct Sensor sensores[100];
static int *numSensores = sizeof(sensores) / sizeof(sensores[0]); // Calcula o número de sensores



void RecebeInstrucao(char *instrucao){

    // Usamos o strtok para dividir a instrução em strings com base nos espaços
    char *param = strtok(instrucao, " ");

    int posicao = 0;

    int sensorID = 0;
    int corrente = 0;
    int tensao = 0;
    int eficiencia = 0;

    if(instrucao[0] == "INS_REC"){

        while (param != NULL) {
            if (posicao == 1) {
                sscanf(param, "%d", &sensorID);
            } else if (posicao == 2) {
                sscanf(param, "%lf", &corrente);
            } else if (posicao == 3) {
                sscanf(param, "%lf", &tensao);
            } else if (posicao == 4) {
                sscanf(param, "%lf", &eficiencia);
            }

            param = strtok(NULL, " ");
            posicao++;
        }

        instalarSensor(sensores, sensorID, corrente, tensao, eficiencia);
    }
    else if(instrucao[0] == "REM_REC"){

        while (param != NULL) {
            if (posicao == 1) {
                sscanf(param, "%d", &sensorID);
            }

            param = strtok(NULL, " ");
            posicao++;
        }

        desligarSensor(sensorID);
    }

}



void instalarSensor(struct Sensor sensores[], int sensorID, int corrente, int tensao, int eficiencia) {
    int i;
    const int MAX_SENSORES = 100; 

    
    for (i = 0; i < numSensores; i++) {
        if (sensores[i].id == sensorID) {

            printf("sensor already exists\n");
            
            break; // Saímos do loop quando encontramos o sensor
        
        } else {
            printf("sensor not installed\n");

              // Calcula a potência elétrica e adiciona o sensor à tabela 1
              const int potencia = tensao * corrente;

              //Adiciona o sensor à struct
              if (*numSensores < MAX_SENSORES) {
                sensores[*numSensores].id = sensorID;
                sensores[*numSensores].potencia = potencia;
                sensores[*numSensores].eficiencia = eficiencia;
                (*numSensores)++; // Incrementa o número de sensores na struct
                printf("successful installation\n");
            } else {
                return 1;
            }

        }
    }
}

void desligarSensor(int sensorID){
    for (int i = 0; i < numSensores; i++) {
        if (sensores[i].id == sensorID) {

            for (int j = i; j < *numSensores - 1; j++) {
                sensores[j] = sensores[j + 1];
            }

            (*numSensores)--; // Decrementa o número de sensores

            printf("successful removal\n");
        }

        }
        printf("sensor not installed\n"); //COMO MANDAR A MENSAGEM DE ERRO PARA A RTU?
    }



void exibirInstrucoesDeUso(int argc, char **argv) {
    printf("Uso: %s <v4|v6> <Porta do servidor>\n", argv[0]);
    printf("Exemplo: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    srand((unsigned) time(NULL));
    if (argc < 3) {
        exibirInstrucoesDeUso(argc, argv);
    }

    struct sockaddr_storage storage;
    if (0 != inicializarSockAddrServer(argv[1], argv[2], &storage)) {
        exibirInstrucoesDeUso(argc, argv);
    }

    int socket_;
    socket_ = socket(storage.ss_family, SOCK_STREAM, 0);
    if (socket_ == -1) {
        exibirLogSaida("socket");
    }

    int enable = 1;
    if (0 != setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        exibirLogSaida("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(socket_, addr, sizeof(storage))) {
        exibirLogSaida("bind");
    }

    if (0 != listen(socket_, 10)) {
        exibirLogSaida("listen");
    }

    char addrstr[BUFSZ];
    converterEnderecoEmString(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", addrstr);

    while (1) {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int socketCliente = accept(socket_, caddr, &caddrlen);
        if (socketCliente == -1) {
            exibirLogSaida("accept");
        }

        char caddrstr[BUFSZ];
        converterEnderecoEmString(caddr, caddrstr, BUFSZ);
        printf("[log] connection from %s\n", caddrstr);

        char buff[BUFSZ];
        int auxRetorno = 0;
        unsigned totalBytes;
        size_t numBytes;
        char *linhas;
        while(1){
            memset(buff, 0, BUFSZ);
            totalBytes = 0;
            while(buff[strlen(buff)-1] != '\n') {
                numBytes = recv(socketCliente, buff + totalBytes, BUFSZ - totalBytes, 0);
                if(numBytes == 0){
                    auxRetorno = -1;
                    break;
                }
                totalBytes += numBytes;
            }
            if(auxRetorno == -1)
                break;
            printf("%s", buff);
            linhas = strtok(buff, "\n");
            if(strcmp(buff, "kill") == 0){
                close(socketCliente);
                exit(EXIT_SUCCESS);
                return 0;
            }

            while(linhas != NULL){
		auxRetorno = avaliarComando(buff, socketCliente);
		if(auxRetorno == -1){
                    close(socketCliente);
                    exit(EXIT_SUCCESS);
                    return 0;
                }
                linhas = strtok(NULL, "\n");
            }
	    if(auxRetorno == -1)
                break;
        }
        close(socketCliente);
    }

    exit(EXIT_SUCCESS);
}

