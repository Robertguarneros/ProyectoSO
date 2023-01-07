using System;
using System.Collections;
using System.Collections.Generic;
using System.Threading.Tasks;
using UnityEngine;
using UnityEditor;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using Client;
using MainMenuUI;
using TMPro;
using Level_1UI;

public class Game_MessageParser : MonoBehaviour
{
    private Level_1 level_1;
    private ServerConnection serverConnection;
    public TextMeshProUGUI EndMessage;

    void Awake()
    {
        SceneManager.sceneLoaded += OnSceneLoaded;
    }
    void OnSceneLoaded(Scene scene, LoadSceneMode mode)
    {
        Debug.Log("Loading Game_Message_Parser");
        GameObject Level_1Window = GameObject.Find("Level1_UI");//Obtenemos la informacion de la ventana del menu principal antes de inicar sesion
        level_1 = Level_1Window.GetComponent<Level_1>();
        ConnectToServer();
    }

    async private void ListenForServer()
    {//formato de los mensaje codigo/mensaje
        bool run2 = true;
        while (run2)
        {
            Debug.Log("Esperando Respuesta del Servidor en Game_MessageParser");
            string serverResponse = await Task.Run(() => serverConnection.ListenForMessage());

            Debug.Log("Respuesta del servidor"+serverResponse);
            string[] pieces = serverResponse.Split('/');
            int code = Convert.ToInt32(pieces[0]);
            string message = pieces[1].Split('\0')[0];
            Debug.Log("Mensaje antes de switch:"+message);

            Debug.Log("Start of Switch");
            switch (code)
            {
                case 0:
                    if (message == "Cerrando Sesion")
                    {
                        Debug.Log("Cerrando Sesion");
                        run2 = false;
                    }
                    break;
                case 1://registro de usuarios
                    Debug.Log(message);
                    break;
                case 2://login 
                    Debug.Log(message);
                    break;
                case 3://Contar el numero de partidas jugadas
                    Debug.Log(message);
                    break;
                case 4://Buscar una partida
                    Debug.Log(message);
                    break;
                case 5://Ver lista de conectados
                    Debug.Log(message);
                    break;
                case 6://cerrar sesion
                    Debug.Log(message);
                    break;
                case 7://Notificacion nuevo usuario conectado
                    Debug.Log(message);
                    break;
                case 8://Invitation Sent
                    Debug.Log(message);
                    break;
                case 9://Invitation Notification
                    Debug.Log(message);
                    break;
                case 10://response to my response to an invitation
                    Debug.Log(message);
                    break;
                case 11:
                    Debug.Log(message);
                    break;
                case 12:
                    Debug.Log(message);
                    break;
                case 13:
                    Debug.Log(message);
                    break;
                case 14:
                    Debug.Log(message);
                    break;
                case 15:
                    Debug.Log(message);
                    break;
                case 16:
                    Debug.Log("Ahora dentro del caso 16");
                    level_1.SendMessageToChat(message);
                    Debug.Log("Se acaba de llamar a la funcion SendMessageToChat");
                    break;
                case 18://recibimos respuseta del servidor
                    run2 = false;
                    if (message == "You won")
                        EndMessage.text = message;
                    else if (message == "You lost")
                        EndMessage.text = message;
                    else if (message == "It is a tie")
                        EndMessage.text = message;
                    else
                        EndMessage.text = message;
                    break;
                case 20:
                    if (message == "Close")
                        run2 = false;
                    break;
                default:
                    throw new ArgumentOutOfRangeException("Unknown value");
            }
        }
    }
    //Asynchronous function that runs until we join a game.
    //It listens and parses all the messages sent by the server.
    //Sometimes messages come concatenated so we split them by ~

    //Connects to the server only if its is not connected yet. Notifies an error if not able to.
    async private void ConnectToServer()
    {
        serverConnection = ServerConnection.GetInstance();
        if (!serverConnection.IsConnected())
        {
            int connectionResult = await Task.Run(() =>
            {
                return serverConnection.ConnectToServer();
            });
            if (connectionResult == 0)
            {
                serverConnection.SetConnected(true);
                Debug.Log("Conexion Exitosa");

                //Listen forever
                ListenForServer();
            }
            else if (connectionResult == -1)
            {
                //Si hay excepcion imprimimos error y salimos del programa con return 
                Debug.Log("Error al conectar con el servidor");
            }

        }
        //Server was already connected.
        //Go directly to the main menu
        else
        {
            ListenForServer();
            Debug.Log("Ya conectado al servidor");
        }
    }
    //Method executed when the application is quited. Sending a disconnection code to the server.
    void OnApplicationQuit()
    {
        Debug.Log("Application ending after " + Time.time + " seconds");
        Debug.Log("Cerrando conexion");
        serverConnection.DisconnectFromServer();
        Debug.Log("Conexion cerrada");
    }
    public void OnDisable()
    {
        SceneManager.sceneLoaded -= OnSceneLoaded;
    }
}
