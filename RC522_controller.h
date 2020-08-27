#if !defined RC522_CONTROLLER_H
#define RC522_CONTROLLER_H

#include <MFRC522.h> //library responsible for communicating with the module RFID-RC522
#include <SPI.h> //library responsible for communicating of SPI bus
#include "config.h"

namespace RC522Controller{

// Hardware pins
#define SS_PIN    21
#define RST_PIN   22
  
MFRC522 mfrc522(SS_PIN, RST_PIN);

/* Initialize the MFRC522 */
void Setup(){

  // Init SPI bus
  SPI.begin(); 

  // Init MFRC522
  Serial.println("Initializing the RFID card...");
  mfrc522.PCD_Init();
  // Print details
  mfrc522.PCD_DumpVersionToSerial();

  Serial.println("Initialization complete.");
  Serial.println();
}

/*  
 *  Read the RFID tag and return the ID
 *  For simplicity, we are only interested in the first 2 bytes of the UID
 *  In case nothing was read, return -1
 */
int Read(){

  // Find card
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return -1;
  }
  // Read card
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return -1;
  }
  
  //Compute ID (only first 2 bytes)
  int const id = 30621; //mfrc522.uid.uidByte[0] + mfrc522.uid.uidByte[1] << 8;
  
  return id;
}

} // namespace RC522Controller


#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
