using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CharacterController : MonoBehaviour
{
    public float velocidad;
    public float fuerzaSalto;
    private Rigidbody2D rigidBody;
    private CapsuleCollider2D boxCollider;
    private bool mirandoDerecha = true;
    private Animator animator;

    
    private void Start()
    {
        rigidBody = GetComponent<Rigidbody2D>();
        animator = GetComponent<Animator>();
        boxCollider = GetComponent<CapsuleCollider2D>();
    }

    // Update is called once per frame
    void Update()
    {
        ProcesarMovimiento();
        ProcesarSalto();
    }
   
    void ProcesarSalto()
    {
        if (Input.GetKeyDown(KeyCode.Space) )
        {
                rigidBody.AddForce(Vector2.up * fuerzaSalto, ForceMode2D.Impulse);
        }
    }
    void ProcesarMovimiento()
    {
        //Logica de movimiento.
        float inputMovimiento = Input.GetAxis("Horizontal");

        if(inputMovimiento != 0f)
        {
            animator.SetBool("Running", true);
        }
        
        else
        {
            animator.SetBool("Running", false);
        }
        rigidBody.velocity = new Vector2(inputMovimiento * velocidad, rigidBody.velocity.y);

        GestionarOrientacion(inputMovimiento);
    }
    void GestionarOrientacion(float inputMovimiento)
    {
        //Si se cumple condicion
        if ( (mirandoDerecha == true && inputMovimiento < 0) || (mirandoDerecha == false && inputMovimiento > 0))
        {
            mirandoDerecha = !mirandoDerecha;
            transform.localScale = new Vector2(-transform.localScale.x, transform.localScale.y);

        }
            //Ejecutar codigo de volteado

    }
    
}

