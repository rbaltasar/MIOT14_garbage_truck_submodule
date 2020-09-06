
#if !defined CONFIG_H
#define CONFIG_H

const uint8_t kNumWiFiAttempts = 5;
const uint32_t kNumIterationsConnect = 100;

const char* mqtt_server = "192.168.2.118";
const char* mqtt_id_topic = "garbage_truck/container_id";
#endif
