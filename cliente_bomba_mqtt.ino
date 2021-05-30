/***************************************
 *              LIBRARIES              
 ***************************************/
#include "resource.h"

void setup() {
  configureDebug();

  delay(2000);

  debug();
  debug(" - - - INICIO DEL PROGRAMA - - -");
  
  setup_wifi();
  delay(1000);
  setup_mqtt();
  debug("Device name: "+device_name_str);
  setupIO();
}

void loop() {
  reconnect_wifi();
  reconnect_mqtt();

  main_loop();
  checkMqttMsg();

  mqttClient.loop();


}
