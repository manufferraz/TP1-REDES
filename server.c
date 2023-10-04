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

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;                           // estrutura de armazenamento p/ ipv6 ou ipv4
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage)) // parsing criada em common.c (útil tbm para server) do endereço para dentro da estrutura
    {
        usage(argc, argv);
    }

    // criação do socket
    int s = socket(storage.ss_family, SOCK_STREAM, 0); // CRIA SOCKET CONEXÃO INTERNET COM TCP (lib types e socket)
    if (s == -1)
    {
        logExit("socket");
    }

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) // reutilizar porto
    {
        logExit("setsocketopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage); // instanciação do endereço
    if (0 != bind(s, addr, sizeof(storage)))
    {
        logExit("bind");
    }

    if (0 != listen(s, 10))
    {
        logExit("listen");
    }

    char addrstr[BUFSZ];
    addrToStr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", addrstr);

    struct sockaddr_storage cstorage;
    struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
    socklen_t caddrlen = sizeof(cstorage);

    int csock = accept(s, caddr, &caddrlen);
    if (csock == -1)
    {
        logExit("accept");
    }

    char caddrstr[BUFSZ];
    addrToStr(caddr, caddrstr, BUFSZ);
    printf("[log] connection from %s\n", caddrstr);
    char *file_path;

    while (1)
    {
        char buf[BUFSZ];
        char *instrucao;

        memset(buf, 0, BUFSZ);

        // Recebe o comando do cliente
        ssize_t bytes_received = recv(csock, buf, BUFSZ, 0);
        if (bytes_received > 0)
        {
            buf[bytes_received] = '\0';

            instrucao = strtok(buf, " ");
            printf("%s", buf);

            // Verifica se o comando é "send file"
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

            if(strcmp(buf, "kill") == 0){
                close(csock);
                exit(EXIT_SUCCESS);
                return 0;
            }

    exit(EXIT_SUCCESS);
}
    }
