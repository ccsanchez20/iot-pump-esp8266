/* Values and topics 
 *  TOPIC = /chipID/valor 0,valor 1,valor 2...
 *  0 = Temperatura
 *  1 = Humedad
 *  2 = Luminosidad
 *  
*/



/*
void sendFlag(unsigned int index){
    haveToSend = true;
    mqttValues[index].newValue = true;
    mqttValues[index].msg = ioValues[index].value;
}


void readValues(){
  // read the input pin:
  int buttonState = digitalRead(pushButton);
  // print out the state of the butto
  if ( !buttonState && !statusBtn ) {
    digitalWrite(pinRelay,HIGH);
    statusBtn = 1;
    Serial.println(statusBtn);
    delay(500);        // delay in between reads for stability   
    ioValues[0].value = String(statusBtn);
    sendFlag(0);
  } else if ( buttonState && statusBtn ) {    
    digitalWrite(pinRelay,LOW);
    statusBtn = 0;
    Serial.println(statusBtn);
    delay(500);        // delay in between reads for stability   
    ioValues[0].value = String(statusBtn);
    haveToSend = true;
    sendFlag(0);
  } else if (requiredUpdate()){
    ioValues[0].value = String(statusBtn);
    haveToSend = true;
    sendFlag(0);    
  }
}

void setOutputStatus(){
  if (pumpStatus) digitalWrite(pinRelay,HIGH);
  else digitalWrite(pinRelay,LOW);
}

void program(){
  readValues();
  setOutputStatus();
  checkUpadteMsg();
}*/
