/* 
 *  2 IO and 2 parameter = 4
 *  
 *  0 = btn status
 *  1 = relay status
 *  2 = ON time of pump (on seconds)
 *  3 = Enable pump time out 
 */
#define MAX_IO_QUANTITY 4

struct mqttValues
{
  /* flags 
   *  haveToSend
   *  haveToReceive
   */
  boolean haveToSend;
  boolean haveToReceive;
  unsigned int mqttAddr; // Dirección software
  /* VALUE TYPES:
   * 0 = boolean
   * 1 = int
   * 2 = float
   */
  unsigned int gpioValueType; //
  long period;                // Periodo
  String msg;                 // mqttValue
  String prevValue;
  long prevTime;
  /* Magnitudes:
    *  0 = none
    *  1 = temperature
    *  2 = humidity
    *  3 = luminosity
    *  4 = length
    *  5 = time
    */
  String magnitude;
  /* Magnitudes:
    *  0 = none
    *  1 = ºC
    *  2 = ºF
    *  3 = K
    *  4 = %
    *  5 = lux
    *  6 = m
    *  7 = cm
    *  8 = mm
    *  9 = "
    *  10 = s
    *  11 = min
    *  12 = h
    */
  String unit;
  String topic;
};

mqttValues mqttBuffer[MAX_IO_QUANTITY];

/*  
 *  0: GPIO btn --> 4 (DI5 fisical)
 *  1: GPIO relay --> 12
*/
unsigned int pushButton = 4; /* invertido */
unsigned int pinRelay = 12;
unsigned int gpios[MAX_IO_QUANTITY] = { pushButton, pinRelay };

/* VARIABLES */
boolean btnStatus = false;
boolean mqttStatus = false;
boolean pumpStatus = false;
boolean prevBtnStatus = false;
boolean prevMqttStatus = false;
boolean enablePumpTimeOut = PR1;
unsigned long pumpOnTimeMiliSeconds = PR0;
unsigned int pumpOnTimeSeconds = pumpOnTimeMiliSeconds / 1000;
unsigned long actualTime = 0;

/* io setup 
 *  0 = pump status
*/
void setupIO()
{
  for (int i = 0; i < MAX_IO_QUANTITY; i++)
  {
    /*  0 = button input (GPIO 4 (5))
     *  1 = relay output (GPIO 12)
     *  2 = pump-off param
     *  3 = enable auto pump-off param
     */
    if (i == 0)
    {
      pinMode(gpios[i], INPUT);
      mqttBuffer[i].mqttAddr = i;
      mqttBuffer[i].gpioValueType = 0;
      mqttBuffer[i].magnitude = "0";
      mqttBuffer[i].unit = "0";
      mqttBuffer[i].topic = device_name_str + "/value/" + mqttBuffer[i].mqttAddr;
      mqttBuffer[i].msg = "0";
    }
    else if (i == 1)
    {
      pinMode(gpios[i], OUTPUT);
      mqttBuffer[i].mqttAddr = i;
      mqttBuffer[i].gpioValueType = 0;
      mqttBuffer[i].magnitude = "0";
      mqttBuffer[i].unit = "0";
      mqttBuffer[i].topic = device_name_str + "/value/" + mqttBuffer[i].mqttAddr;
      mqttBuffer[i].msg = "0";
    }
    else if (i == 2)
    {
      mqttBuffer[i].mqttAddr = i;
      mqttBuffer[i].gpioValueType = 1;
      mqttBuffer[i].magnitude = "5";
      mqttBuffer[i].unit = "10";
      mqttBuffer[i].topic = device_name_str + "/value/" + mqttBuffer[i].mqttAddr;
      mqttBuffer[i].msg = String(pumpOnTimeMiliSeconds);
    }
    else if (i == 3)
    {
      mqttBuffer[i].mqttAddr = i;
      mqttBuffer[i].gpioValueType = 0;
      mqttBuffer[i].magnitude = "0";
      mqttBuffer[i].unit = "0";
      mqttBuffer[i].topic = device_name_str + "/value/" + mqttBuffer[i].mqttAddr;
      mqttBuffer[i].msg = String(enablePumpTimeOut);
    }
  }
}

boolean toBool(String value) {
    if (value == "1")
    {
      return true;
    }
    else
    {
      return false;
    }
  
}

void stringToValue(unsigned int index)
{
  switch (index)
  {
  case 1: // bit
    mqttStatus = toBool(mqttBuffer[index].msg);
    debug("Valor de mqtt: " + String(mqttStatus));
    break;
  case 2: // unsigned int
    pumpOnTimeSeconds = mqttBuffer[index].msg.toInt();
    pumpOnTimeMiliSeconds = pumpOnTimeSeconds * 1000;
    debug("Estado de time out: " + String(pumpOnTimeSeconds));
    break;
  case 3: // bit
    enablePumpTimeOut = toBool(mqttBuffer[index].msg);
    debug("Estado de habilitación de time out: " + String(enablePumpTimeOut));
    break;
  }
}

void processMqttValue(unsigned int index){
  if (index == 1){
    stringToValue(index);
  }
}

void dataToSend() {
  mqttBuffer[1].haveToSend = true;
  mqttBuffer[1].msg = String(pumpStatus);
  mqttBuffer[2].haveToSend = true;
  mqttBuffer[2].msg = String(pumpOnTimeSeconds)+"/"+mqttBuffer[2].magnitude+";"+mqttBuffer[2].unit;
}

void main_loop()
{
  btnStatus = digitalRead(gpios[0]);
  if ((!btnStatus && prevBtnStatus) || (mqttStatus && !prevMqttStatus))
  {
    debug("Encendido bomba");
    prevBtnStatus = btnStatus;
    prevMqttStatus = mqttStatus;
    pumpStatus = true;
    digitalWrite(gpios[1], HIGH);
    mqttBuffer[1].msg = "1";
    mqttBuffer[1].haveToSend = true;
    actualTime = millis();
    if (!btnStatus) {
      mqttBuffer[0].msg = "1";
      mqttBuffer[0].haveToSend = true;      
    }
  }
  else if ((btnStatus && !prevBtnStatus) || (!mqttStatus && prevMqttStatus))
  {
    debug("Apagado bomba");
    prevBtnStatus = btnStatus;
    prevMqttStatus = mqttStatus;
    pumpStatus = false;
    digitalWrite(gpios[1], LOW);
    mqttBuffer[1].msg = "0";
    mqttBuffer[1].haveToSend = true;
    if (btnStatus) {
      mqttBuffer[0].msg = "0";
      mqttBuffer[0].haveToSend = true;      
    }
  }

  if (pumpStatus && ((millis() - actualTime) > pumpOnTimeMiliSeconds) && enablePumpTimeOut)
  {
    debug("Apagado bomba por límite de tiempo");
    actualTime = millis();
    pumpStatus = false;
    digitalWrite(gpios[1], LOW);
    mqttBuffer[1].msg = "0";
    mqttBuffer[1].haveToSend = true;
    if (prevMqttStatus) mqttStatus = prevMqttStatus = false;
    if (!prevBtnStatus) btnStatus = prevBtnStatus = false;
  }
  delay(500);
}
