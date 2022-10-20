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

    public GameObject connectedLabel;
    public GameObject errorLabel;
    public GameObject disconnectLabel;
    public GameObject reconnectButton;

    public TextMeshProUGUI registradoLabel;
    public TextMeshProUGUI loginSuccesfulLabel;

    public TMP_InputField UsernameReg;
    public TMP_InputField PasswordReg;
    public TMP_InputField NameReg;
    public TMP_InputField UsernameLog;
    public TMP_InputField PasswordLog;

    //Funcion para conectarse al servidor
    public void Connect()
    {
        int puerto = 9050;
        bool conexionCorrecta =false;
        //Creamos un IPEndPoint con el ip del servidor y puerto del servidor 
        //al que deseamos conectarnos
        IPAddress direc = IPAddress.Parse("192.168.56.102");
        server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        while (conexionCorrecta ==false)
        {
            IPEndPoint ipep = new(direc, puerto);
            //Creamos el socket 
            try
            {
                server.Connect(ipep);//Intentamos conectar el socket
                connectedLabel.SetActive(true);
                disconnectLabel.SetActive(false);
                errorLabel.SetActive(false);
                conexionCorrecta = true;
                conectado = true;
                Debug.Log("Conexion Exitosa en puerto " + puerto);
            }
            catch (SocketException)
            {
                //Si hay excepcion imprimimos error y salimos del programa con return 
                errorLabel.SetActive(true);
                Debug.Log("Error al conectar con el servidor");
            }
            if (conexionCorrecta == false)
            {
                if (puerto == 9050)
                {
                    puerto = 9051;
                }
                else if (puerto == 9051)
                    puerto = 9052;
                else
                    puerto = 9050;
            }

        }

    }
    //Funcion para desconectarse del servidor
    public void Disconnect()
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
            connectedLabel.SetActive(false);
            disconnectLabel.SetActive(true);
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
        while (conectado == false)
        {
            Connect();
        }
    }
    private void Update()
    {
        if (conectado == false)
        {
            disconnectLabel.SetActive(true);
            reconnectButton.SetActive(true);
        }
    }
    //Funcion para salir del juego y cerrar la conexion con el servidor
    public void ExitGameBtn()
    {
        Disconnect();
        Application.Quit();

    }
    //Primera consulta, registrar un usuario nuevo
    public void RegisterUser()
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
    //Funcion para inicar sesion
    public void Login()
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
