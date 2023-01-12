using System;
using System.Threading.Tasks;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using TMPro;
using MainMenuUI;
using Client;
using System.Collections;
using Unity.VisualScripting;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using UnityEngine.Events;
using Level_1UI;


public class MessageParser : MonoBehaviour
{
    private MainMenu mainMenu;
    private ServerConnection serverConnection;

    public TextMeshProUGUI UserJoinedNotificationtxt;

    void Awake()
    {
        
        SceneManager.sceneLoaded += OnSceneLoaded;
    }

    //start is called before the first frame update
    //it gets all the unity objects needed on this script, makes the infolog invisible
    //get an instance of the server connection
    //set the button events
    //executes the function to connect to the server for the first time
    void OnSceneLoaded(Scene scene, LoadSceneMode mode)
    {
        Debug.Log("Loading Message_Parser");

        GameObject mainMenuWindow = GameObject.Find("MainMenu");//Obtenemos la informacion de la ventana del menu principal antes de inicar sesion
        mainMenu = mainMenuWindow.GetComponent<MainMenu>();

        ConnectToServer();
    }
    async private void ListenForServer()
    {//formato de los mensaje codigo/mensaje
        bool run = true;
        while (run)
        {
            string serverResponse = await Task.Run(() => serverConnection.ListenForMessage());

            Debug.Log(serverResponse);
            string[] pieces = serverResponse.Split('/');
            int code = Convert.ToInt32(pieces[0]);
            string message = pieces[1].Split('\0')[0];
            Debug.Log(message);

            Debug.Log("Start of Switch");
            switch (code)
            {
                case 0:
                    if (message == "Cerrando Sesion")
                    {
                        run = false;
                    }
                        break;
                case 1://registro de usuarios
                    if (message == "Registrado Correctamente")
                        mainMenu.registradoLabel.text = "Registration Successful\nGo back to login";
                    else if (message == "Username is taken, pick another Username")
                        mainMenu.registradoLabel.text = message;
                    else
                        mainMenu.registradoLabel.text = message;
                    break;
                case 2://login 
                    if (message == "Login Correcto")
                    {
                        mainMenu.LoginUI.SetActive(false);
                        mainMenu.LoggedInUI.SetActive(true);
                        mainMenu.loginSuccesfulLabel.text = "Login Successful";
                        mainMenu.loginLabel.SetActive(true);
                    }
                    else if (message == "Password Incorrecto")
                        mainMenu.loginSuccesfulLabel.text = "Wrong Password";
                    else if (message == "Username Incorrecto o No Registrado")
                        mainMenu.loginSuccesfulLabel.text = "Username incorrect or not registered";
                    else if (message == "Servidor lleno, intenta mas tarde")
                        mainMenu.loginSuccesfulLabel.text = "Server full please try later";
                    else if (message == "Usuario ya conectado, intenta con otro usuario")
                        mainMenu.loginSuccesfulLabel.text = "User is already connected, try with another user";
                    else
                        mainMenu.loginSuccesfulLabel.text = "Error when logging in please try again";
                    mainMenu.loginLabel.SetActive(true);
                    break;
                case 3://Contar el numero de partidas jugadas
                    if (message == "Username Incorrecto o No Registrado")
                        mainMenu.resultCountGames.text = "Username incorrect or not registered";
                    else if (message == "No se encontraron partidas")
                        mainMenu.resultCountGames.text = "No matches were found";
                    else
                        mainMenu.resultCountGames.text = "Found " + message + " matches";
                    break;
                case 4://Buscar una partida
                    if (message == "No se encontraron partidas")
                    {
                        mainMenu.resultViewScore.text = "No matches found";
                        mainMenu.resultsViewScoreLabel.SetActive(true);
                    }
                    else
                    {
                        mainMenu.resultViewScore.text = message;
                        mainMenu.resultsViewScoreLabel.SetActive(true);
                    }
                    break;
                case 5://Ver lista de conectados
                    if (message == "No hay otros usuarios")
                        mainMenu.ConnectedUserListLbl.text = "No users online, only you";
                    else
                        mainMenu.ConnectedUserListLbl.text = message;
                    break;
                case 6://cerrar sesion
                    if (message == "Desconectado")
                    {
                        mainMenu.MainMenuUI.SetActive(true);
                        mainMenu.LoggedInUI.SetActive(false);
                        mainMenu.loginSuccesfulLabel.text = "Login Again";
                        mainMenu.UsernameLog.text = "";
                        mainMenu.PasswordLog.text = "";
                    }
                    else if (message == "Error al desconectar")
                    {
                        mainMenu.logoutLbl.text = "Error when logging out";
                        mainMenu.logoutLblObj.SetActive(true);
                    }
                    break;
                case 7://Notificacion nuevo usuario conectado
                    StartCoroutine(ShowUserJoinedNotification(message,5));
                    break;
                case 8://Invitation Sent
                    if (message == "Invite Sent")
                    {
                        mainMenu.invitationResponse.text = "Invitation Sent, waiting for response";
                        mainMenu.invitationResponseObj.SetActive(true);
                    }
                    else if (message == "Username does not exist or is not connected")
                    {
                        mainMenu.invitationResponse.text = message;
                        mainMenu.invitationResponseObj.SetActive(true);
                    }
                    else if (message == "Waiting for response")
                    {
                        mainMenu.invitationResponse.text = message;
                        mainMenu.invitationResponseObj.SetActive(true);
                    }
                    else if (message == "Cant invite yourself")
                    {
                        mainMenu.invitationResponse.text = "You cannot invite yourself";
                        mainMenu.invitationResponseObj.SetActive(true);
                    }
                    break;
                case 9://Invitation Notification
        
                    mainMenu.UserThatSentInvite = message;
                    mainMenu.UserHasInvitedYouLbl.text ="New Invite from "+ message;
                    mainMenu.InvitationBox.SetActive(true);
                    break;
                case 10://response to my response to an invitation
                    Debug.Log("Llego"+message);
                    break;
                case 11:
                    if (message == "Invitation Accepted")
                    {
                        mainMenu.SendMatchID(pieces[2]);
                        mainMenu.invitationResponse.text = "Invitation Accepted";
                        mainMenu.invitationResponseObj.SetActive(true);
                        run = false;
                        SceneManager.LoadScene("Level_1_Multiplayer", LoadSceneMode.Single);//regreasar a level1
                    }
                    else if (message == "Invitation Rejected")
                    {
                        mainMenu.invitationResponse.text = "Invitation Rejected";
                        mainMenu.invitationResponseObj.SetActive(true);
                    }
                    else
                    {
                        mainMenu.invitationResponse.text = "Invitation Error";
                        mainMenu.invitationResponseObj.SetActive(true);
                    }
                    break;
                case 12:
                    Debug.Log("MatchID"+message);
                    run = false;//esto para el message parser al recibir el match id
                    SceneManager.LoadScene("Level_1_Multiplayer", LoadSceneMode.Single);
                    break;
                case 13:
                    mainMenu.GetConnectedUsers();
                    break;
                case 14:
                    run = false;
                    SceneManager.LoadScene("Level_1_Solo",LoadSceneMode.Single);
                    Debug.Log(message);
                    break;
                case 15:
                    Debug.Log(message);
                    break;
                case 16:
                    Debug.Log(message);
                    break;
                case 18:
                    Debug.Log(message);
                    break;
                case 21: //user deleted
                    if (message == "User deleted")
                    {
                        mainMenu.UserDelSuccesful.text = "User has been deleted";
                        mainMenu.UserSuccesfulDel.SetActive(true);
                    }
                    else if (message == "Error, try again")
                    {
                        mainMenu.UserDelSuccesful.text = "Error when deleting user, try again";
                        mainMenu.UserSuccesfulDel.SetActive(true);
                    }
                    break;
                default:
                    throw new ArgumentOutOfRangeException("Unknown value");
            }
        }
    }
    IEnumerator ShowUserJoinedNotification(string message, float delay)
    {
        UserJoinedNotificationtxt.enabled = true;
        UserJoinedNotificationtxt.text = message;
        yield return new WaitForSeconds(delay);
        UserJoinedNotificationtxt.enabled = false;
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
                mainMenu.connectedImage.SetActive(true);
                mainMenu.disconnectedImage.SetActive(false);
                mainMenu.errorLabel.SetActive(false);
                Debug.Log("Conexion Exitosa");

                //Listen forever
                ListenForServer();
            }

            else if (connectionResult == -1)
            {
                //Si hay excepcion imprimimos error y salimos del programa con return 
                mainMenu.errorLabel.SetActive(true);
                Debug.Log("Error al conectar con el servidor");
            }

        }
        //Server was already connected.
        //Go directly to the main menu
        else
        {
            ListenForServer();
            mainMenu.connectedImage.SetActive(true);
            mainMenu.disconnectedImage.SetActive(false);
            mainMenu.errorLabel.SetActive(false);
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
    void OnDestroy()
    {
        Debug.Log("Destroyed...");
    }
}
