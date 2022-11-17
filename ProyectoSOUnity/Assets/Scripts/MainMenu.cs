using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Net;
using System.Net.Sockets;
using Unity.VisualScripting.Antlr3.Runtime.Tree;
using UnityEngine.UI;
using TMPro;
using System.Text;
using UnityEngine.SceneManagement;
using Unity.VisualScripting;
using Client;

namespace MainMenuUI
{

    public class MainMenu : MonoBehaviour
    {   
        //variables generales
        private ServerConnection serverConnection;
        //variables main menu
        public GameObject MainMenuUI;
        

        //variables de la ventana de login
        public GameObject LoginUI;
        
        public GameObject connectedImage;
        public GameObject errorLabel;
        public GameObject disconnectedImage;

        public TextMeshProUGUI registradoLabel;
        public TextMeshProUGUI loginSuccesfulLabel;

        public TMP_InputField UsernameReg;
        public TMP_InputField PasswordReg;
        public TMP_InputField NameReg;
        public TMP_InputField UsernameLog;
        public TMP_InputField PasswordLog;
        
        

        //variables del menu despues de iniciar sesion
        public GameObject LoggedInUI;
        
        public TextMeshProUGUI resultCountGames;
        public TextMeshProUGUI resultViewScore;
        public TextMeshProUGUI ConnectedUserListLbl;
        public TextMeshProUGUI logoutLbl;

        public GameObject logoutLblObj;
        public GameObject resultsViewScoreLabel;
        
        public TMP_InputField GameIDInput;
        
        //aqui empiezan las funcions y metodos

        void Start()//ejecutada al iniciar escena
        {
            serverConnection = ServerConnection.GetInstance();
        }
        public void ExitGameBtn()//Funcion para salir del juego y cerrar la conexion con el servidor
        {
            Application.Quit();
        }
        public void RegisterUser()//metodo para registrar un usuario nuevo
        {
            //enviamos al servidor el nombre, usuario y contrasena
            string mensaje = "1/" + UsernameReg.text + "/" + PasswordReg.text + "/" + NameReg.text;
            Debug.Log(mensaje);
            serverConnection.SendMessage(mensaje);
            Debug.Log("Enviado");
        }
        public void Login()//metodo para inicar sesion
        {
            //enviamos al servidor el usuario y contrasena
            string mensaje = "2/" + UsernameLog.text + "/" + PasswordLog.text;
            Debug.Log(mensaje);
            serverConnection.SendMessage(mensaje);
            Debug.Log("Enviado");
        }
        public void CountGames()//metodo para contar el numero de partidas jugadas
        {
            string mensaje = "3/";
            Debug.Log(mensaje);
            serverConnection.SendMessage(mensaje);
            Debug.Log("Enviado");

        }
        public void ViewScore()//metodo para ver el resultado de una partida
        {
            string mensaje = "4/" + GameIDInput.text;
            Debug.Log(mensaje);
            serverConnection.SendMessage(mensaje);
            Debug.Log("Enviado");
        }
        public void GetConnectedUsers()//metodo para ver la lista de usuarios conectados
        {
            string mensaje = "5/";
            Debug.Log(mensaje);
            serverConnection.SendMessage(mensaje);
            Debug.Log("Enviado");
        }
        public void Logout()//metodo para cerrar sesion 
        {
            string mensaje = "6/";
            Debug.Log(mensaje);
            serverConnection.SendMessage(mensaje);
            Debug.Log("Cerrando Sesion");
        }
    }
    
}