#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>  

int main(int argc, char *argv[])
{
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	// INICIALIZACIONES
	// Se abre el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creating socket");
	// Hacemos el bind al puerto
	memset(&serv_adr, 0, sizeof(serv_adr));// inicializa a zero serv_addr
	serv_adr.sin_family = AF_INET;
	// asocia el socket a cualquiera de las IP de la maquina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// escucharemos en el port 9050
	serv_adr.sin_port = htons(9081);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	//Podemos atender 4 clientes al mismo tiempo.Quiere decir que estaremos escuchando al mismo tiempo para 4 clientes
	if (listen(sock_listen, 4) < 0)
		printf("Error en el Listen");
	int i;
	
	printf("Escuchando\n");
	int terminar = 0;
	while(terminar==0)
	{
		sock_conn = accept(sock_listen, NULL, NULL);
		printf("He recibido conexion\n");
	}
}
