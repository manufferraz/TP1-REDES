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
static int posicao = 0;
static int sensorID = 0;
static int corrente = 0;
static int tensao = 0;
static int eficiencia = 0;


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
    printf("Exemplo: %s v4 90900\n", argv[0]);
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

    struct sockaddr_storage cstorage;
    struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
    socklen_t caddrlen = sizeof(cstorage);


    
        
        int socketCliente = accept(socket_, caddr, &caddrlen);
        if (socketCliente == -1) {
            exibirLogSaida("accept");
        }

//Aqui estão conectados


        char caddrstr[BUFSZ];
        converterEnderecoEmString(caddr, caddrstr, BUFSZ);
        printf("[log] connection from %s\n", caddrstr);

        while (1) {

        char buff[BUFSZ];
        int auxRetorno = 0;
        size_t numBytes;

        while(1){
            memset(buff, 0, BUFSZ);
            while(buff[strlen(buff)-1] != '\n') {
                numBytes = recv(socket_, buff, BUFSZ, 0);
                printf("%s", buff);

                if(numBytes == 0){
                    auxRetorno = -1;
                    break;
                }
            }
            if(auxRetorno == -1)
                break;

            printf("%s", buff);

            char *instrucao;
            instrucao = strtok(buff, " ");

            while(instrucao) {
                printf("token: %s\n", instrucao );
                instrucao = strtok(NULL, " ");
            }

            for (size_t i = 0; i < strlen(buff); i++){
               instrucao[i] = strtok(buff, " ");
            } 
            //instrucao = {"INS_REQ", "1", "2", "3", "4"}

            if(instrucao[0] == "INS_REC"){

                while (instrucao != NULL) {
                    if (posicao == 1) {
                        sscanf(instrucao, "%d", &sensorID);
                    } else if (posicao == 2) {
                        sscanf(instrucao, "%lf", &corrente);
                    } else if (posicao == 3) {
                        sscanf(instrucao, "%lf", &tensao);
                    } else if (posicao == 4) {
                        sscanf(instrucao, "%lf", &eficiencia);
                    }

                    instrucao = strtok(NULL, " ");
                    posicao++;
                } 

                instalarSensor(sensores, sensorID, corrente, tensao, eficiencia);
            }
            else if(instrucao[0] == "REM_REC"){

                while (instrucao != NULL) {
                    if (posicao == 1) {
                        sscanf(instrucao, "%d", &sensorID);
                    }

                    instrucao = strtok(NULL, " ");
                    posicao++;
                }

                desligarSensor(sensorID);
            }

            if(strcmp(buff, "kill") == 0){
                close(socketCliente);
                exit(EXIT_SUCCESS);
                return 0;
            }

            while(instrucao != NULL){
                if(auxRetorno == -1){
                            close(socketCliente);
                            exit(EXIT_SUCCESS);
                            return 0;
                        }
                        instrucao = strtok(NULL, " ");
                    }
                if(auxRetorno == -1)
                        break;
                }
                close(socketCliente);
            }

    exit(EXIT_SUCCESS);
}

