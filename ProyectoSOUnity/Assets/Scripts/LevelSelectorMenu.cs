using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Net;
using System.Net.Sockets;
using Unity.VisualScripting.Antlr3.Runtime.Tree;
using UnityEngine.UI;
using TMPro;
using UnityEditor.VersionControl;
using System.Text;
using UnityEngine.SceneManagement;
using System.IO.Pipes;

public class LevelSelectorMenu : MonoBehaviour
{
    Socket server;
    public TextMeshProUGUI resultCountGames;
    public TextMeshProUGUI resultViewScore;
    public TMP_InputField GameIDInput;
    //para viewScores
    public GameObject resultsViewScoreLabel;

    public void CountGames()
    {
        server = MainMenu.server;
        
        string mensaje = "3/";
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
            if (mensaje == "-1")
                resultCountGames.text = "No se han encontrado juegos";
            else
                resultCountGames.text = "Se han encontrado " + mensaje + " juegos";
        }
        catch
        {
            resultCountGames.text = "No estas conectado al servidor";
        }
    }
    public void ViewScore()
    {
        string mensaje = "4/" + GameIDInput.text;
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
            if (string.IsNullOrEmpty(mensaje))
            {
                resultViewScore.text = "No se ha encontrado el juego";
                resultsViewScoreLabel.SetActive(true);
            }
            else
            {
                resultViewScore.text = mensaje;
                resultsViewScoreLabel.SetActive(true);
            }
        }
        catch
        {
            resultCountGames.text = "No estas conectado al servidor";
        }
    }
    public void Logout()
    {
        SceneManager.LoadScene("MainMenu", LoadSceneMode.Single);
    }
}