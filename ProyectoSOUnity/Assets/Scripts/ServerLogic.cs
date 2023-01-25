using System;
using System.Text;
using System.Net;
using System.Net.Sockets;
using UnityEngine;
using System.Collections;

namespace Client
{ 
    /** Singleton class para conectarse al servidor a traves del socket*/
    class ServerConnection
    {
        Socket server;
        IPAddress ip;
        IPEndPoint port;
        bool connected = false;
        bool loggedin=false;
        static ServerConnection connection = null;
        public int puerto = 50001;
        string user;
        string passwd;

        //Constructor para inicializar la conexion
        private ServerConnection()
        {
            this.ip = IPAddress.Parse("147.83.117.22");
            this.port = new IPEndPoint(this.ip, puerto);
            server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        }

        //regresa la conexion al servidor para acceder desde otros scripts
        public static ServerConnection GetInstance()
        {
            connection ??= new ServerConnection();

            return connection;
        }

        //funcion para conectarse al servidor
        public int ConnectToServer()
        {
            Debug.Log("Connecting to server");
            connected = true;
            try
            {
                server.Connect(port);//Intentamos conectar el socket     
                Console.WriteLine("Connected");

                return 0;
            }
            catch (SocketException)
            {
                Console.WriteLine("Server Connection Error");
                return -1;

            }
        }
        
        //funcion que envia mensaje al servidor
        public void SendMessage(String message)
        {
            byte[] msg = System.Text.Encoding.ASCII.GetBytes(message);
            server.Send(msg);
        }

        //funcion que escucha para recibir mensaje del servidor
        public String ListenForMessage()
        {

            byte[] data = new byte[1024];
            int dataSize = server.Receive(data);
            String msg = Encoding.ASCII.GetString(data, 0, dataSize);
            return msg;

        }

        //funcion para cerrar la conexion
        public void DisconnectFromServer()
        {
            string mensaje = "0/";

            byte[] msg = Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);

            server.Shutdown(SocketShutdown.Both);
            server.Disconnect(true);
            server.Close();
        }
        //getter para bool connected
        public bool IsConnected()
        {
            return connected;
        }
        public bool IsLoggedIn()
        {
            return loggedin;
        }
        public void SetLoggedIn(bool loggedin)
        {
            this.loggedin = loggedin;
        }
        //setter para bool connected
        public void SetConnected(bool connected)
        {
            this.connected = connected;
        }
        public void SetUser(string user)
        {
            this.user = user;
        }
        public string GetUser()
        {
            return user;
        }
        public void SetPasswd(string passwd)
        {
            this.passwd = passwd;
        }
        public string GetPasswd()
        {

            return passwd;
        }
        
    }
}