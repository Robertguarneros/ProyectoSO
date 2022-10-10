using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using System.Net;
using System.Net.Sockets;
using Unity.VisualScripting.Antlr3.Runtime.Tree;

public class MainMenu : MonoBehaviour
{
    Socket server;
    public GameObject connectedLabel;
    public GameObject errorLabel;
    public void Connect()
    {
        //Creamos un IPEndPoint con el ip del servidor y puerto del servidor 
        //al que deseamos conectarnos
        IPAddress direc = IPAddress.Parse("192.168.56.102");
        IPEndPoint ipep = new IPEndPoint(direc, 9081);

        //Creamos el socket 
        server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        try
        {
           server.Connect(ipep);//Intentamos conectar el socket
            connectedLabel.SetActive(true);
            errorLabel.SetActive(false);
            
        }
        catch (SocketException ex)
        {
            //Si hay excepcion imprimimos error y salimos del programa con return 
            errorLabel.SetActive(true);
            return;
        }

    }
}
