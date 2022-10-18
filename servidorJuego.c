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
#include <pthread.h>


//preguntas 
//Es mejor guardar el username por parte del cliente no?

//Declaracion de funciones

//Funcion para crear una conexion con el servidor SQL
MYSQL * ConnectToSQL(char respuesta_sql_char[512])
{
	MYSQL *conn;
	//parametros para inicializar la conexion
	char *server = "localhost";//servidor
	char *user = "root";//usuario
	char *password = "mysql";//contrasena
	char *database = "ProyectoSO";//base de datos que usremos
	//Creamos una conexion al servidor MYSQL 
	conn = mysql_init(NULL);
	if (conn==NULL) {
		sprintf(respuesta_sql_char,"Error al crear la conexion: %u %s", mysql_errno(conn), mysql_error(conn));
	}
	/* Inicializar la base de datos */
	conn = mysql_real_connect (conn, server,user, password, database,0, NULL, 0);
	if (conn==NULL) {
		sprintf (respuesta_sql_char,"Error al inicializar la conexion: %u %s", mysql_errno(conn), mysql_error(conn));
	}
	else
	{
		sprintf (respuesta_sql_char,"Exito al conectar con Base de Datos");
		return conn;
	}
}
//Funcion para desconectar de SQL
void DisconnectFromSQL(char respuesta_sql_char[512],MYSQL *conn)
{
	MYSQL *ptr = conn;
	mysql_close(conn);
	sprintf(respuesta_sql_char,"Desconectado");
}
//Funcion para hacer consultas
int QuerySQL(char consulta[512], MYSQL *conn, MYSQL_ROW *row)
{
	MYSQL_RES *resultado;
	int err;
	int resultado_consulta;
	
	err = mysql_query(conn, consulta);
	if (err!=0 && err!=1) 
	{
		resultado_consulta = -1;//Error al consultar la base de datos
		exit (1);
	}else if (err==1)
	{
		resultado_consulta=-2;
	}
	else if(err==0)
	{
		//recogemos el resultado de la consulta
		resultado = mysql_store_result(conn); 
		if (resultado != NULL)
		{
			*row = mysql_fetch_row(resultado);
		}
		resultado_consulta=0;
	}
	return resultado_consulta;
}
//Funcion para agregar jugadores,le pasamos la peticion pero ya sin el codigo de peticion
int RegisterUser(char peticion[512],MYSQL *conn)
{
	char username[100];
	char password[100];
	char name[100];
	char consulta [512];//string para enviar la consulta a BBDD
	MYSQL_ROW row;
	int respuesta_funcion;
	
	strcpy(username,strtok(peticion, "/"));
	strcpy(password,strtok(NULL, "/"));
	strcpy(name,strtok(NULL, "/"));
	
	//Se construye la consulta sql
	sprintf(consulta,"INSERT INTO Jugador VALUES('%s','%s','%s');",username,password,name);
	// hacemos la consulta
	respuesta_funcion=QuerySQL(consulta,conn,&row);
	
	//regresamos el resultado
	return respuesta_funcion;
}
int CountGames(char username[100],MYSQL *conn)//funcion para ver el total de partidas jugadas
{
	int cuenta;
	MYSQL_ROW row;
	int respuesta_funcion;
	
	char consulta [512];//string para enviar la consulta a BBDD
	
	//Se construye consulta sql	
	sprintf(consulta,"SELECT COUNT(Games.Game_ID) FROM Games WHERE Games.Username_Player1='%s' OR Games.Username_Player2='%s';",username,username);
	// hacemos la consulta 
	respuesta_funcion=QuerySQL(consulta,conn,&row);//hacemos consulta y recibimos respuesta
	if(respuesta_funcion==0)
	{
		if (row == NULL)
		{
			cuenta=-1;
		}
		else
		{
			cuenta =atoi(row[0]);//regresamos el valor de la cuenta 
		}
	}
	//regresamos el resultado
	return cuenta;
}
//Funcion para regresar los puntos de una partida
int ViewGameScore(char peticion[512],char respuesta_sql_char[512],MYSQL *conn)
{
	MYSQL_ROW row;
	int respuesta_query;
	int respuesta_funcion;
	
	char username[100];
	char GameID[100];
	
	strcpy(username,strtok(peticion, "/"));
	strcpy(GameID,strtok(NULL, "/"));
	
	char consulta [512];//string para enviar la consulta a BBDD
	sprintf(consulta,"SELECT * FROM Games WHERE Game_ID=%s AND (Username_Player1='%s' OR Username_Player2='%s');",GameID,username,username);
	
	respuesta_query=QuerySQL(consulta,conn,&row);//hacemos consulta y recibimos respuesta
	
	if(respuesta_query==0)
	{
		if (row == NULL)
		{
			respuesta_funcion=-1;//No se ha encontrado partida
		}
		else
		{
			sprintf(respuesta_sql_char,"%s: %s puntos\n%s: %s puntos",row[1],row[3],row[2],row[4]);
			respuesta_funcion=0;
		}
	}
	//regresamos el resultado
	return respuesta_funcion;
}
//Funcion para inicar sesion
int Login(char peticion[512],MYSQL *conn)
{
	MYSQL_ROW row;
	int respuesta_query;
	int respuesta_funcion;
	
	char username[100];
	char password[100];
	
	char db_username[100];
	char db_password[100];
	int rescmp1;
	int rescmp2;
	
	strcpy(username,strtok(peticion, "/"));
	strcpy(password,strtok(NULL, "/"));
	
	char consulta [512];//string para enviar la consulta a BBDD
	sprintf(consulta,"SELECT * FROM Jugador WHERE Username='%s';",username);
	
	respuesta_query=QuerySQL(consulta,conn,&row);//hacemos consulta y recibimos respuesta
	
	if(respuesta_query==0)
	{
		if (row == NULL)
			respuesta_funcion=-1;//Username Incorrecto o No existe
		else
		{
			strcpy(db_username,row[0]);
			strcpy(db_password,row[1]);
			rescmp1 = strcmp(username,db_username);
			rescmp2 = strcmp(password, db_password);
			if ((rescmp1==0)&&(rescmp2==0))
				respuesta_funcion=0;//Inicio de Sesion correcto
			else if((rescmp1==0)&&(rescmp2!=0))
				respuesta_funcion=-2;//Usuario Correcto pero contrasena incorrecta
			
			
		}
	}
	//regresamos el resultado
	return respuesta_funcion;
}

//Funcion para atender cliente
void* ServeClient(void* socket, char respuesta_sql_char[512],MYSQL *conn)
{
	int sock_conn;
	int* s;
	s = (int*)socket;
	sock_conn = *s;
	int ret;
	char peticion[512];
	
	
	int respuesta_servidor_sql;
	char respuesta_para_cliente[512]; 
	/////////
	int terminar = 0;
	while (terminar == 0)
	{
		
		ret = read(sock_conn, peticion, sizeof(peticion));
		printf("Recibida una petición\n");
		peticion[ret] = '\0';//Tenemos que a?adirle la marca de fin de string para que no escriba lo que hay despues en el buffer
		char* p = strtok(peticion, "/");
		int codigo = atoi(p);
		strcpy(peticion,strtok(NULL,"\0"));
		
		if (codigo == 0)
			terminar = 1;
		else if (codigo == 1)//registrar un usuario
		{ 
			respuesta_servidor_sql = RegisterUser(peticion, conn);
			if (respuesta_servidor_sql==0)
			{
				strcpy(respuesta_para_cliente, "Registrado Correctamente\n");
			}
			else if (respuesta_servidor_sql==-2)
			{
				strcpy(respuesta_para_cliente, "Username ya existe, escoge otro Username\n");
			}else
			{
				strcpy(respuesta_para_cliente, "Error en el registro\n");
			}
		}else if (codigo ==2)//login
		{
			respuesta_servidor_sql = Login(peticion, conn);
			if (respuesta_servidor_sql==-1)
			{
				strcpy(respuesta_para_cliente, "Username Incorrecto o No Registrado\n");
			}
			else if (respuesta_servidor_sql==-2)
			{
				strcpy(respuesta_para_cliente, "Password Incorrecto\n");
			}
			else if(respuesta_servidor_sql==0)
			{
				sprintf(respuesta_para_cliente, "Login Correcto\n");
			}
		}else if (codigo ==3)//contar partidas jugadas
		{
			respuesta_servidor_sql = CountGames(peticion, conn);
			if (respuesta_servidor_sql==-1)
			{
				strcpy(respuesta_para_cliente, "Error en la consulta\n");
			}
			else if (respuesta_servidor_sql==0)
			{
				strcpy(respuesta_para_cliente, "No se encontraron partidas\n");
			}
			else
			{
				sprintf(respuesta_para_cliente, "%d partidas\n",respuesta_servidor_sql);
			}
		}else if (codigo==4)//ver resultados de una partida
		{
			respuesta_servidor_sql = ViewGameScore(peticion,respuesta_sql_char,conn);
			if(respuesta_servidor_sql==-1)
			{
				sprintf(respuesta_para_cliente, "No se encontraron partidas\n");
			}else if (respuesta_servidor_sql==0)
			{
				sprintf(respuesta_para_cliente, "%s\n",respuesta_sql_char);
			}
		}
		
		
		if (codigo != 0) 
			write(sock_conn, respuesta_para_cliente, strlen(respuesta_para_cliente));	// Enviamos la respuesta
	}
	// Se acabo el servicio para este cliente
	close(sock_conn);
}
//Main 
int main(int argc, char *argv[]) 
{
	int puerto =9060;
	char respuesta_sql_char[512];//Aqui se reciben las respuestas y errores del servidor sql
	MYSQL *conn = ConnectToSQL(respuesta_sql_char);
	printf("%s\n",respuesta_sql_char);
	
	int sock_conn, sock_listen;
	struct sockaddr_in serv_adr;
	
	// Inicializaciones
	// Abrimos el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	
	// Hacemos el bind
	memset(&serv_adr, 0, sizeof(serv_adr));// Inicializa a cero serv_addr
	serv_adr.sin_family = AF_INET;
	
	// asocia el socket a cualquiera de las IP de la maquina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// establecemos el puerto de escucha
	serv_adr.sin_port = htons(puerto);
	if (bind(sock_listen, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) < 0)
		printf("Error al bind");
	
	if (listen(sock_listen, 3) < 0)
		printf("Error en el Listen");
	
	int i;
	int sockets[100];
	pthread_t thread;
	i = 0;
	for (;;) {
		printf("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf("He recibido conexion\n");
		
		sockets[i] = sock_conn;
		//sock_conn es el socket que usaremos para este cliente
		// Crear thead y decirle lo que tiene que hacer
		pthread_create(&thread, NULL, ServeClient(&sock_conn,respuesta_sql_char,conn), &sockets[i]);
		i = i + 1;
		
	}
}

