/* LUMINOSIDAD */
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
void displaySensorDetails(void);
void configureSensor(void);

void setup_lum_sensor(){
  /* LUMINOSIDAD */
  if(!tsl.begin())
  {
    /* There was a problem detecting the TSL2561 ... check your connections */
    debug("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  displaySensorDetails();
  configureSensor();
}

int readLuminosity(){
  /* Get a new sensor event */ 
  sensors_event_t event;
  tsl.getEvent(&event);
 
  /* Display the results (light is measured in lux) */
  if (event.light)
  {
    debug(String(event.light)+" lux");
  }
  else
  {
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
    debug("Sensor overload");
  }
  
  return event.light;
}

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t sensor;
  tsl.getSensor(&sensor);
  debug("------------------------------------");
  debug("Sensor:       "+String(sensor.name));
  debug("Driver Ver:   "+String(sensor.version));
  debug("Unique ID:    "+String(sensor.sensor_id));
  debug("Max Value:    "+String(sensor.max_value)+" lux");
  debug("Min Value:    "+String(sensor.min_value)+" lux");
  debug("Resolution:   "+String(sensor.resolution)+" lux");
  debug("------------------------------------");
  debug();
  delay(500);
}

/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2561
*/
/**************************************************************************/
void configureSensor(void)
{
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */  
  debug("------------------------------------");
  debug("Gain:         Auto");
  debug("Timing:       13 ms");
  debug("------------------------------------");
}
