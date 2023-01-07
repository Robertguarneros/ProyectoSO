using Client;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.SceneManagement;

public class FinalCoinMulti : MonoBehaviour
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
            AudioManager.Instance.ReproducirSonido(sonidoMoneda);
            gameManager.SumarPuntos(valor);
            gameManager.SetMultiplayerEndMessage();
            Time.timeScale = 0;
            EndMenu.SetActive(true);
            gameManager.SendFinalScore();
            Destroy(this.gameObject); //destruye el Game Object al qué pertenece este Script, es decir , la moneda
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
