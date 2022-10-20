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

//estructura de usuarios con su socket para lista de conectados
typedef struct
{
	char username[100];
	int socket;
}User;
//estrucutra de usuarios conectados
typedef struct
{
	User user[100];
	int num;
}ConnectedUsers;

//variables globales necesarias
char respuesta_sql_char[512];//Aqui se reciben las respuestas y errores del servidor sql
MYSQL *conn;
bool loggedIn=false;
//Variables necesarias para lista de conectados
ConnectedUsers ListaConectados;
char conectados[300];
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
//Funcion para agregar usuario a lista de conectados
int AddConnectedUser(ConnectedUsers *list,char username[100], int socket)
{//regresa -1 si lista estaba llena, 0 si se anadio
	if (list->num==100)
		return -1;
	else
	{
		strcpy(list->user[list->num].username,username);//le pasamos el usuario
		list->user[list->num].socket=socket;//le pasamos el socket
		list->num++;
		return 0;
	}
}
//Funcion para buscar la posicion de un usuario en lista
int UsernamePosition(ConnectedUsers *list,char username[100])
{//devuelve posicion en lista o -1 si no esta en lista
	int i = 0;
	int encontrado = 0;
	while ((i< list-> num) && !encontrado)
	{
		if (strcmp(list->user[i].username,username)==0)
			encontrado=1;
		if(!encontrado)
			i++;
	}
	if (encontrado)
		return i;
	else 
		return -1;
}

//Funcion para eliminar usuario de lista de conectados cuando se desconecte
int RemoveConnectedUser(ConnectedUsers *list,char username[100])
{//devuelve 0 si elimina o -1 si no esta en lista
	int pos = UsernamePosition(list,username);
	if (pos == -1)
		return -1;
	else{
		int i;
		for (i=0;i<list->num-1;i++)
		{
			list->user[i]=list->user[i+1];
			//strcpy(lista->conectados[i].nombre,lista->conectados[i+1].nombre);
			//lista->conectados[i].socket=lista->conectados[i+1].socket;
		}
		list->num--;
		return 0;
	}
}

//Funcion para ver los usuarios conectados
void OnlineUsers(ConnectedUsers *list, char conectados[300])
{
	sprintf(conectados, "%d", list->num);
	int i =0;
	for(i=0;i<list->num;i++)
	{
		sprintf(conectados, "%s/%s",conectados,list->user[i].username);
	}
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
//Funcion para ver el total de partidas jugadas
int CountGames(char username[100], MYSQL *conn)
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
int ViewGameScore(char peticion[512],char respuesta_sql_char[512],MYSQL *conn,char username[100])
{
	MYSQL_ROW row;
	int respuesta_query;
	int respuesta_funcion;
	
	char GameID[100];
	strcpy(GameID,strtok(peticion, "\0"));
	
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
int Login(char peticion[512],MYSQL *conn, int socket,char username[100])
{
	MYSQL_ROW row;
	int respuesta_query;
	int respuesta_funcion;
	
	char password[100];
	
	char db_username[100];
	char db_password[100];
	int rescmp1;
	int rescmp2;
	int connectUser;
	
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
			if ((rescmp1==0)&&(rescmp2==0)&&(ListaConectados.num<100))
			{
				//agregamos el usuario a la lista de conectados ya que se mantendra conectado
				connectUser = AddConnectedUser(&ListaConectados,username,socket);
				if (connectUser==0)
					respuesta_funcion=0;//Inicio de Sesion correcto
				else if (connectUser==-1)
					respuesta_funcion = -3;//no se puede iniciar sesion, servidor lleno
			}
			else if((rescmp1==0)&&(rescmp2!=0))
				respuesta_funcion=-2;//Usuario Correcto pero contrasena incorrecta
		}
	}
	//regresamos el resultado
	return respuesta_funcion;
}
//Funcion que atiende a cada cliente en un thread
void* ServeClient(void* socket)
{
	int sock_conn;
	int* s;
	s = (int*)socket;
	sock_conn = *s;
	
	int ret;
	char peticion[512];
	
	int respuesta_servidor_sql;
	char respuesta_para_cliente[512];
	char username[100];//variable para que el thread recuerde el usuario de ese thread
	
	int terminar = 0;
	while (terminar == 0)
	{
		ret = read(sock_conn, peticion, sizeof(peticion));
		printf("Recibida una petición\n");
		peticion[ret] = '\0';//Tenemos que anadirle la marca de fin de string para que no escriba lo que hay despues en el buffer
		

		char* p = strtok(peticion, "/");
		int codigo = atoi(p);
		p=strtok(NULL,"\0");
		if(p != NULL)
			strcpy(peticion,p);
		
		if (codigo == 0)
			terminar = 1;
		else if (codigo == 1)//registrar un usuario
		{ 
			respuesta_servidor_sql = RegisterUser(peticion, conn);
			if (respuesta_servidor_sql==0)
				strcpy(respuesta_para_cliente, "Registrado Correctamente");
			else if (respuesta_servidor_sql==-2)
				strcpy(respuesta_para_cliente, "Username ya existe, escoge otro Username");
			else
				strcpy(respuesta_para_cliente, "Error en el registro");
		}else if (codigo ==2)//login
		{
			respuesta_servidor_sql = Login(peticion,conn,sock_conn,username);
			
			if (respuesta_servidor_sql==-1)
				strcpy(respuesta_para_cliente, "Username Incorrecto o No Registrado");
			else if (respuesta_servidor_sql==-2)
				strcpy(respuesta_para_cliente, "Password Incorrecto");
			else if (respuesta_servidor_sql == -3)
				sprintf(respuesta_para_cliente, "Servidor lleno, intenta mas tarde");
			else if(respuesta_servidor_sql==0)
			{
				loggedIn=true;
				sprintf(respuesta_para_cliente, "Login Correcto");
			}
		}else if (codigo ==3)//contar partidas jugadas
		{
			respuesta_servidor_sql = CountGames(username, conn);
			if (respuesta_servidor_sql==-1)
				strcpy(respuesta_para_cliente, "Username Incorrecto o No Registrado");
			else if (respuesta_servidor_sql==0)
				strcpy(respuesta_para_cliente, "No se encontraron partidas");
			else
				sprintf(respuesta_para_cliente, "%d",respuesta_servidor_sql);
		}else if (codigo==4)//ver resultados de una partida
		{
			respuesta_servidor_sql = ViewGameScore(peticion,respuesta_sql_char,conn,username);
			if(respuesta_servidor_sql==-1)
				sprintf(respuesta_para_cliente, "No se encontraron partidas");
			else if (respuesta_servidor_sql==0)
				sprintf(respuesta_para_cliente, "%s",respuesta_sql_char);
		}else if(codigo==5)//ver lista de conectados
		{
			OnlineUsers(&ListaConectados, conectados);
			strcpy(respuesta_para_cliente, conectados);
		}
		if (codigo != 0)
			write(sock_conn, respuesta_para_cliente, strlen(respuesta_para_cliente));// Enviamos la respuesta
	}
	// Se acabo el servicio para este cliente
	close(sock_conn);
	if (loggedIn=true)
	{
		int removeUser = RemoveConnectedUser(&ListaConectados, username);
		if(removeUser==0)
			printf("%s se ha desconectado\n",username);
		else if (removeUser==-1)
			printf("Error al desconectar");
	}
	else if (loggedIn=false)
	{
		printf("Conexion cerrada antes del login");
	}
	
} 
//Main 
int main(int argc, char *argv[]) 
{
	conn = ConnectToSQL(respuesta_sql_char);
	printf("%s\n",respuesta_sql_char);
	
	int sock_conn, sock_listen;
	struct sockaddr_in serv_adr;
	
	ListaConectados.num=0;
	
	// Inicializaciones
	// Abrimos el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creando socket");
	
	bool bindExitoso;
	int puerto=9050;
	while(bindExitoso==false)
	{
		printf("Intentando bind\n");
		
		// Hacemos el bind
		memset(&serv_adr, 0, sizeof(serv_adr));// Inicializa a cero serv_addr
		serv_adr.sin_family = AF_INET;
		
		// asocia el socket a cualquiera de las IP de la maquina. 
		//htonl formatea el numero que recibe al formato necesario
		serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
		// establecemos el puerto de escucha
		serv_adr.sin_port = htons(puerto);
		
		if (bind(sock_listen,(struct sockaddr*)&serv_adr, sizeof(serv_adr)) !=0)
		{
			printf("Error al bind\nIntentando de nuevo\n");
			if(puerto==9050)
				puerto=9051;
			else if(puerto ==9051)
				puerto = 9052;
			else 
				puerto = 9050;
		}
		else
		{
			bindExitoso=true;
			printf("Bind realizado correctamente en puerto %d\n",puerto);
		}
	}
	if (listen(sock_listen, 3) < 0)
		printf("Error en el Listen\n");
	
	
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
		
		pthread_create(&thread, NULL, ServeClient, &sockets[i]);
		i = i + 1;
	}
}

