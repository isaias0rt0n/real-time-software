/*  Programa de demonstracao de uso de sockets UDP em C no Linux
 *  Funcionamento:
 *  Usuario escolhe opcao no menu e entao envia uma msg para a caldeira.
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <pthread.h>
#include "socket.h"

#define FALHA 1

static int socket_local;

static struct sockaddr_in endereco_destino;

static pthread_mutex_t exclusao_socket = PTHREAD_MUTEX_INITIALIZER;

int cria_socket_local(void)
{
	int socket_local;		/* Socket usado na comunicacao */

	socket_local = socket( PF_INET, SOCK_DGRAM, 0);
	if (socket_local < 0) {
		perror("socket");
		return -1;
	}
	return socket_local;
}

struct sockaddr_in cria_endereco_destino(char *destino, int porta_destino)
{
	struct sockaddr_in servidor;	/* Endereco do servidor incluindo ip e porta */
	struct hostent *dest_internet;	/* Endereco destino em formato proprio */
	struct in_addr dest_ip;			/* Endereco destino em formato ip numerico */

	if (inet_aton ( destino, &dest_ip ))
		dest_internet = gethostbyaddr((char *)&dest_ip, sizeof(dest_ip), AF_INET);
	else
		dest_internet = gethostbyname(destino);

	if (dest_internet == NULL) {
		fprintf(stderr,"Endereco de rede invalido\n");
		exit(FALHA);
	}

	memset((char *) &servidor, 0, sizeof(servidor));
	memcpy(&servidor.sin_addr, dest_internet->h_addr_list[0], sizeof(servidor.sin_addr));
	servidor.sin_family = AF_INET;
	servidor.sin_port = htons(porta_destino);

	return servidor;
}


void envia_mensagem(int socket_local, struct sockaddr_in endereco_destino, char *mensagem)
{
	/* Envia msg ao servidor */

	if (sendto(socket_local, mensagem, strlen(mensagem)+1, 0, (struct sockaddr *) &endereco_destino, sizeof(endereco_destino)) < 0 )
	{
		perror("sendto");
		return;
	}
}


int recebe_mensagem(int socket_local, char *buffer, int TAM_BUFFER)
{
	int bytes_recebidos;		/* Numero de bytes recebidos */

	/* Espera pela msg de resposta do servidor */
	bytes_recebidos = recvfrom(socket_local, buffer, TAM_BUFFER, 0, NULL, 0);
	if (bytes_recebidos < 0)
	{
		perror("recvfrom");
	}

	return bytes_recebidos;
}

/*Metodo responsavel por criar uma conexao socket*/
void criar_socket(char *dst, int port_dst){
    pthread_mutex_lock(&exclusao_socket);

    socket_local = cria_socket_local();
    endereco_destino = cria_endereco_destino(dst, port_dst);

    pthread_mutex_unlock(&exclusao_socket);

}

/*Metodo para enviar mensagem via socket*/
double mensagem(char *msg){
    char msg_recebida[1000];
    int nrec;

    pthread_mutex_lock(&exclusao_socket);

    envia_mensagem(socket_local, endereco_destino, msg);
    nrec = recebe_mensagem(socket_local, msg_recebida, 1000);
    msg_recebida[nrec] = '\0';

    pthread_mutex_unlock(&exclusao_socket);

    return atof(&msg_recebida[3]);
}
