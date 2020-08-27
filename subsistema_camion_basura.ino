#include <rom/rtc.h>
#include <WiFi.h>
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

void setup() {
  Serial.begin(115200);

  /* Setup the hardware */
  SetupHardware(); 
  Serial.println("Finished setup");
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
    Serial.println(WiFi.localIP()); // Send the IP address of the ESP8266 to the computer
    retval = true;
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

/* Transmit a single ID to the cloud */
void TransmitId(int const id){
  
}

/* Send gathered data to the cloud */
void SendData(){

  // Extract ids from the container until empty
  while(!id_container.empty()){
    int const id = id_container.pop(); // This action returns the ID and removes it from the container
    TransmitId(id);
  }
  
  Serial.println("Data sent!");
}

/* Insert a new ID into the id_container avoiding duplicates */
void ProcessID(int const id){

  // Check if the ID has been read before
  bool const found = id_container.find(id);

  if(found){
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
    bool connected = ConnectWifi();
    if(connected){
      // Send data
      SendData();
      // Check updates
      bool updates_available = CheckUpdates();
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
