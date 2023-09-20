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
    double potencia;
    double eficiencia;
};


void funçãoGeralQueRecebeOComando(char *instrucao){
     // Variáveis para armazenar os tokens e a posição
     // ADICIONAR ERRO SE INSTRUÇÃO ESTIVER VAZIA!!!!!!!!!
    char *tokens[100]; // Um array de ponteiros para armazenar os tokens
    int posicao = 0;

    // Usamos o strtok para dividir a instrução em tokens com base nos espaços
    char *token = strtok(instrucao, " ");

    while (token != NULL) {
        
        tokens[posicao] = token;// Armazena o token no array de tokens
        token = strtok(NULL, " ");// Avança para o próximo token
        posicao++;// Incrementa a posição
    }

    // INS_REQ
        if (tokens[0] == "INS_REQ") {
            int numero;
            
            if (posicao > 1 && strstr(tokens[1], ".txt") != NULL) { // Verifica se o parâmetro é um arquivo .txt
                // Abre o arquivo e lê os parâmetros
                // chama a função de instalar sensor com os parâmetros
        } else if (posicao > 1 && sscanf(tokens[1], "%d", &numero) == 1) { // Verifica se os parâmetros são números
            //Verifica se a instrução tem 4 parâmetros
            if (posicao > 4) {
                int num1, num2, num3, num4;
                if (sscanf(tokens[2], "%d", &num1) == 1 &&
                    sscanf(tokens[3], "%d", &num2) == 1 &&
                    sscanf(tokens[4], "%d", &num3) == 1 &&
                    sscanf(tokens[5], "%d", &num4) == 1) {

                // chama a função de instalar sensor com os parâmetros
                } else {
                    printf("A instrução não contém quatro números válidos após tokens[1].\n");
                }
            } else {
                printf("A instrução não contém quatro números após tokens[1].\n");
            }
        } else {
            printf("O segundo token não é um número ou não existe.\n");
        }

        return 0;
    }
    }


void instalarSensor(struct Sensor sensores[], int numSensores, int sensorID) {
    int i;
    for (i = 0; i < numSensores; i++) {
        if (sensores[i].id == sensorID) {
            printf("sensor already exists\n");
            break; // Saímos do loop quando encontramos o sensor
        } else {
            // Calcula a potência ele´trica e adiciona o sensor à tabela 1
            //Imprime código 01
        }
    }

    if (i == numSensores) {
        printf("Sensor com o ID %d não encontrado.\n", sensorID);
    }
}


void consultarEquipamento(char *instrucao, int socketCliente) {
    // char msg[500];
    // memset(msg, 0, 500);
    // bool entrou = false;
    // char *texto = strtok(instrucao, " ");
    // while(strcmp(texto, "in") != 0) {
    //     texto = strtok(NULL, " ");
    // }
    // char *id = strtok(NULL, " ");
    // if(validaId(id) == -1) {
    //     strcat(msg, "invalid equipment ");
    // }
    // else {
    // int idEquipamento = atoi(id);
    // char idSensor[4];
    // for(int i = 0; i < 4; i++) {
    //     if(sistema[idEquipamento-1][i] == true) {
    //         sprintf(idSensor, "0%d ", i+1);
    //         strcat(msg, idSensor);
    //         entrou = true;
    //     }
    // }
    // if(!entrou)
    //     strcat(msg, "none ");
    // }
    // msg[strlen(msg)-1] = '\n';
    // int numBytes = send(socketCliente, msg, strlen(msg), 0);
    // if(numBytes != strlen(msg))
    //     exibirLogSaida("send");
}

void removerSensor(char *instrucao, int socketCliente) {
//     char msg[500];
//     memset(msg, 0, 500);
//     int sensores[MAX_SENSORES_POR_VEZ];
//     char *entrada = strtok(instrucao, " ");
//     entrada = strtok(instrucao, " ");
//     int numSensores = 0;
//     bool entradaInvalida = false;
//     while(strcmp(entrada, "in") != 0 && !entradaInvalida) {
//         if(validaId(entrada) == -1) {
//             entradaInvalida = true;
//             strcat(msg, "invalid sensor ");
//         }
//         else {
//             sensores[numSensores] = atoi(entrada);
//             numSensores++;
//         }
//         entrada = strtok(NULL, " ");
//     }
//     entrada = strtok(NULL, " ");
//     char msgSensorNaoExiste[100];
//     memset(msgSensorNaoExiste, 0, 100);
//     if(!entradaInvalida) {
//         if(validaId(entrada) == -1)
//             strcat(msg, "invalid equipment ");
//         else {
//             int equipamentoId = atoi(entrada);
//             strcat(msg, "sensor");
//             char idStr[4];
//             bool fezExclusao = false;
//             for(int i = 0; i < numSensores; i++) {
//                 if(sistema[equipamentoId-1][sensores[i]-1]) {
//                     sistema[equipamentoId-1][sensores[i]-1] = false;
//                     sprintf(idStr, " 0%d", sensores[i]);
//                     strcat(msg, idStr);
//                     qtdSensoresDisponiveis += 1;
//                     fezExclusao = true;
//                 }
//                 else {
//                     sprintf(idStr, "0%d ", sensores[i]);
//                     strcat(msgSensorNaoExiste, idStr);
//                 }
//             }
//             if(strcmp(msgSensorNaoExiste, "") != 0) {
//                 strcat(msgSensorNaoExiste, "does not exist in ");
//                 sprintf(idStr, "0%d ", equipamentoId);
//                 strcat(msgSensorNaoExiste, idStr);
//             }
//             if(strcmp(msgSensorNaoExiste, "") != 0 && fezExclusao)
//                 strcat(msg, " removed ");
//             else if(fezExclusao)
//                 strcat(msg, " removed ");
//             else
//                 strcat(msg, " ");
//             strcat(msg, msgSensorNaoExiste);
//         }
//     }
//     msg[strlen(msg)-1] = '\n';
//     int numBytes = send(socketCliente, msg, strlen(msg), 0);
//     if(numBytes != strlen(msg))
//         exibirLogSaida("send");
// }

// double gerarLeituraSensor() {
//     return (double)(rand() % 1001)/100;
}

void consultarVariavelDeProcesso(char *instrucao, int socketCliente) {
    // char msg[500];
    // memset(msg, 0, 500);
    // int sensores[MAX_SENSORES_POR_VEZ];
    // char *entrada = strtok(instrucao, " ");
    // int numSensores = 0;
    // bool entradaInvalida = false;
    // while(strcmp(entrada, "in") != 0 && !entradaInvalida) {
    //     if(validaId(entrada) == -1) {
    //         entradaInvalida = true;
    //         strcat(msg, "invalid sensor ");
    //     }
    //     else {
    //         sensores[numSensores] = atoi(entrada);
    //         numSensores++;
    //     }
    //     entrada = strtok(NULL, " ");
    // }
    // entrada = strtok(NULL, " ");
    // char msgSensorNaoInstalado[100];
    // memset(msgSensorNaoInstalado, 0, 100);
    // if(!entradaInvalida){
    //     if(validaId(entrada) == -1)
    //         strcat(msg, "invalid equipment ");
    //     else {
    //         int equipamentoId = atoi(entrada);
    //         char idStr[4];
    //         bool fezAlgumaLeitura = false;
    //         char leituraStr[6];
    //         for(int i = 0; i < numSensores; i++) {
    //             if(sistema[equipamentoId-1][sensores[i]-1]) {
    //                 sprintf(leituraStr, "%.2f ", gerarLeituraSensor());
    //                 strcat(msg, leituraStr);
    //                 fezAlgumaLeitura = true;
    //             }
    //             else {
    //                 sprintf(idStr, "0%d ", sensores[i]);
    //                 strcat(msgSensorNaoInstalado, idStr);
    //             }
    //         }
    //         if(fezAlgumaLeitura && strcmp(msgSensorNaoInstalado, "") != 0) {
    //             strcat(msg, "and ");
    //             strcat(msg, msgSensorNaoInstalado);
    //             strcat(msg, "not installed ");
    //         }
    //         else if(strcmp(msgSensorNaoInstalado, "") != 0) {
    //              strcpy(msg, msgSensorNaoInstalado);
    //              strcat(msg, "not installed ");
    //         }
    //     }
    // }
    // msg[strlen(msg)-1] = '\n';
    // int numBytes = send(socketCliente, msg, strlen(msg), 0);
    // if(numBytes != strlen(msg))
    //     exibirLogSaida("send");
}

int avaliarComando(char *comando, int socketCliente) {
    // char *instrucao = strtok(comando, " ");
    // while(instrucao != NULL) {
    //     if(strcmp(instrucao, "add") == 0) {
    //         instalarSensor(NULL, socketCliente);
    //     }
    //     else if(strcmp(instrucao, "remove") == 0) {
    //         removerSensor(NULL, socketCliente);
    //     }
    //     else if(strcmp(instrucao, "list") == 0) {
    //         consultarEquipamento(NULL, socketCliente);
    //     }
    //     else if(strcmp(instrucao, "read") == 0) {
    //         consultarVariavelDeProcesso(NULL, socketCliente);
    //     }
    //     else {
    //         close(socketCliente);
    //         return -1;
    //     }
    //     instrucao = strtok(NULL, " ");
    // }
    // return 0;
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

