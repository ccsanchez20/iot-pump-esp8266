#include "./lib/WiFiManager/WiFiManager.h"
#include "./lib/WiFiManager/WiFiManager.cpp"

#define SHOW_WIFI_MANAGER_DEBUG   true
#define WIFI_MANAGER_PREFIX       "[WM] "
#define DARK_WIFI_MANAGER         true
#define CONFIGURATION_TIMEOUT     120      //seconds  
#define RECONNECTION_TIMEOUT      180      //seconds 

// For some unknown reason webserver can only be started once per boot up
// so webserver can not be used again in the sketch.
#define WIFI_CONNECT_TIMEOUT        60000L
#define WHILE_LOOP_DELAY            5000L
#define WHILE_LOOP_STEPS            ( WIFI_CONNECT_TIMEOUT / ( 3 * WHILE_LOOP_DELAY ) )

boolean configState;
boolean wifiConfigured = false;

WiFiManager wifiManager;

/* WiFi AP Configuration */
const char* ap_ssid     = device_name_char;
const char* ap_password = "12345678";
IPAddress ap_ip = IPAddress(192, 168, 1, 250);
IPAddress ap_gw = IPAddress(192, 168, 1, 1);
IPAddress ap_sn = IPAddress(255, 255, 255, 0);
IPAddress ap_dns1 = IPAddress(8, 8, 8, 8);

/* WiFi Client Configuration */
String routerSSID;
String routerPassword;
IPAddress ip;
IPAddress gw;
IPAddress sn;
IPAddress dns1;
IPAddress dns2;


/* Functions */
/* Get WiFi Configuration */
boolean getWiFiConfiguration(void) {
  routerSSID = wifiManager.getWiFiSSID();
  routerPassword = wifiManager.getWiFiPass();

  return wifiConfigured = true;
  
}
/* WiFi connection */
void setup_wifi() {
  wifiManager.setDebugOutput(SHOW_WIFI_MANAGER_DEBUG, WIFI_MANAGER_PREFIX);
  wifiManager.setDarkMode(DARK_WIFI_MANAGER);

  getWiFiConfiguration();
  
    wifiManager.setSTAStaticIPConfig(ap_ip, ap_gw, ap_sn, ap_dns1);
  
    if (!wifiManager.autoConnect(ap_ssid, ap_password)) {
      debug("failed to connect, we should reset as see if it connects");
      delay(3000);
      ESP.restart();
      delay(5000);
    }
  
    //if you get here you have connected to the WiFi
    debug("connected...yeey :)");
  
  
    debug("Local ip: "+WiFi.localIP().toString());  

}

/* WiFi reconnection */
void reconnect_wifi() {
  if (WiFi.status() != WL_CONNECTED) {
    debug("WiFi connection lost. Waiting before retry.");
    delay(RECONNECTION_TIMEOUT*1000);
    Serial.print("Reconectando WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(" .");
    }
    debug();
    debug("Conectado a red WiFi!");
    debug("Dirección IP: "+WiFi.localIP().toString());
  } 
}
