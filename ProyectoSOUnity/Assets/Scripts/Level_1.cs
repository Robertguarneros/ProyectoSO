using Client;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

namespace Level_1UI
{
    public class Level_1 : MonoBehaviour
    {
        //variables generales
        private ServerConnection serverConnection;
        public int maxMessage = 25;

        public List<Message> messageList = new List<Message>();
        public GameObject chatPanel, textObject;
        public TMP_InputField chatBox;
        public void Start()
        {
            serverConnection = ServerConnection.GetInstance();
        }
        public void Update()
        {
            if (chatBox.text != "")
            {
                if (Input.GetKeyDown(KeyCode.Return))
                {
                    string mensaje = "15/" + chatBox.text;
                    Debug.Log(mensaje);
                    serverConnection.SendMessage(mensaje);
                    Debug.Log("Enviado");
                    chatBox.text = "";
                }
            }
            else
            {
                if (!chatBox.isFocused && Input.GetKeyDown(KeyCode.Return))
                {
                    chatBox.ActivateInputField();
                }
            }
            if (!chatBox.isFocused)
            {
                //    if (Input.GetKeyDown(KeyCode.Space))
                //    {
                //        SendMessageToChat("You pressed space");
                //    }
            }
        }
        public void SendMessageToChat(string text)
        {
            if (messageList.Count >= maxMessage)
            {
                Destroy(messageList[0].textObject.gameObject);
                messageList.Remove(messageList[0]);
            }
            Message newMessage = new Message();
            newMessage.text = text;

            GameObject newText = Instantiate(textObject, chatPanel.transform);

            newMessage.textObject = newText.GetComponent<TextMeshProUGUI>();

            newMessage.textObject.text = newMessage.text;

            messageList.Add(newMessage);
        }

    }
    [System.Serializable]
    public class Message
    {
        public string text;
        public TextMeshProUGUI textObject;
    }
}