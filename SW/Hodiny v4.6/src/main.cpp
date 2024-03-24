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
#include <Wire.h>
// Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)
//#include <U8g2lib.h>
#include <U8x8lib.h>
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE); 

#define U8LOG_WIDTH 16
#define U8LOG_HEIGHT 8
uint8_t u8log_buffer[U8LOG_WIDTH*U8LOG_HEIGHT];
U8X8LOG u8x8log;

// Global variables

bool sdCard = false;
bool internetStatus = false;

// Variable to store Name and Password WIFI
char ssid[15];
char password[15];

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

int hours = 0;
int minutes = 0;
int seconds = 0;

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

void getTime();

// ************************************************************************************
//
//      SETUP
//
// ************************************************************************************

void setup() {
  
  delay(500);
  
  Serial.begin(115200);
  
  // Oled SSD1306 init I2C
  // work as terminal, update with newline
  // 16 character per line
  u8x8.begin();
  u8x8.setFont(u8x8_font_victoriamedium8_r);
  
  u8x8log.begin(u8x8, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
  u8x8log.setRedrawMode(0);		// 0: Update screen with newline, 1: Update screen for every char
  u8x8log.print("\f");
  u8x8log.print("Starting...\n");
  u8x8log.print("\n");
  u8x8log.print("Hodiny v4.6\n");
  u8x8log.print("\n");
  
  // Read SD card first
  if (!SD.begin(5)) {

    Serial.println("Card Mount Failed");
    u8x8log.print("Card Mount Fail\n");
    sdCard = false;
    //return;

  } else {

    uint8_t cardType = SD.cardType();
    
    if (cardType == CARD_NONE) {

      Serial.println("No SD card attached");
      u8x8log.print("No SD card\n");

      sdCard = false;
      //return;

    } else {

      Serial.print("SD Card Type: ");
      u8x8log.print("SD Card: ");

      if (cardType == CARD_MMC) {

        Serial.println("MMC");
        u8x8log.print("MMC\n");

      } else if (cardType == CARD_SD) {

        Serial.println("SDSC");
        u8x8log.print("SDSC\n");

      } else if (cardType == CARD_SDHC) {

        Serial.println("SDHC");
        u8x8log.print("SDHC\n");

      } else {

        Serial.println("UNKNOWN");
        u8x8log.print("xxx\n");

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
  

  // We can connect to the internet
  if (sdCard == true) {
    
    Serial.print("Connecting to ");
    Serial.println(ssid);
    u8x8log.print("Connecting to: \n");
    u8x8log.print(ssid);
    u8x8log.print("\n");

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
      u8x8log.print("Connection fail\n");
                    
      internetStatus = false;
        
    } else {

      Serial.println("");
      Serial.println("WiFi connected.");
      u8x8log.print("WiFi connected.\n");

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
  getTime();
  Serial.print(hours);
  Serial.print(":");
  Serial.print(minutes);
  Serial.print(":");
  Serial.print(seconds);
  Serial.print("\n");
  delay(1000);
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

void getTime() {
  
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {

    Serial.println("Failed to obtain time");
    return;

  }

  hours = timeinfo.tm_hour;
  minutes = timeinfo.tm_min;
  seconds = timeinfo.tm_sec;
}