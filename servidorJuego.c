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
int RegisterUser(char peticion_servidor[512])//Funcion para agregar jugadores
{
	int respuesta_servidor_sql;//variable que se usara para control de errores
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
	
	char consulta [150];//string para enviar la consulta a BBDD
	
	//Creamos una conexion al servidor MYSQL 
	conn = mysql_init(NULL);
	if (conn==NULL) {
		respuesta_servidor_sql = 1;//regresar 1 porque hay error al crear la conexion
		printf ("Error al crear la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
	}
	//inicializar la conexion
	conn = mysql_real_connect (conn, "localhost","root", "mysql", "ProyectoSO",0, NULL, 0);
	if (conn==NULL) {
		respuesta_servidor_sql=2;//error al inicializar la conexion
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
		respuesta_servidor_sql = 3;//error al consultar datos 
		printf ("Error al consultar datos de la base %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}else if(err==0)
		respuesta_servidor_sql=0;
	// cerrar la conexion con el servidor MYSQL 
	mysql_close (conn);
	//regresamos el resultado
	return respuesta_servidor_sql;
}

int Login(char peticion_servidor[512])//Funcion para verificar credenciales del jugador e iniciar sesion
{
	int respuesta_servidor_sql;//variable que se usara para control de errores
	int codigo_peticion;
	char username[100];
	char password[100];
	char db_username[100];
	char db_password[100];
	int rescmp1;
	int rescmp2;
	
	char* p = strtok(peticion_servidor, "/");
	int codigo = atoi(p);
	strcpy(username,strtok(NULL, "/"));
	strcpy(password,strtok(NULL, "/"));
	
	MYSQL *conn;
	int err;
	// Estructura especial para almacenar resultados de consultas 
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	
	char consulta [150];//string para enviar la consulta a BBDD
	
	//Creamos una conexion al servidor MYSQL 
	conn = mysql_init(NULL);
	if (conn==NULL) {
		respuesta_servidor_sql = 1;//regresar 1 porque hay error al crear la conexion
		printf ("Error al crear la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
	}
	//inicializar la conexion
	conn = mysql_real_connect (conn, "localhost","root", "mysql", "ProyectoSO",0, NULL, 0);
	if (conn==NULL) {
		respuesta_servidor_sql=2;//error al inicializar la conexion
		printf ("Error al inicializar la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
	}
	
	// consulta SQL para obtener una tabla con todos los datos
	// de la base de datos
	//construir consulta sql	
	sprintf(consulta,"SELECT * FROM Jugador WHERE Username='%s';",username);
	// hacemos la consulta 
	err=mysql_query (conn, consulta); 
	if (err!=0) {
		respuesta_servidor_sql = 3;//error al consultar datos 
		printf ("Error al consultar datos de la base %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}else if(err==0)
	{
		//recogemos el resultado de la consulta 
		resultado = mysql_store_result (conn); 
		row = mysql_fetch_row (resultado);
	}
	if (row == NULL)
	{
		printf ("No se han obtenido datos en la consulta\n");
		respuesta_servidor_sql=5;
	}
	else
	{
		strcpy(db_username,row[0]);
		strcpy(db_password,row[1]);
		rescmp1 = strcmp(username,db_username);
		rescmp2 = strcmp(password, db_password);
		
		// El resultado debe ser una matriz con una sola fila
		// una columna con Username y una columna con Passwd y una con el nombre, esta ultima no se usa aqui
		if ((rescmp1==0)&&(rescmp2==0))
			respuesta_servidor_sql=0;//exito
		else if((rescmp1==0)&&(rescmp2!=0))
			respuesta_servidor_sql=4;//4 si la contrasena es incorrecta pero el usuario esta bien
	}
	// cerrar la conexion con el servidor MYSQL 
	mysql_close (conn);
	//regresamos el resultado
	return respuesta_servidor_sql;
}


int CountGames(char username[100])//funcion para ver el total de partidas jugadas
{
	char respuesta_cuenta[20];
	int respuesta_servidor_sql;//variable que se usara para control de errores
	
	MYSQL *conn;
	int err;
	// Estructura especial para almacenar resultados de consultas 
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	
	char consulta [150];//string para enviar la consulta a BBDD
	
	//Creamos una conexion al servidor MYSQL 
	conn = mysql_init(NULL);
	if (conn==NULL) {
		respuesta_servidor_sql = 1;//regresar 1 porque hay error al crear la conexion
		printf ("Error al crear la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
	}
	//inicializar la conexion
	conn = mysql_real_connect (conn, "localhost","root", "mysql", "ProyectoSO",0, NULL, 0);
	if (conn==NULL) {
		respuesta_servidor_sql=2;//error al inicializar la conexion
		printf ("Error al inicializar la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
	}
	
	// consulta SQL para obtener una tabla con todos los datos
	// de la base de datos
	//construir consulta sql	
	
	sprintf(consulta,"SELECT COUNT(Games.Game_ID) FROM Games WHERE Games.Username_Player1='%s' OR Games.Username_Player2='%s';",username,username);
	// hacemos la consulta 
	err=mysql_query (conn, consulta); 
	if (err!=0) {
		respuesta_servidor_sql = 3;//error al consultar datos 
		printf ("Error al consultar datos de la base %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}else if(err==0)
	{
		//recogemos el resultado de la consulta 
		resultado = mysql_store_result (conn); 
		row = mysql_fetch_row (resultado);
	}
	if (row == NULL)
	{
		printf ("No se han obtenido datos en la consulta\n");
		respuesta_servidor_sql=5;
	}
	else
	{
		strcpy(respuesta_cuenta,row[0]);

		// El resultado debe ser una matriz con una sola fila
		// una columna con la cuenta
		if (respuesta_cuenta!=NULL)
			respuesta_servidor_sql=atoi(respuesta_cuenta);//exito
		else
			respuesta_servidor_sql=4;//error en cuenta
	}
	// cerrar la conexion con el servidor MYSQL 
	mysql_close (conn);
	//regresamos el resultado
	return respuesta_servidor_sql;
}



char* viewScore(char username[100])//funcion para ver los puntos de una partida en especifico
{
	char* respuesta_char_servidor_sql=malloc(512);
	
	
	MYSQL *conn;
	int err;
	// Estructura especial para almacenar resultados de consultas 
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	
	char consulta [150];//string para enviar la consulta a BBDD
	
	//Creamos una conexion al servidor MYSQL 
	conn = mysql_init(NULL);
	if (conn==NULL) {
		printf ("Error al crear la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
	}
	//inicializar la conexion
	conn = mysql_real_connect (conn, "localhost","root", "mysql", "ProyectoSO",0, NULL, 0);
	if (conn==NULL) {
		printf ("Error al inicializar la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
	}
	
	// consulta SQL para obtener una tabla con todos los datos
	// de la base de datos
	//construir consulta sql	
	
	//sprintf(consulta,"SELECT *, IF(Username_Player1='%s',Score_Player1,Score_Player2) AS Score FROM Games WHERE Game_ID=2 AND (Username_Player1='%s' OR Username_Player2='%s');",username,username,username);
	sprintf(consulta,"SELECT * FROM Games WHERE Game_ID=2 AND (Username_Player1='%s' OR Username_Player2='%s');",username,username);
	// hacemos la consulta 
	err=mysql_query (conn, consulta); 
	if (err!=0) {
		printf ("Error al consultar datos de la base %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}else if(err==0)
	{
		//recogemos el resultado de la consulta 
		resultado = mysql_store_result (conn); 
		row = mysql_fetch_row (resultado);
		// El resultado debe ser una matriz con una sola fila
		// una columna con Username y una columna con Passwd y una con el nombre, esta ultima no se usa aqui
	}
	if (row == NULL)
	{
		strcpy(respuesta_char_servidor_sql,"No se ha encontrado la partida\n");
	}
	else
	{
		sprintf(respuesta_char_servidor_sql,"%s:%s puntos\n%s:%s",row[1],row[3],row[2],row[4]);
	}
	// cerrar la conexion con el servidor MYSQL 
	mysql_close (conn);
	//regresamos el resultado
	return respuesta_char_servidor_sql;
}

//main
int main(int argc, char *argv[])
{
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	
	char peticion[512];
	char peticion_servidor[512];
	char respuesta[512];
	int respuesta_servidor_sql;
	char respuesta_char_servidor_sql[512];
	char username_loggedin[100];
	
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
	serv_adr.sin_port = htons(9088);
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
			printf("Recibida una peticion\n");
			// Tenemos que agregar la marca de fin de string 
			// para que no escriba lo que hay despues en el buffer
			peticion[ret] = '\0';
			strcpy(peticion_servidor,peticion);
			char* p = strtok(peticion, "/");
			int codigo = atoi(p);
			//Escribimos la peticion en la consola
			if (codigo != 0){
				printf("La peticion es: %s\n", peticion_servidor);
			}
			
			if (codigo == 0)
				terminar = 1;
			else if (codigo == 1){ //registrar un usuario
				respuesta_servidor_sql = RegisterUser(peticion_servidor);
				if (respuesta_servidor_sql==0)
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
				respuesta_servidor_sql = Login(peticion_servidor);
				if (respuesta_servidor_sql==0)
				{
					strcpy(username_loggedin,strtok(NULL, "/"));
					strcpy(respuesta, "Login correcto");
				}
				else if(respuesta_servidor_sql==4)
				{
					strcpy(respuesta, "Password Incorrecto");
				}else if(respuesta_servidor_sql==5)
				{
					strcpy(respuesta, "Username Incorrecto");
				}else
				{
					strcpy(respuesta, "Error al iniciar sesion");
				}
			}else if(codigo==3)
			{
				respuesta_servidor_sql = CountGames(username_loggedin);
				sprintf(respuesta, "%d", respuesta_servidor_sql);
			}else if(codigo==4)
			{
				strcpy(respuesta_char_servidor_sql,viewScore(username_loggedin));
				strcpy(respuesta,respuesta_char_servidor_sql);
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
