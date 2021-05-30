uint32_t device_ID = ESP.getChipId();
String device_name_str = String(device_ID) + "_client";
const char *device_name_char = device_name_str.c_str();
