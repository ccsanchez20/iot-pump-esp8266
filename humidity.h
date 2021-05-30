/* HUMEDAD */
const int sensorPin = A0;

int readHumidity(){
  int humedad = analogRead(sensorPin);
  int humedad_p = map(humedad,0,1023,100,0);
  debug(String(humedad)+" - "+String(humedad_p)+"%");

  return humedad_p;
}
