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
#include <errno.h>
#include <my_global.h>
#include <unistd.h>

int puerto=50001;//puertos para shiva 50000-50003

//Estructura para acceso excluyente cuando se comparte una estructura compartida
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//Se pone "pthread_mutex_lock( &mutex );" antes de la operacion que no se puede interrumpir
//se pone la operacion a realizar
//se pone "pthread_mutex_unlock( &mutex );"

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
//Variables necesarias para lista de conectados
ConnectedUsers ListaConectados;
char conectados[300];
int i;
int sockets[100];
//Declaracion de funciones
//Funcion para crear una conexion con el servidor SQL
MYSQL * ConnectToSQL(char respuesta_sql_char[512])
{
	MYSQL *conn;
	//parametros para inicializar la conexion
	char *server = "shiva2.upc.es";//servidor
	char *user = "root";//usuario
	char *password = "mysql";//contrasena
	char *database = "M01_Runner2057";//base de datos que usremos
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
	{
		//printf("Se encontro el usuario en la posicion %d\n",i);
		return i;
	}else
		return -1;
}

//Funcion para eliminar usuario de lista de conectados cuando se desconecte
int RemoveConnectedUser(ConnectedUsers *list,char username[100])
{//devuelve 0 si elimina o -1 si no esta en lista
	int pos = UsernamePosition(list,username);
	if (pos == -1)
		return -1;
	else{
		//printf("Eliminando el usuario de la posicion %d\n",pos);
		int i;
		for (i=pos;i<list->num-1;i++)
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
void OnlineUsers(ConnectedUsers *list, char conectados[300],char username[100])
{
	strcpy(conectados,"");
	//checamos si solo hay un usuario.De ser asi quiere decir que estamos solos
	if(list->num==1)
	{
		strcpy(conectados, "No hay otros usuarios");
	}
	else
	{
		int i =0;
		for(i=0;i<list->num;i++)
		{
			if (strcmp(list->user[i].username,username)!=0)
				sprintf(conectados, "%s\n%s",conectados,list->user[i].username);
		}
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
			cuenta=atoi(row[0]);//regresamos el valor de la cuenta
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
	//verificamos que el usuario no este conectado en otro ordenador
	if(UsernamePosition(&ListaConectados,username)==-1)
	{
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
					pthread_mutex_lock(&mutex);
					//agregamos el usuario a la lista de conectados ya que se mantendra conectado
					connectUser = AddConnectedUser(&ListaConectados,username,socket);
					if (connectUser==0)
						respuesta_funcion=0;//Inicio de Sesion correcto
					else if (connectUser==-1)
						respuesta_funcion = -3;//no se puede iniciar sesion, servidor lleno
					pthread_mutex_unlock(&mutex);
				}
				else if((rescmp1==0)&&(rescmp2!=0))
					respuesta_funcion=-2;//Usuario Correcto pero contrasena incorrecta
			}
		}
	}else
	{
		respuesta_funcion=-4;
	}
	//regresamos el resultado
	return respuesta_funcion;
}
//Funcion para agregar una partida
int AddMatch(MYSQL *conn,char username1[100],char username2[100])
{
	char consulta [512];//string para enviar la consulta a BBDD
	MYSQL_ROW row;
	int respuesta_funcion;

	//Se construye la consulta sql
	sprintf(consulta,"INSERT INTO Games (Username_Player1, Username_Player2, Score_Player1, Score_Player2) VALUES('%s','%s',0,0);",username1,username2);
	// hacemos la consulta
	respuesta_funcion=QuerySQL(consulta,conn,&row);
	//regresamos el resultado
	return respuesta_funcion;
}
int AddUserScore(MYSQL *conn,char player[100],int puntos,int GameID)
{
	char consulta [512];//string para enviar la consulta a BBDD
	MYSQL_ROW row;
	int respuesta_funcion;

	//Se construye la consulta sql
	sprintf(consulta,"UPDATE Games SET %s=%d WHERE Game_ID=%d;",player,puntos,GameID);
	// hacemos la consulta
	respuesta_funcion=QuerySQL(consulta,conn,&row);
	//regresamos el resultado
	return respuesta_funcion;
}
int GetUserScore(MYSQL *conn,char player[100],int GameID)
{
	char consulta [512];//string para enviar la consulta a BBDD
	MYSQL_ROW row;
	int respuesta_funcion;

	//Se construye la consulta sql
	sprintf(consulta,"SELECT %s from Games WHERE Game_ID=%d;",player,GameID);
	// hacemos la consulta
	respuesta_funcion=QuerySQL(consulta,conn,&row);
	//regresamos el resultado
	if(respuesta_funcion==0)
	{
		if (row == NULL)
		{
			respuesta_funcion=-1;
		}
		else
		{
			respuesta_funcion=atoi(row[0]);//regresamos el valor de la cuenta
		}
	}
	return respuesta_funcion;
}
int CheckPlayerNumber(MYSQL *conn,char username1[100],int GameID)
{
	char consulta [512];//string para enviar la consulta a BBDD
	MYSQL_ROW row;
	int respuesta_funcion;

	//Se construye la consulta sql
	sprintf(consulta,"SELECT IF(STRCMP(Username_Player1,'%s')=0,1,0) FROM Games WHERE Game_ID=%d;",username1,GameID);
	// hacemos la consulta
	respuesta_funcion=QuerySQL(consulta,conn,&row);
	if(respuesta_funcion==0)
	{
		if (row == NULL)
		{
			respuesta_funcion=-1;
		}
		else
		{
			respuesta_funcion=atoi(row[0]);//regresamos el valor de la cuenta
		}
	}
	//regresamos el resultado
	return respuesta_funcion;
}
int GetLastMatchCreatedID(MYSQL *conn)
{
	char consulta [512];//string para enviar la consulta a BBDD
	MYSQL_ROW row;
	int respuesta_funcion;
	int matchID;
	//construimos consulta
	sprintf(consulta,"SELECT LAST_INSERT_ID()");
	// hacemos la consulta
	respuesta_funcion=QuerySQL(consulta,conn,&row);
	if(respuesta_funcion==0)
	{
		if (row == NULL)
		{
			matchID=-1;
		}
		else
		{
			matchID=atoi(row[0]);//regresamos el valor de la cuenta
		}
	}
	//regresamos el resultado
	return matchID;
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
	bool loggedIn;
	int currentlyinMatch=0;
	char currentOponent[100];
	int currentmatchID;
	int myscore=0;
	int opponentscore=0;
	char scorePlayer[100];
	int terminar = 0;
	while (terminar == 0)
	{
		ret = read(sock_conn, peticion, sizeof(peticion));
		printf("Recibida una peticion:\n");
		peticion[ret] = '\0';//Tenemos que anadirle la marca de fin de string para que no escriba lo que hay despues en el buffer
		printf("%s\n",peticion);

		char* p = strtok(peticion, "/");
		int codigo = atoi(p);
		p=strtok(NULL,"\0");
		if(p != NULL)
			strcpy(peticion,p);

		if (codigo == 0)
		{
			if (loggedIn==true)
			{
				pthread_mutex_lock(&mutex);//ahora no interrumpir
				int removeUser = RemoveConnectedUser(&ListaConectados, username);//eliminamos de lista de conectados
				pthread_mutex_unlock(&mutex);//ahora se puede interrumpir
				if(removeUser==0)
				{
					printf("%s se ha desconectado\n",username);
					loggedIn=false;
				}
				else if (removeUser==-1)
				{
					printf("Error al desconectar\n");
				}
			}
			else if(loggedIn==false)
			{
				printf("No has iniciado sesion\n");
			}
			terminar = 1;
			strcpy(respuesta_para_cliente,"0/Cerrando Sesion");
		}else if (codigo == 1)//registrar un usuario
		{
			respuesta_servidor_sql = RegisterUser(peticion, conn);
			if (respuesta_servidor_sql==0)
				strcpy(respuesta_para_cliente, "1/Registrado Correctamente");
			else if (respuesta_servidor_sql==-2)
				strcpy(respuesta_para_cliente, "1/Username ya existe, escoge otro Username");
			else
				strcpy(respuesta_para_cliente, "1/Error en el registro");
		}else if (codigo == 2)//login
		{
			respuesta_servidor_sql = Login(peticion,conn,sock_conn,username);
			if (respuesta_servidor_sql==-1)
				strcpy(respuesta_para_cliente, "2/Username Incorrecto o No Registrado");
			else if (respuesta_servidor_sql==-2)
				strcpy(respuesta_para_cliente, "2/Password Incorrecto");
			else if (respuesta_servidor_sql == -3)
				sprintf(respuesta_para_cliente, "2/Servidor lleno, intenta mas tarde");
			else if(respuesta_servidor_sql==0)
			{
				loggedIn=true;
				sprintf(respuesta_para_cliente, "2/Login Correcto");
			}else if(respuesta_servidor_sql==-4){
			sprintf(respuesta_para_cliente, "2/Usuario ya conectado, intenta con otro usuario");
			}
		}else if (codigo == 3)//contar partidas jugadas
		{
			respuesta_servidor_sql = CountGames(username, conn);
			if (respuesta_servidor_sql==-1)
				strcpy(respuesta_para_cliente, "3/Username Incorrecto o No Registrado");
			else if (respuesta_servidor_sql==0)
				strcpy(respuesta_para_cliente, "3/No se encontraron partidas");
			else
				sprintf(respuesta_para_cliente, "3/%d",respuesta_servidor_sql);
		}else if (codigo == 4)//ver resultados de una partida
		{
			respuesta_servidor_sql = ViewGameScore(peticion,respuesta_sql_char,conn,username);
			if(respuesta_servidor_sql==-1)
				sprintf(respuesta_para_cliente, "4/No se encontraron partidas");
			else if (respuesta_servidor_sql==0)
				sprintf(respuesta_para_cliente, "4/%s",respuesta_sql_char);
		}else if (codigo == 5)//ver lista de conectados
		{
			OnlineUsers(&ListaConectados, conectados,username);
			sprintf(respuesta_para_cliente,"5/%s" ,conectados);
		}else if (codigo == 6)//logout, cerrar sesion y quitar cliente de lista de conectados
		{
			if (loggedIn==true)
			{
				pthread_mutex_lock(&mutex);//ahora no interrumpas
				//printf("Username:%s\n",username);
				int removeUser = RemoveConnectedUser(&ListaConectados, username);
				pthread_mutex_unlock(&mutex);//ahora se puede interrumpir
				//printf("Resultado de remover usuario:%d\n",removeUser);
				if(removeUser==0)
				{
					printf("%s se ha desconectado\n",username);
					strcpy(respuesta_para_cliente, "6/Desconectado");
					loggedIn=false;
				}
				else if (removeUser==-1)
				{
					printf("Error al desconectar\n");
					strcpy(respuesta_para_cliente,"6/Error al desconectar");
				}
			}
			else if(loggedIn==false)
			{
				printf("No has iniciado sesion\n");
				sprintf(respuesta_para_cliente,"6/No has iniciado sesion");
			}
		}else if (codigo == 8)//Mandar Invitacion a un usario
		{
			strcpy(currentOponent,peticion);
			if(strcmp(username,currentOponent)!=0)
			{
				int positionForInvite = UsernamePosition(&ListaConectados,currentOponent);
				if(currentlyinMatch==1)
				{
					sprintf(respuesta_para_cliente,"8/Wating for response");
				}
				else{

					if(positionForInvite==-1)
					{
						sprintf(respuesta_para_cliente,"8/Username does not exist or is not connected");
					}else{
						currentlyinMatch=0;
						sprintf(respuesta_para_cliente,"9/%s",username);
						write(ListaConectados.user[positionForInvite].socket,respuesta_para_cliente,strlen(respuesta_para_cliente));
						sprintf(respuesta_para_cliente,"8/Invite Sent");
					}
				}
			}else if(strcmp(username,currentOponent)==0){
				sprintf(respuesta_para_cliente,"8/Cant invite yourself");
			}
			printf("se ha enviado invitacion\n");
		}else if (codigo == 10)
		{
			char responseForInvite[20];
			char userWhoInvited[20];
			strcpy(responseForInvite,strtok(peticion, "/"));
			strcpy(userWhoInvited,strtok(NULL, "\0"));
			int positionForResponse = UsernamePosition(&ListaConectados,userWhoInvited);
			if(strcmp(responseForInvite,"Accept")==0)
			{
				currentlyinMatch=1;
				strcpy(currentOponent,userWhoInvited);
				int addmatch = AddMatch(conn, username,currentOponent);
				currentmatchID = GetLastMatchCreatedID(conn);
				sprintf(respuesta_para_cliente,"11/Invitation Accepted/%d",currentmatchID);
				write(ListaConectados.user[positionForResponse].socket,respuesta_para_cliente,strlen(respuesta_para_cliente));
			}else if (strcmp(responseForInvite,"Decline")==0)
			{
				sprintf(respuesta_para_cliente,"11/Invitation Rejected");
				currentlyinMatch=0;
				write(ListaConectados.user[positionForResponse].socket,respuesta_para_cliente,strlen(respuesta_para_cliente));
			}else{
				sprintf(respuesta_para_cliente,"11/Error");
				currentlyinMatch=0;
				write(ListaConectados.user[positionForResponse].socket,respuesta_para_cliente,strlen(respuesta_para_cliente));
			}
			sprintf(respuesta_para_cliente,"10/Response Sent");
		}else if (codigo == 12)//le pasamos el match id al oponente para despues utilizarlo
		{
			currentmatchID=atoi(peticion);
			sprintf(respuesta_para_cliente,"12/Obtained MatchID");
			int positionForMessage = UsernamePosition(&ListaConectados,currentOponent);//obtengo el socket del oponente
			write(ListaConectados.user[positionForMessage].socket,respuesta_para_cliente,strlen(respuesta_para_cliente));//envio mensaje al oponente
		}else if(codigo == 14)
		{
			sprintf(respuesta_para_cliente,"14/Cargando Escena");
		}else if(codigo == 15)
		{
			sprintf(respuesta_para_cliente,"16/%s: %s",username,peticion);
			write(sock_conn, respuesta_para_cliente, strlen(respuesta_para_cliente));//me la mando a mi mismo
			int positionForMessage = UsernamePosition(&ListaConectados,currentOponent);//obtengo el socket del oponente
			write(ListaConectados.user[positionForMessage].socket,respuesta_para_cliente,strlen(respuesta_para_cliente));//envio mensaje al oponente
		}else if(codigo == 17)//subimos nuestra puntuacion a base de datos y checamos quien gana
		{
			myscore = atoi(peticion);
			int Player1bool = CheckPlayerNumber(conn,username,currentmatchID);//primero checamos si el usuario es el Player1 o 2
			printf("Player1bool=%d\n",Player1bool);
			if(Player1bool==1)//quiere decir que mi usuario es el player 1 en la base de datos
			{
				strcpy(scorePlayer,"Score_Player1");
				printf("Yo soy el player1, %s\n",username);
			}else{
				strcpy(scorePlayer,"Score_Player2");
				printf("Yo soy el player2, %s\n",username);
			}
			int addtoscore = AddUserScore(conn,scorePlayer, myscore,currentmatchID);
			while(opponentscore==0)
			{
				if(Player1bool == 1)
				{
					opponentscore = GetUserScore(conn,"Score_Player2",currentmatchID);//checar el score del oponente jugador 2
				}else{
					opponentscore = GetUserScore(conn,"Score_Player1",currentmatchID);//checar el score del oponente jugador 1
				}
				printf("Score del oponente es:%d\n",opponentscore);
				printf("My score es: %d\n",myscore);
				if(opponentscore!=0)
				{
					if(opponentscore>myscore)
					{
						sprintf(respuesta_para_cliente,"18/You lost");
					}else if(opponentscore<myscore){
						sprintf(respuesta_para_cliente,"18/You won");
					}else if(opponentscore==myscore){
						sprintf(respuesta_para_cliente,"18/It is a tie");
					}else{
						sprintf(respuesta_para_cliente,"18/Error when viewing who won");
					}
				}
				sleep(5);
			}

			currentlyinMatch=0;//avisamos que ya termino la partida y podemos jugar
		}else if(codigo == 19)//subimos nuestra puntuacion a base de datos despues de que alguien abandono la partida
		{
			myscore = atoi(peticion);
			int Player1bool = CheckPlayerNumber(conn,username,currentmatchID);//primero checamos si el usuario es el Player1 o 2
			printf("Player1bool=%d\n",Player1bool);
			if(Player1bool==1)//quiere decir que mi usuario es el player 1 en la base de datos
			{
				strcpy(scorePlayer,"Score_Player1");
				printf("Yo soy el player1, %s\n",username);
			}else{
				strcpy(scorePlayer,"Score_Player2");
				printf("Yo soy el player2, %s\n",username);
			}
			int addtoscore = AddUserScore(conn,scorePlayer, myscore,currentmatchID);
			sprintf(respuesta_para_cliente,"20/Close");//mensaje para cerras game message parser
			currentlyinMatch=0;//avisamos que ya termino la partida y podemos jugar
		}

		if (((codigo != 0) && (codigo != 15)&&(codigo !=10))||(codigo == 0))
		{
			write(sock_conn, respuesta_para_cliente, strlen(respuesta_para_cliente));// Enviamos la respuesta
			if(codigo == 2)//enviamos notifiacion de usuario conectado/desconectado
			{
				int j;
				pthread_mutex_lock(&mutex);
				for (j=0;j<ListaConectados.num;j++)
				{
					printf("%s\n",respuesta_para_cliente);
					if(j==UsernamePosition(&ListaConectados,username))
					{
						sprintf(respuesta_para_cliente,"7/Te has conectado/");
						write(ListaConectados.user[j].socket,respuesta_para_cliente,strlen(respuesta_para_cliente));
					}else
					{
						sprintf(respuesta_para_cliente,"7/%s se ha conectado/",username);
						write(ListaConectados.user[j].socket,respuesta_para_cliente,strlen(respuesta_para_cliente));
						sprintf(respuesta_para_cliente,"13/");
						write(ListaConectados.user[j].socket,respuesta_para_cliente,strlen(respuesta_para_cliente));
					}
				}
				pthread_mutex_unlock(&mutex);
			}else if (codigo == 6)
			{
				int j;
				pthread_mutex_lock(&mutex);
				for (j=0;j<ListaConectados.num;j++)
				{
					printf("%s\n",respuesta_para_cliente);
					if(j==UsernamePosition(&ListaConectados,username))
					{
						sprintf(respuesta_para_cliente,"7/Te has desconectado/");
						write(ListaConectados.user[j].socket,respuesta_para_cliente,strlen(respuesta_para_cliente));
					}else
					{
						sprintf(respuesta_para_cliente,"7/%s se ha desconectado/",username);
						write(ListaConectados.user[j].socket,respuesta_para_cliente,strlen(respuesta_para_cliente));
						sprintf(respuesta_para_cliente,"13/");
						write(ListaConectados.user[j].socket,respuesta_para_cliente,strlen(respuesta_para_cliente));
					}
				}
				pthread_mutex_unlock(&mutex);
			}
		}
	}
	// Se acabo el servicio para este cliente
	close(sock_conn);
	printf("Conexion cerrada\n");
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
		printf("Error creando socket\n");

	printf("Intentando bind\n");
	// Hacemos el bind
	memset(&serv_adr, 0, sizeof(serv_adr));// Inicializa a cero serv_addr
	serv_adr.sin_family = AF_INET;

	// asocia el socket a cualquiera de las IP de la maquina.
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// establecemos el puerto de escucha
	serv_adr.sin_port = htons(puerto);
	if (bind(sock_listen, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) < 0)
		printf("Error al bind\n");

	if (listen(sock_listen, 3) < 0)
		printf("Error en el Listen\n");

	pthread_t thread;
	i = 0;
	for (;;) {
		printf("Escuchando\n");

		sock_conn = accept(sock_listen, NULL, NULL);

		printf("He recibido conexion\n");

		sockets[i] = sock_conn;
		//sock_conn es el socket que usaremos para este cliente
		// Crear thread y decirle lo que tiene que hacer

		pthread_create(&thread, NULL, ServeClient, &sockets[i]);
		i = i + 1;
	}
}
