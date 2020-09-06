#include <rom/rtc.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#include "OTA_updater_ESP32.h"
#include "network_credentials.h"
#include "RC522_controller.h"
#include "IDContainer.h"

/* ------------------------ */
/* --- Global variables --- */
/* ------------------------ */

/* Web Server for OTA softare updates */
OTAUpdater_ESP32 updater;
/* Loop count to attempt WiFi connection */
uint8_t iteration = 0;
/* Container of read IDs */
IDContainer id_container;
/* MQTT connectivity */
WiFiClient espClient;
PubSubClient m_client;

/* Initial setup of the microcontroller */
void setup() {
  Serial.begin(115200);

  /* Setup the hardware */
  SetupHardware(); 
  Serial.println("Finished setup");
}

/* Connect to the MQTT broker */
void ConnectMQTT(){
  Serial.println("Connecting to MQTT broker");
  
  m_client.setClient(espClient);
  m_client.setServer(mqtt_server, 1883);
  m_client.loop();
  // Connect to the broker
  Serial.print("Attempting MQTT connection..");
  while(!m_client.connected()){
    Serial.print(".");
    if (m_client.connect("TestID")) //Unique name for each instance of a garbage truck
    {
      Serial.println(".");
      Serial.println("connected");
    } else {
      // Attempt connection until success (only for prototype!)
      delay(1000);
    }
  }
}

/* Setup WiFi connection */
bool ConnectWifi()
{
  bool retval = false;
  
  WiFi.begin(ssid, password); // Connect to the network. Use network_credentials.h
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println(" ...");

  for(int i=0; i<kNumWiFiAttempts; i++){
    if(WiFi.status() == WL_CONNECTED) break;
    delay(200);
  }

  if(WiFi.status() == WL_CONNECTED){
    Serial.println("Connection established!");
    Serial.println(WiFi.localIP());
    retval = true;

    // Connect to MQTT broker only if there is data to send
    if(!id_container.empty()){
      ConnectMQTT();
    }
  }

  return retval;
  
}

/* Disconnect WiFi connection */
void DisConnectWifi(){
  WiFi.disconnect();
  Serial.println("Disconnected from WLAN");
}

/* Setup the LED controller */
void SetupHardware()
{  
  RC522Controller::Setup();
}

/* Transmit a single ID to the MQTT broker */
void TransmitId(int const id){
  Serial.print("Transmitting id ");
  Serial.println(id);

  m_client.publish(mqtt_id_topic, String(id).c_str());
  m_client.loop(); // MQTT client must be fed in order to send events
  delay(10);
}

/* Send gathered data */
void SendData(){

  // Extract ids from the container until empty
  while(!id_container.empty()){
    int const id = id_container.pop(); // This action returns the ID and removes it from the container
    TransmitId(id);
    delay(200); // Delay to allow the backend to receive and forward the data
  }
  
  Serial.println("Data sent!");
}

/* Insert a new ID into the id_container avoiding duplicates */
void ProcessID(int const id){

  // Check if the ID has been read before
  bool const found = id_container.find(id);

  if(!found){
    // Add the ID to the container
    id_container.push(id);
  }
}

/* Attempt to read RFID tag */
void ReadRfid(){
  
  int const result = RC522Controller::Read();
  if(result != -1){
    Serial.print("Detected container with ID: ");
    Serial.println(result);

    //Process the new read ID
    ProcessID(result);
  }
}

/* Create an OTA software update web server and wait for updates */
void EnableOTAUpdate(){
  
  // Check during 10 seconds for updates
  updater.begin(kTruckName);
  Serial.println("Waiting for OTA software update");
  for(int i=0; i< (kOTATimeoutMs/10); i++){
    updater.OTA_handle();
    delay(10);
  }

  // This line will only be reached in case of no SW update
  Serial.println("OTA SW update timeout. No update request was received");
}

/*  This is a dummy function as the protocol for checking SW updates is
 *  not implemented in the prototype.
 *  This function should communicate with the cloud to ask for SW updates.
 *  In case of available updates, this function will return true.
 */
bool CheckUpdates(){
  /* Dummy */
  return false;
}

void loop() {

  // Try to read the RFID tag
  ReadRfid();

  // Every kNumIterations iterations of the main loop, try to connect to WiFi
  if(iteration == kNumIterationsConnect){
    bool const connected = ConnectWifi();
    if(connected){
      // Send data
      if(!id_container.empty()){
        SendData();
      }
      // Check updates
      bool const updates_available = CheckUpdates();
      if(updates_available){
        EnableOTAUpdate();
      }
      // Disconnect
      DisConnectWifi();
    }
    iteration = 0;
  }
  
  iteration++;
  
  delay(100);
}
