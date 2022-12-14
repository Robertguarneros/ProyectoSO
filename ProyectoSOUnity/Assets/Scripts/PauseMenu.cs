using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using Client;
using TMPro;

public class PauseMenu : MonoBehaviour
{

    private ServerConnection serverConnection;
    
    public GameObject pauseMenu;
    public bool isPaused;
    public GameManager gameManager;
    
    // Start is called before the first frame update
    void Start()
    {
        serverConnection = ServerConnection.GetInstance();
        pauseMenu.SetActive(false);
        isPaused = false;
    }

    // Update is called once per frame
    void Update()
    {

        if (Input.GetKeyDown(KeyCode.Escape))
        {
            if (isPaused)
            {
                Resume();
            }
            else
            {
                Pause();
            }
        }
    }
    public void Pause()
    {
        pauseMenu.SetActive(true);
        Time.timeScale = 0;
        isPaused = true;
    }
    public void Resume()
    {
        pauseMenu.SetActive(false);
        Time.timeScale = 1;
        isPaused = false;
    }
    public void GoToMainMenu()
    {
        gameManager.SendFinalScoreAbandono();
        Time.timeScale = 1;
        string mensaje = "6/";
        Debug.Log(mensaje);
        serverConnection.SendMessage(mensaje);
        Debug.Log("Cerrando Sesion");
        SceneManager.LoadScene("MainMenu");
    }
    public void QuitGame()
    {
        gameManager.SendFinalScoreAbandono();
        serverConnection.DisconnectFromServer();
        Application.Quit();
    }
    public void GoToMainMenuEnd()
    {
        Time.timeScale = 1;
        string mensaje = "6/";
        Debug.Log(mensaje);
        serverConnection.SendMessage(mensaje);
        Debug.Log("Cerrando Sesion");
        SceneManager.LoadScene("MainMenu");
    }
    public void QuitGameEnd()
    {
        serverConnection.DisconnectFromServer();
        Application.Quit();
    }
}
