#include <rom/rtc.h>
#include <WiFi.h>
#include "config.h"
#include "OTA_updater_ESP32.h"
#include "network_credentials.h"

/* ------------------------ */
/* --- Global variables --- */
/* ------------------------ */

/* Web Server for OTA softare updates */
OTAUpdater_ESP32 updater;
/* Loop count to attempt WiFi connection */
uint8_t iteration = 0;

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

}


/* Send gathered data to the cloud */
void SendData(){
  Serial.println("Data sent!");
}

/* Delete gathered data */
void DeleteData(){
  Serial.println("Data deleted!");
}

/* Attempt to read RFID tag */
void ReadRfid(){
  
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
  if(iteration == kNumIterations){
    bool connected = ConnectWifi();
    if(connected){
      // Send data
      SendData();
      // Erase data
      DeleteData();
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

  Serial.print("Iteration: ");
  Serial.println(iteration);
  
  delay(100);
}
