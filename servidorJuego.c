#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>  
#include <mysql.h>

//Declaracion de funciones y metodos
int RegisterUser(char peticion_servidor[512])
{
	int respuesta_servidor;//variable que se usara para control de errores
	int codigo_peticion;
	char username[100];
	char password[100];
	char name[100];
	
	char* p = strtok(peticion_servidor, "/");
	int codigo = atoi(p);
	strcpy(username,strtok(NULL, "/"));
	strcpy(password,strtok(NULL, "/"));
	strcpy(name,strtok(NULL, "/"));
	
	MYSQL *conn;
	int err;
	// Estructura especial para almacenar resultados de consultas 
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	
	char consulta [80];//string para enviar la consulta a BBDD
	
	//Creamos una conexion al servidor MYSQL 
	conn = mysql_init(NULL);
	if (conn==NULL) {
		respuesta_servidor = 1;//regresar 1 porque hay error al crear la conexion
		printf ("Error al crear la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
	}
	//inicializar la conexion
	conn = mysql_real_connect (conn, "localhost","root", "mysql", "ProyectoSO",0, NULL, 0);
	if (conn==NULL) {
		respuesta_servidor=2;//error al inicializar la conexion
		printf ("Error al inicializar la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
	}
	
	// consulta SQL para obtener una tabla con todos los datos
	// de la base de datos
	//construir consulta sql
	sprintf(consulta,"INSERT INTO Jugador VALUES('%s','%s','%s');",username,password,name);
	// hacemos la consulta 
	err=mysql_query (conn, consulta); 
	if (err!=0) {
		respuesta_servidor = 3;//error al consultar datos 
		printf ("Error al consultar datos de la base %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}else if(err=0)
		respuesta_servidor=0;
	// cerrar la conexion con el servidor MYSQL 
	mysql_close (conn);
	//regresamos el resultado
	return respuesta_servidor;
}



int main(int argc, char *argv[])
{
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	
	char peticion[512];
	char peticion_servidor[512];
	char respuesta[512];
	int resultado;
	
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
	//La cola de peticiones pendientes no podr? ser superior a 4 Que quiere decir???
	if (listen(sock_listen, 4) < 0)
		printf("Error en el Listen");
	

	int server_listening=0;
	while(server_listening==0)
	{
		printf("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf("He recibido conexion\n");
		
		int terminar = 0;
		while(terminar==0)
		{
			//Recibimos peticiones
			ret = read(sock_conn, peticion, sizeof(peticion));
			printf("Recibida una petición\n");
			// Tenemos que agregar la marca de fin de string 
			// para que no escriba lo que hay despues en el buffer
			peticion[ret] = '\0';
			strcpy(peticion_servidor,peticion);
			char* p = strtok(peticion, "/");
			int codigo = atoi(p);
			//Escribimos la peticion en la consola
			if (codigo != 0){
				printf("La petición es: %s\n", peticion_servidor);
			}
			
			if (codigo == 0)
				terminar = 1;
			else if (codigo == 1){ //registrar un usuario
				resultado = RegisterUser(peticion_servidor);
				printf("el resultado regreso %d",resultado);
				if (resultado==0)
				{
					strcpy(respuesta, "Registrado Correctamente");
				}
				else
				{
					strcpy(respuesta, "Error en el registro");
				}
				
			}
			else if (codigo == 2)//iniciar sesion
			{
				
			}
			if (codigo !=0){
				// Enviamos la respuesta
				write(sock_conn,respuesta, strlen(respuesta));
			}
		}
		// Se acabo el servicio para este cliente
		close(sock_conn); 
	}
}
