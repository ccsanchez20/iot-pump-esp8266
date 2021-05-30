
  values ioValues[MAX_IO_QUANTITY];
  valuesTopics ioMQTT[MAX_IO_QUANTITY];

  boolean b;
  int intValue;
  float floatValue;

  /* OUTPUTS */
  /* This program only has one digital output */

  void checkNewValues() {
    for (int i = 0; i < MAX_IO_QUANTITY; i++) {
      if (ioValues[i].mqttType) {
        checkPeriodicalValue(i);
      } else {
        checkOnChangeValue(i);
      }
    }
  }

  void checkPeriodicalValue(int index) {
    if (((millis() - ioValues[index].prevTime) > ioValues[index].period)) {
      /* read values */
      /* public */
      /* update time */
      ioValues[index].prevTime = millis();
    }
  }


  void checkOnChangeValue(int index) {
    /* Check if have to public on mqtt */
    if (ioValues[index].value != ioValues[index].prevValue) {
      /* public value */
      /* public */
      /* update time */
      ioValues[index].prevValue = ioValues[index].value;
    }

    /* Check if has received new value on mqtt subscription */
    if (ioMQTT[index].newValue) {
      /* resetValue */
      ioMQTT[index].newValue = false;
      /* receive value */
      ioValues[index].value = ioMQTT[index].msg;
      stringToValue(index);
      /* update time */
      ioValues[index].prevValue = ioValues[index].value;
    }
  }

  void stringToValue(int index) {
    switch (ioValues[index].gpioValueType) {
      case 0: // bit
        if (ioValues[index].value == "1") {
          b = HIGH;
        } else {
          b = LOW;
        }
        digitalWrite(ioValues[index].gpioAddr, b);
        break;
      case 1: // integrer
        intValue = ioValues[index].value.toInt();
        mapValueToOutput(intValue);
        break;
      case 2: // float
        floatValue = ioValues[index].value.toFloat();
        mapValueToOutput(floatValue);   /* Middleware entre esto y la lógica del programa */
        break;
    }
  }
