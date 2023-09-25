#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void exibirInstrucoesDeUso(int argc, char **argv) {
    printf("Uso: %s <IP do servidor> <Porta do servidor>\n", argv[0]);
    printf("Exemplo: %s 127.0.0.1 90900\n", argv[0]);
    exit(EXIT_FAILURE);
}

static char *instrucao[100];


#define BUFFERSIZE 500

int main(int argc, char **argv) {

    if(argc < 3) 
        exibirInstrucoesDeUso(argc, argv);

    struct sockaddr_storage storage;
    if(parsearEndereco(argv[1], argv[2], &storage) != 0)
        exibirInstrucoesDeUso(argc, argv);
    
    int socket_;
    socket_ = socket(storage.ss_family, SOCK_STREAM, 0);
    if(socket_ == -1)
        exibirLogSaida("socket");
    
    struct sockaddr *endereco = (struct sockaddr *) (&storage);
    if(connect(socket_, endereco, sizeof(storage)) != 0)
        exibirLogSaida("connect");

    char addrstr[BUFFERSIZE];
    converterEnderecoEmString(endereco, addrstr, BUFFERSIZE);

    printf("connected to %s\n", addrstr);

    char buff[BUFFERSIZE];
    while(1){
	printf("> ");
	memset(buff, 0, BUFFERSIZE);
	size_t numBytes;
	while(buff[strlen(buff)-1] != '\n'){
	    memset(buff, 0, BUFFERSIZE);
	    fgets(buff, BUFFERSIZE-1, stdin);
	    numBytes = send(socket_, buff, strlen(buff), 0);
	    if (numBytes != strlen(buff)) {
	        exibirLogSaida("send");
   	    }
	}

	if(strcmp(buff, "kill\n") == 0){
	    close(socket_);
	    exit(EXIT_SUCCESS);
	}
	memset(buff, 0, BUFFERSIZE);
	unsigned totalBytes = 0;
	while(buff[strlen(buff)-1] != '\n') {
	    numBytes = recv(socket_, buff + totalBytes, BUFFERSIZE - totalBytes, 0);
	    if(numBytes == 0) {
	        break; // Fecha a conexão
	    }
	    totalBytes += numBytes;
	}
	printf("< %s", buff);
    }
    close(socket_);

    exit(EXIT_SUCCESS);
}


void TrataComando(char *comando){

	char *string[100];
	int posicao = 0;

	char *param = strtok(comando, "");

	while (param != NULL) {
		string[posicao] = param;// Armazena o param no array de string
        param = strtok(NULL, " ");// Avança para o próximo param
        posicao++;

		//install
		if (string[0] == "install") {

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
					sprintf(instrucao, "INS_REQ %d %d %d %d", sensorID, corrente, tensao, eficiencia);
					
					return instrucao;
               
			    } else {
                    printf("Erro ao ler os parâmetros do arquivo.\n");
                }

                fclose(arquivo);// Fecha o arquivo após a leitura

			// param
			} else if (string[1] == "param") {
				int numero; 

				if (sscanf(string[2], "%d", &numero) == 1) {
					if (posicao > 4) {
						int num1, num2, num3, num4;
						if (sscanf(string[posicao++], "%d", &num1) == 1 &&
							sscanf(string[posicao++], "%d", &num2) == 1 &&
							sscanf(string[posicao++], "%d", &num3) == 1 &&
							sscanf(string[posicao++], "%d", &num4) == 1) {

							int sensorID = string[2];    
							int corrente = string[3];
							int tensao = string[4];
							int eficiencia = string[5];

							sprintf(instrucao, "INS_REQ %d %d %d %d", sensorID, corrente, tensao, eficiencia);

						} else { //client kill
							printf("A instrução não contém quatro números válidos após string[1].\n");
						}

				return 0;
				}
			}

		}
	}

	//remove
	if (string[0] == "remove") {
			
		int sensorID, numero;

		// Lê os parâmetros da instrucao
		if (sscanf(string[1], "%d", &numero) == 1) {
					if (posicao > 4) {
						int num1;
						if (sscanf(string[posicao++], "%d", &num1) == 1) {

						int sensorID = string[1]; 

						// monta string para mandar para o server
						sprintf(instrucao, "REM_REQ %d", sensorID);
			
						return instrucao;
		
					} else {
						printf("Erro ao ler os parâmetros do arquivo.\n");
					}

		return 0;
		}
	}

	}
	}
	
