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
String pubValuesMsg = "";
bool setValues = true;


/* Gloabl topics */
String getValuesTopicStr = device_name_str + "/getValues";
String setValuesTopicStr = device_name_str + "/setValues";
const char *getValuesTopic = getValuesTopicStr.c_str();
String setValuesTopic = setValuesTopicStr.c_str();
String getValueTopicStr = device_name_str + "/getValue/#";
String getValueTopic = getValueTopicStr.c_str();
String setValueTopicStr = device_name_str + "/setValue/#";
const char *setValueTopic = setValueTopicStr.c_str();

/*
  boolean haveToSend = true;
  boolean update = false;
  String valuesTopic = device_name_str + "/values";
*/

void sendValues()
{
  if (setValues) {
    for (int i = 0; i < MAX_IO_QUANTITY; i++)
    {
      pubValuesMsg+=(mqttBuffer[i].msg+"/");
    }
    pubValuesMsg.toCharArray(msg, MAX_PAYLOAD_SIZE);
    setValuesTopic.toCharArray(topic, MAX_TOPIC_SIZE);
    debug("Datos a publicar -> " + String(msg));
    mqttClient.publish(topic, msg, true);
    setValues = false;
    pubValuesMsg = "";
  } else {
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
}

void receiveValues()
{

  for (int i = 0; i < MAX_IO_QUANTITY; i++)
  {
    if (mqttBuffer[i].haveToReceive)
    {
      stringToValue(i);
      mqttBuffer[i].haveToReceive = false;
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
  if (topic == getValuesTopicStr)
  {
    debug("Recibida solicitud de actualización de estado");
    setValues = true;
  }
  else {
    String prefix = device_name_str + "/setValue/";
    int pos=topic.indexOf(prefix);
    if (pos>=0)
    {
      debug("Recibido valor de parámetro");
      topic.replace(prefix,"");
      for (int i = 0; i < MAX_IO_QUANTITY; i++)
      {
        if (topic.toInt() == mqttBuffer[i].mqttAddr)
        {
          mqttBuffer[i].haveToReceive = true;
          mqttBuffer[i].msg = msg;
        }
      }
    } else {
      debug("Recibido valor extraño");      
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
  //incoming.trim();
  //debug("Mensaje después de trim -> " + incoming);
}
