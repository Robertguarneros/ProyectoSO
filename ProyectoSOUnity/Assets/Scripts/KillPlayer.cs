using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class KillPlayer : MonoBehaviour
{

    public int valor = -1;
    public GameManager gameManager;
    [SerializeField] Transform spawnPoint;

    private void OnCollisionEnter2D(Collision2D collision)
    {
        if (collision.transform.CompareTag("Player"))
        {
            gameManager.SumarPuntos(valor);
            collision.transform.position = spawnPoint.position;
        }
    }
}
