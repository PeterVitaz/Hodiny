/* ************************************************************************************
 *   Hodiny v4.6
 *   Odlahcena verzia iba hodiny s hlavnym displejom a kruh LED
 * 
 * ************************************************************************************/

#include <Arduino.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"

// Global variables

bool sdCard = false;
bool internet = true;

// Variable to store Name and Password WIFI
char ssid[15];
char password[15];

// ************************************************************************************
//
//      Declarations
//
// ************************************************************************************

// Read file from SD card
// Return:  1 - Failed to open file for reading
//          0 - Reading successfull

int readFile(fs::FS &fs, const char * path, char ssid[], char password[]);

// ************************************************************************************
//
//      SETUP
//
// ************************************************************************************

void setup() {

  Serial.begin(115200);

  // Read SD card first
  if (!SD.begin(5)) {

    Serial.println("Card Mount Failed");
    sdCard = false;
    //return;

  } else {

    uint8_t cardType = SD.cardType();
    
    if (cardType == CARD_NONE) {

      Serial.println("No SD card attached");
      sdCard = false;
      //return;

    } else {

      Serial.print("SD Card Type: ");

      if (cardType == CARD_MMC) {

        Serial.println("MMC");

      } else if (cardType == CARD_SD) {

        Serial.println("SDSC");
      
      } else if (cardType == CARD_SDHC) {

        Serial.println("SDHC");

      } else {

        Serial.println("UNKNOWN");
      }
      
      // Read ssid and password from SD Card
      if ( readFile(SD, "/wifi.txt", ssid, password) == 1 ) {
        
        sdCard = false;
        //return;

      } else {
        
        sdCard = true;

      }
    }
  }

  // End of Reading SD card
  // if it was successful sdCard = true
  // if not sdCard = false
  if (sdCard == false) {
    Serial.println("I don't have SSID and PASSWORD");
  } else {
    Serial.println("I have SSID and PASSWORD");
    
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}

// ************************************************************************************
//
//      Definitions
//
// ************************************************************************************


int readFile (fs::FS &fs, const char * path, char ssid[], char password[]) {

    String LineString = "";
    
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);

    if (!file) {

        Serial.println("Failed to open file for reading");
        return 1;

    }

    Serial.println("Read from file: ");
    Serial.println();
    
    while (file.available()) {

        LineString = file.readStringUntil(',');
        LineString.toCharArray(ssid, LineString.length()+1);
        Serial.println(ssid);

        LineString = file.readStringUntil(',');
        LineString.toCharArray(password, LineString.length()+1); 
        Serial.println(password); 

    }
    
    file.close();

    return 0;
}