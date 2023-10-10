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
    int efic_energ;
};

static struct Sensor sensores[100];
static int numSensores = 0; // Inicialmente, não há sensores instalados

char* instalarSensor(int sensorId, int cor, int ten, int efic_energ) {
    const int MAX_SENSORES = sizeof(sensores) / sizeof(sensores[0]);
    char *mensagem = (char *)malloc(50); // Aloca memória para a mensagem

    // Verifica se o sensor já está instalado
    for (int i = 0; i < numSensores; i++) {
        if (sensores[i].id == sensorId) {
            strcpy(mensagem, "sensor already installed");
            return; // Sai da função sem fazer nada, pois o sensor já existe
        }
    }

    // Calcula a potência elétrica
    const int potencia = ten * cor;

    // Adiciona o sensor à struct se houver espaço
    if (numSensores < MAX_SENSORES) {
        sensores[numSensores].id = sensorId;
        sensores[numSensores].potencia = potencia;
        sensores[numSensores].efic_energ = efic_energ;
        numSensores++; // Incrementa o número de sensores na struct
        strcpy(mensagem, "sensor successfully installed");
    } else {
        printf("maximum number of sensors reached\n");
    }

    printf("%s\n", mensagem);
    
    return mensagem;
}


char* desligarSensor(int sensorId)
{
    char *mensagem = (char *)malloc(50); // Aloca memória para a mensagem
    bool sensorEncontrado = false; // Variável para rastrear se o sensor foi encontrado
    
    // Verifica se o sensor já está instalado
    for (int i = 0; i < numSensores; i++) {
        if (sensores[i].id == sensorId) {
            sensorEncontrado = true; // Sensor encontrado
            
            for (int j = i; j < numSensores - 1; j++) {
                sensores[j] = sensores[j + 1];
            }
            
            numSensores--;
        }
    }
    
    if (sensorEncontrado) {
        strcpy(mensagem, "successful removal");
    } else {
        strcpy(mensagem, "sensor not installed");
    }

    printf("%s\n", mensagem);
    
    return mensagem;
}

char* atualizarSensor(int sensorId, int cor, int ten, int efic_energ) {
    const int MAX_SENSORES = sizeof(sensores) / sizeof(sensores[0]);

    char *mensagem = (char *)malloc(BUFSZ); 
    bool sensorEncontrado = false; // Variável para rastrear se o sensor foi encontrado

    // Procura o sensor pelo sensorId
    for (int i = 0; i < numSensores; i++) {
        if (sensores[i].id == sensorId) {
            // Atualiza os parâmetros do sensor
            sensores[i].potencia = ten * cor;
            sensores[i].efic_energ = efic_energ;
            strcpy(mensagem, "successful change");
            sensorEncontrado = true;
            break; 
        }
    }

    // Se o sensorId não foi encontrado, define a mensagem de erro
    if (!sensorEncontrado) {
        strcpy(mensagem, "sensor not installed");
    }

    printf("%s\n", mensagem);

    return mensagem;
}


char* MostrarSensor(int sensorId) {
    char *mensagem = (char *)malloc(50); 
    bool sensorEncontrado = false; // Variável para rastrear se o sensor foi encontrado
    
    // Verifica se o sensor já está instalado
    for (int i = 0; i < numSensores; i++) {
        if (sensores[i].id == sensorId) {
            sensorEncontrado = true; // Sensor encontrado
            snprintf(mensagem, 50, "sensor %d %d %d", sensores[i].id, sensores[i].potencia, sensores[i].efic_energ);
            break; 
        }
    }
    
    if (!sensorEncontrado) {
        strcpy(mensagem, "sensor not installed");
    }

    printf("%s\n", mensagem);
    
    return mensagem;
}

char* MostrarSensores() {
    char *mensagem = (char *)malloc(512); 
    bool sensoresEncontrados = false; // Variável para rastrear se sensores foram encontrados
    
    mensagem[0] = '\0'; 
    // Verifica se há sensores instalados
    if (numSensores > 0) {
        sensoresEncontrados = true;
        
        // Percorre a lista de sensores e adiciona as informações à mensagem
        for (int i = 0; i < numSensores; i++) {
            char sensorInfo[50];
            snprintf(sensorInfo, sizeof(sensorInfo), "%d (%d %d) ", sensores[i].id, sensores[i].potencia, sensores[i].efic_energ);
            strcat(mensagem, sensorInfo);
        }
    }
    
    if (!sensoresEncontrados) {
        strcpy(mensagem, "Nenhum sensor instalado");
    }

    printf("%s\n", mensagem);
    
    return mensagem;
}



void usage(int argc, char **argv)
{
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
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

    int csock;

    char addrstr[BUFSZ];
    addrToStr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", addrstr);

    struct sockaddr_storage cstorage;
    struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
    socklen_t caddrlen = sizeof(cstorage);

    int flag = 0;

     while (1) {
        if(flag == 0){
        csock = accept(s, caddr, &caddrlen);
            if (csock == -1)
            {
                logExit("accept");
        }

        char caddrstr[BUFSZ];
        addrToStr(caddr, caddrstr, BUFSZ);
        printf("[log] connection from %s\n", caddrstr);

        flag = 1;
        }

        char buf[BUFSZ];
        char comando[BUFSZ]; // Variável para armazenar a instrução recebida

        memset(buf, 0, BUFSZ);

        // Recebe o comando do cliente
        // bzero(buf);
        ssize_t bytes_received = recv(csock, buf, BUFSZ, 0);

        if (bytes_received == 0 ){
            flag = 0;
        } else if (bytes_received > 0) {
            // Copia o conteúdo recebido para 'comando'
            strncpy(comando, buf, sizeof(comando) - 1);
            comando[strcspn(comando, "\n")] = '\0';

            printf("Comando recebido: %s\n", comando);

            // Processa a instrução e cria um array de elementos
            char *aux[BUFSZ];
            int numTokens = 0;

            char temp[BUFSZ];
            strcpy(temp, comando); 
            char *token = strtok(temp, " ");
            while (token != NULL) {
                aux[numTokens++] = token;
                token = strtok(NULL, " ");
            }

            // Agora 'comando' contém os elementos separados da instrução
            if (numTokens > 0 && strcmp(aux[0], "INS_REQ") == 0) {                
                int sensorId, cor, ten, efic_energ;

                for (int i = 1; i < numTokens; i++) {
                    if (i == 1) {
                        sscanf(aux[i], "%d", &sensorId);
                    } else if (i == 2) {
                        sscanf(aux[i], "%d", &cor);
                    } else if (i == 3) {
                        sscanf(aux[i], "%d", &ten);
                    } else if (i == 4) {
                        sscanf(aux[i], "%d", &efic_energ);
                    }
                }
                char *mensagem =  instalarSensor(sensorId, cor, ten, efic_energ);
                if (comando != NULL) {
                    send(csock, mensagem, strlen(mensagem), 0);
                    free(mensagem);
                }
            } else if (numTokens > 0 && strcmp(aux[0], "REM_REQ") == 0) {                
                int sensorId;

                for (int i = 1; i < numTokens; i++) {
                    if (i == 1) {
                        sscanf(aux[i], "%d", &sensorId);
                    }
                }

                char *mensagem = desligarSensor(sensorId);
                if (comando != NULL) {
                    send(csock, mensagem, strlen(mensagem), 0);
                    free(mensagem);
                }
            }else if (numTokens > 0 && strcmp(aux[0], "CH_REQ") == 0) {                
                int sensorId, cor, ten, efic_energ;

                for (int i = 1; i < numTokens; i++) {
                    if (i == 1) {
                        sscanf(aux[i], "%d", &sensorId);
                    } else if (i == 2) {
                        sscanf(aux[i], "%d", &cor);
                    } else if (i == 3) {
                        sscanf(aux[i], "%d", &ten);
                    } else if (i == 4) {
                        sscanf(aux[i], "%d", &efic_energ);
                    }
                }
                char *mensagem = atualizarSensor(sensorId, cor, ten, efic_energ);
                if (comando != NULL) {
                    send(csock, mensagem, strlen(mensagem), 0);
                    free(mensagem);
                }
            } else if (numTokens > 0 && strcmp(aux[0], "SEN_REQ") == 0) {                
                int sensorId;

                for (int i = 1; i < numTokens; i++) {
                    if (i == 1) {
                        sscanf(aux[i], "%d", &sensorId);
                    }
                }

                char *mensagem = MostrarSensor(sensorId);
                if (comando != NULL) {
                    send(csock, mensagem, strlen(mensagem), 0);
                    free(mensagem);
                }
            } else if (numTokens > 0 && strcmp(aux[0], "VAL_REQ") == 0) {               

                char *mensagem = MostrarSensores();
                if (comando != NULL) {
                    send(csock, mensagem, strlen(mensagem), 0);
                    free(mensagem);
                }
            } else if (strcmp(comando, "kill") == 0) {
                close(csock);
                printf("Servidor encerrado pelo cliente.\n");
                exit(EXIT_SUCCESS);
            } 

        }
    }
    close(csock);
    return 0;
}
