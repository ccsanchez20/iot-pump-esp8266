/* ESTRUCTURA:
 * * Type value: 0 = not isolated ; 1 = isolated
 * * Value addr: only for isolated (not isolated has a secuencial addr)
 * * period: 0 = periodical ; 1 = on change
 * * Isolated / not isolated
*/

/* MQTT SINTAX
   - Suscripción a todas las salidas a controlar
      Topic en evento ---> deviceId/output/addr
   - Publicación periódica (o frente a eventos) de valores leídos en las entradas
      Topic periodico ---> deviceId/values
      Topic en evento ---> deviceId/input/addr
   - Update values
      Topic para solicitar datos ---> deviceId/getValues
   - Alternativa para inputs/outputs  ---> deviceId/dio/addr
*/
#include <PubSubClient.h>

#define MAX_TOPIC_SIZE 50
#define MAX_PAYLOAD_SIZE 50

WiFiClient espClient;

PubSubClient mqttClient(espClient);

/*
  String getValueTopic = "getValues";
  String input0Topic = "output/0";
  String input1Topic = "output/1";
  boolean mqttInputStatus;
*/

long lastMsg = 0;
char msg[MAX_PAYLOAD_SIZE];
char topic[MAX_TOPIC_SIZE];


/* Gloabl topics */
String getValueTopicStr = device_name_str + "/getValues";
String getValueTopic = getValueTopicStr.c_str();

/*
  boolean haveToSend = true;
  boolean update = false;
  String valuesTopic = device_name_str + "/values";
*/

void sendValues()
{
  for (int i = 0; i < MAX_IO_QUANTITY; i++)
  {
    if (mqttBuffer[i].haveToSend)
    {
      mqttBuffer[i].msg.toCharArray(msg, MAX_PAYLOAD_SIZE);
      mqttBuffer[i].topic.toCharArray(topic, MAX_TOPIC_SIZE);
      debug("Datos a publicar -> " + String(msg));
      mqttClient.publish(topic, msg, true);
      mqttBuffer[i].haveToSend = false;
    }
  }
}

void receiveValues()
{

  for (int i = 0; i < MAX_IO_QUANTITY; i++)
  {
    if (mqttBuffer[i].haveToReceive)
    {
      mqttBuffer[i].msg.toCharArray(msg, MAX_PAYLOAD_SIZE);
      mqttBuffer[i].topic.toCharArray(topic, MAX_TOPIC_SIZE);
      debug("Datos a publicar -> " + String(msg));
      mqttClient.publish(topic, msg, true);
      mqttBuffer[i].haveToSend = false;
    }
  }
}

void checkMqttMsg()
{
  sendValues();
  receiveValues();
}

/* TOPIC MATCH */
void topicMatch(String topic, String msg)
{
  //String prefix = device_name_str + "/";
  //topic.replace(prefix,"");
  Serial.println("Topic recibido: " + topic);
  if (topic == getValueTopic)
  {
    dataToSend();
  }
  else
  {
    for (int i = 0; i < MAX_IO_QUANTITY; i++)
    {
      if (topic == mqttBuffer[i].topic)
      {
        mqttBuffer[i].haveToReceive = true;
        mqttBuffer[i].msg = msg;
      }
    }
  }
}


/* SUBSCRIBE TOPICS */
//String topic1 = clientId + "/bomba";
//String topic2 = clientId + "/electrovalvula";

/* MQTT Subscriber callback */
void callback(char *topic, byte *payload, unsigned int length)
{
  String incoming = "";
  debug("Mensaje recibido desde -> " + String(topic));
  for (int i = 0; i < length; i++)
  {
    incoming += (char)payload[i];
  }
  debug("Mensaje -> " + incoming);
  topicMatch(String(topic), incoming);
  incoming.trim();
  debug("Mensaje después de trim -> " + incoming);
}
