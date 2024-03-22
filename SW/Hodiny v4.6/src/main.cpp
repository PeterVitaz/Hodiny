/* ************************************************************************************
 *   Hodiny v4.6
 *   Odlahcena verzia iba hodiny s hlavnym displejom a kruh LED
 * 
 * ************************************************************************************/

#include <Arduino.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include <WiFi.h>
#include "time.h"

// Global variables

bool sdCard = false;
bool internetStatus = false;

// Variable to store Name and Password WIFI
char ssid[15];
char password[15];

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

// ************************************************************************************
//
//      Declarations
//
// ************************************************************************************

// Read file from SD card
// Return:  1 - Failed to open file for reading
//          0 - Reading successfull

int readFile(fs::FS &fs, const char * path, char ssid[], char password[]);

void printLocalTime();

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
  
Serial.print(ssid);
Serial.print("x");
Serial.print(password);
Serial.print("x");
Serial.println("x");

  // We can connect to the internet
  if (sdCard == true) {
    
    Serial.print("Connecting to ");
    Serial.println(ssid);
    
    // ************************************************************************************

    WiFi.begin(ssid, password);

    // try to log in to the internet, if it doesn't work in 10 seconds, close it.
    int i = 50;

    while ( WiFi.status() != WL_CONNECTED ) {
      delay(500);

      i--;

      Serial.print(".");
      if ( i == 0) {
        break;
      }
    }

    if (i == 0) {  

      Serial.println("");
      Serial.println("Connection failed!");
      internetStatus = false;
        
    } else {

      Serial.println("");
      Serial.println("WiFi connected.");

      // Init and get the time
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      printLocalTime();

      //disconnect WiFi as it's no longer needed
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
    }
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

void printLocalTime(){
  
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {

    Serial.println("Failed to obtain time");
    return;

  }

  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");

  char timeHour[3];

  strftime(timeHour,3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();

}