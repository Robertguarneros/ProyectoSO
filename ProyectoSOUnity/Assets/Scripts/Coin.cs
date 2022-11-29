using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Coin : MonoBehaviour
{
    public int valor = 1;
    public GameManager gameManager;
    public AudioClip sonidoMoneda;
  
    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.CompareTag("Player"))
        {
            gameManager.SumarPuntos(valor);
            Destroy(this.gameObject); //destruye el Game Object al qué pertenece este Script, es decir , la moneda
            AudioManager.Instance.ReproducirSonido(sonidoMoneda);
        }
    }
}
