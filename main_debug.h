/* HABILITACIÓN DE DEBUG EN EL CÓDIGO PRINCIPAL */
#define SHOW_MAIN_DEBUG true
#define BAUDRATE        115200

String debugPrefix;

void configureDebug(String prefix = "") {
  #ifdef SHOW_MAIN_DEBUG
    Serial.begin(BAUDRATE);
    debugPrefix = prefix;
  #endif
}

void debug(String text=" ") {
  #ifdef SHOW_MAIN_DEBUG
    Serial.println(debugPrefix + text);
  #endif
}
