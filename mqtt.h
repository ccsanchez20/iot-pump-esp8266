

/* MQTT Configuration */
const char *mqtt_server = "app.cristiancs.com";
const int mqtt_port = 1883;
String clientId = device_name_str;
const char *mqtt_user = device_name_char;
const char *mqtt_pass = "electronica";


/* MQTT reconnection */
void reconnect_mqtt() {
  while (!mqttClient.connected()) {
    debug("Intentando conexión Mqtt...");
    mqttClient.connect(clientId.c_str(),mqtt_user,mqtt_pass);
    // Intentamos conectar
    if (mqttClient.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
      debug("Conectado!");
      // Nos suscribimos
      mqttClient.subscribe(getValuesTopic);
      mqttClient.subscribe(setValueTopic);
    } else {
      debug("falló :( con error -> "+String(mqttClient.state()));
      debug(" Intentamos de nuevo en 5 segundos");

      delay(5000);
    }
  }
}

void setup_mqtt(){
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);
}
