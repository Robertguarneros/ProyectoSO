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

public class MainMenu : MonoBehaviour
{
    public static Socket server;
    public static bool conectado;
    public static bool loggedin;

    public GameObject connectedImage;
    public GameObject errorLabel;
    public GameObject disconnectedImage;
    public GameObject reconnectButton;
    
    public TextMeshProUGUI registradoLabel;
    public TextMeshProUGUI loginSuccesfulLabel;

    public TMP_InputField UsernameReg;
    public TMP_InputField PasswordReg;
    public TMP_InputField NameReg;
    public TMP_InputField UsernameLog;
    public TMP_InputField PasswordLog;

   
    public void Connect() //Funcion para conectarse al servidor
    {
        int puerto = 50000;
        bool conexionCorrecta =false;
        //Creamos un IPEndPoint con el ip del servidor y puerto del servidor 
        //al que deseamos conectarnos
        IPAddress direc = IPAddress.Parse("147.83.117.22");
        server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        while (conexionCorrecta ==false)
        {
            IPEndPoint ipep = new(direc, puerto);
            //Creamos el socket 
            try
            {
                server.Connect(ipep);//Intentamos conectar el socket
                connectedImage.SetActive(true);
                disconnectedImage.SetActive(false);
                errorLabel.SetActive(false);
                conexionCorrecta = true;
                conectado = true;
                Debug.Log("Conexion Exitosa");
            }
            catch (SocketException)
            {
                //Si hay excepcion imprimimos error y salimos del programa con return 
                errorLabel.SetActive(true);
                Debug.Log("Error al conectar con el servidor");
            }
        }
    }
    
    public void Disconnect()//Funcion para desconectarse del servidor
    {
        //Mensaje desconexion
        string mensaje = "0/";

        byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
        server.Send(msg);

        // Nos desconectamos
        try
        {
            server.Shutdown(SocketShutdown.Both);
            server.Close();
            connectedImage.SetActive(false);
            disconnectedImage.SetActive(true);
            conectado = false;
            Debug.Log("Desconexion Exitosa");
        }
        catch (SocketException)
        {
            Debug.Log("Error al desconectar con el servidor");
        }
    }

    void Start()//ejecutada al iniciar escena
    {
        if (loggedin)
        {
            Disconnect();//se agrega para que se desconecte el usuario al presionar logout(al iniciar la escena)
            loggedin = false;
        }
        if (conectado == false)
        {
            Connect();
        }
    }
   
    private void Update()//funcion que se ejecuta cada frame para verificar la conexion
    {
        if (conectado == false)
        {
            disconnectedImage.SetActive(true);
            reconnectButton.SetActive(true);
        }
    }
    
    public void ExitGameBtn()//Funcion para salir del juego y cerrar la conexion con el servidor
    {
        Disconnect();
        Application.Quit();

    }
    
    public void RegisterUser()//Primera consulta, registrar un usuario nuevo
    {
        string mensaje = "1/" + UsernameReg.text + "/" + PasswordReg.text + "/" + NameReg.text;
        Debug.Log(mensaje);
        //Try para evitar ensenar mensaje de que el usuario no se ha conectado al servidor
        try
        {
            // Enviamos al servidor el nombre, usuario y contrasena
            byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);

            //Recibimos la respuesta del servidor
            byte[] msg2 = new byte[80];
            server.Receive(msg2);
            mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];
            //Si el registro fue exitoso se muestra el mensaje
            Debug.Log(mensaje);//mensaje en consola para ver que regresa el servidor
            if (mensaje == "Registrado Correctamente")
                registradoLabel.text = "Registrado Correctamente\nInicia Sesion";
            else if (mensaje == "Username ya existe, escoge otro Username")
                registradoLabel.text = mensaje;
            else
                registradoLabel.text = mensaje;
        }
        catch
        {
            registradoLabel.text = "No estas conectado al servidor";
        }
    }
    
    public void Login()//Funcion para inicar sesion
    {
        string mensaje = "2/" + UsernameLog.text + "/" + PasswordLog.text;
        Debug.Log(mensaje);
        try
        {
            // Enviamos al servidor el nombre, usuario y contrasena
            byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);

            //Recibimos la respuesta del servidor
            byte[] msg2 = new byte[80];
            server.Receive(msg2);
            mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];
            Debug.Log(mensaje);//mensaje en consola para ver que regresa el servidor
            if (mensaje == "Login Correcto") 
            {
                loginSuccesfulLabel.text = "Credenciales correctas";
                SceneManager.LoadScene(SceneManager.GetActiveScene().buildIndex + 1);
                loggedin = true;
            }
            else if (mensaje == "Password Incorrecto")
                loginSuccesfulLabel.text = mensaje;
            else if (mensaje == "Username Incorrecto o No Registrado")
                loginSuccesfulLabel.text = mensaje;
            else if (mensaje == "Servidor lleno, intenta mas tarde")
                loginSuccesfulLabel.text = mensaje;
            else
                loginSuccesfulLabel.text = "Error al iniciar sesion, intenta nuevamente";
        }
        catch
        {
            loginSuccesfulLabel.text = "No estas conectado al servidor";
        }
    }
    
}
