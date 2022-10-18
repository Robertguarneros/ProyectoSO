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
    public static string username;
    
    public GameObject connectedLabel;
    public GameObject errorLabel;
    public GameObject disconnectLabel;

    public TextMeshProUGUI registradoLabel;
    public TextMeshProUGUI loginSuccesfulLabel;

    public TMP_InputField UsernameReg;
    public TMP_InputField PasswordReg;
    public TMP_InputField NameReg;
    public TMP_InputField UsernameLog;
    public TMP_InputField PasswordLog;

    int puerto = 9063;

    //Funcion para conectarse al servidor
    public void Connect()
    {
        //Creamos un IPEndPoint con el ip del servidor y puerto del servidor 
        //al que deseamos conectarnos
        IPAddress direc = IPAddress.Parse("192.168.56.102");
        IPEndPoint ipep = new IPEndPoint(direc, puerto);

        //Creamos el socket 
        server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        try
        {
            server.Connect(ipep);//Intentamos conectar el socket
            connectedLabel.SetActive(true);
            disconnectLabel.SetActive(false);
            errorLabel.SetActive(false);
            Debug.Log("Conexion Exitosa");

        }
        catch (SocketException)
        {
            //Si hay excepcion imprimimos error y salimos del programa con return 
            errorLabel.SetActive(true);
            Debug.Log("Error al conectar con el servidor");
            return;
        }

    }
    //Funcion para desconectarse del servidor
    public void Disconnect()
    {
        //Mensaje desconexion
        string mensaje = "0/Goodbye";

        byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
        server.Send(msg);

        // Nos desconectamos
        try
        {
            server.Shutdown(SocketShutdown.Both);
            server.Close();
            connectedLabel.SetActive(false);
            disconnectLabel.SetActive(true);
            Debug.Log("Desconexion Exitosa");
        }
        catch (SocketException)
        {
            Debug.Log("Error al desconectar con el servidor");
        }
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
                username = UsernameLog.text;
                Debug.Log(username);
                SceneManager.LoadScene(SceneManager.GetActiveScene().buildIndex + 1);
                loginSuccesfulLabel.text = "Credenciales correctas";
            }
            else if (mensaje == "Password Incorrecto")
                loginSuccesfulLabel.text = mensaje;
            else if (mensaje == "Username Incorrecto o No Registrado")
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
