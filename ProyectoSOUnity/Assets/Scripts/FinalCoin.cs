using Client;
using System;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.SceneManagement;

public class FinalCoin : MonoBehaviour
{
    public int valor = 10;
    public GameManager gameManager;
    public AudioClip sonidoMoneda;
    private ServerConnection serverConnection;

    public GameObject EndMenu;
    public TextMeshProUGUI EndMessage;

    // Start is called before the first frame update
    void Start()
    {
        serverConnection = ServerConnection.GetInstance();
        EndMenu.SetActive(false);
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.CompareTag("Player"))
        {
            gameManager.SetSoloPlayerEndMessage();
            gameManager.SumarPuntos(valor);
            Destroy(this.gameObject); //destruye el Game Object al qué pertenece este Script, es decir , la moneda
            Time.timeScale = 0;
            //EndMessage.text = "You have reached the end, Checking Scores...";
            EndMenu.SetActive(true);
            AudioManager.Instance.ReproducirSonido(sonidoMoneda);
        }
    }
    public void GoToMainMenu()
    {
        Time.timeScale = 1;
        string mensaje = "6/";
        Debug.Log(mensaje);
        serverConnection.SendMessage(mensaje);
        Debug.Log("Cerrando Sesion");
        SceneManager.LoadScene("MainMenu");
    }
    public void QuitGame()
    {
        serverConnection.DisconnectFromServer();
        Application.Quit();
    }
}
