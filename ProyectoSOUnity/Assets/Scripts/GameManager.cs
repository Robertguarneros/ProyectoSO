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


public class GameManager : MonoBehaviour
{
    private ServerConnection serverConnection;
    public int PuntosTotales { get { return puntosTotales; } }
    private int puntosTotales;

    void Awake()
    {
        ConnectToServer();
    }
    public void SumarPuntos(int puntosASumar)
    {
        puntosTotales += puntosASumar;
        Debug.Log(puntosTotales);
    }
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
}
