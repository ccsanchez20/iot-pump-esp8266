/* 
 *  2 IO and one parameter = 3
 *  
 *  0 = relay status
 *  1 = btn status
 *  2 = ON time of pump (on seconds)
 */
#define MAX_IO_QUANTITY 3

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
unsigned int pumpOnTimeSeconds = 3600;
unsigned long pumpOnTimeMiliSeconds = pumpOnTimeSeconds * 1000;
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
     */
    if (i == 0)
    {
      pinMode(gpios[i], INPUT);
      mqttBuffer[i].mqttAddr = i;
      mqttBuffer[i].gpioValueType = 0;
      mqttBuffer[i].magnitude = "0";
      mqttBuffer[i].unit = "0";
      mqttBuffer[i].topic = device_name_str + "/dio/" + mqttBuffer[i].mqttAddr;
    }
    else if (i == 1)
    {
      pinMode(gpios[i], OUTPUT);
      mqttBuffer[i].mqttAddr = i;
      mqttBuffer[i].gpioValueType = 1;
      mqttBuffer[i].magnitude = "0";
      mqttBuffer[i].unit = "0";
      mqttBuffer[i].topic = device_name_str + "/dio/" + mqttBuffer[i].mqttAddr;
    }
    else
    {
      mqttBuffer[i].mqttAddr = i;
      mqttBuffer[i].gpioValueType = 1;
      mqttBuffer[i].magnitude = "5";
      mqttBuffer[i].unit = "10";
      mqttBuffer[i].topic = device_name_str + "/parameter/" + mqttBuffer[i].mqttAddr;
    }
  }
}

void stringToValue(unsigned int index)
{
  switch (mqttBuffer[index].gpioValueType)
  {
  case 1: // bit
  bool b;
    if (mqttBuffer[index].msg == "1")
    {
      b = true;
    }
    else
    {
      b = false;
    }
    mqttStatus = b;
    break;
  case 2: // unsigned int
    pumpOnTimeSeconds = mqttBuffer[index].msg.toInt();
    pumpOnTimeMiliSeconds = pumpOnTimeSeconds * 1000;
    break;
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
  if ((!btnStatus && prevBtnStatus) || (mqttStatus && !prevMqttStatus))
  {
    prevBtnStatus = btnStatus;
    prevMqttStatus = mqttStatus;
    pumpStatus = true;
    digitalWrite(gpios[1], HIGH);
    mqttBuffer[1].msg = "1";
    mqttBuffer[1].haveToSend = true;
  }
  else if ((btnStatus && !prevBtnStatus) || (!mqttStatus && prevMqttStatus))
  {
    prevBtnStatus = btnStatus;
    prevMqttStatus = mqttStatus;
    pumpStatus = false;
    digitalWrite(gpios[1], LOW);
    mqttBuffer[1].msg = "0";
    mqttBuffer[1].haveToSend = false;
  }

  if (pumpStatus && ((millis() - actualTime) > pumpOnTimeMiliSeconds))
  {
    actualTime = millis();
    pumpStatus = false;
    digitalWrite(gpios[1], LOW);
    mqttBuffer[1].msg = "0";
    mqttBuffer[1].haveToSend = false;
  }
}
