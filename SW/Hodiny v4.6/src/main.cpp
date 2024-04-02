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

// ***** Please set the number of devices you have *****
// But the maximum default of 8 MAX72XX wil also work.
//
// 7segment Led Control 
// pin 27 is connected to the DataIn 
// pin 25 is connected to the CLK 
// pin 26 is connected to LOAD 
// 2x MAX7219
//
// Modified to work for ESP32
#include <LedControl.h>
LedControl lc=LedControl(27,25,26,2);

/* we always wait a bit between updates of the display */
unsigned long delaytime=500;

#define BUTTON_PIN 4 // GPIO04 pin connected to button



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


// 7 segment code for max7219
const byte num[11] ={ B01111110,   // 0 0x7E
                      B00110000,   // 1 0x30
                      B01101101,   // 2 0x6D
                      B01111001,   // 3
                      B00110011,   // 4
                      B01011011,   // 5
                      B01011111,   // 6
                      B01110000,   // 7
                      B01111111,   // 8
                      B01111011,    // 9
                      B00000000    // off
              };

int lastStateBtn = HIGH; // the previous state from the input pin
int currentStateBtn;     // the current reading from the input pin

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

void sendToDisp1(int hour, int minute, int second, byte led1, byte led2);

void sendSec(int second, int minute);

void updateTimeNTP();

void setTimezone(String timezone);

// ************************************************************************************
//
//      SETUP
//
// ************************************************************************************

void setup() {
  
  delay(500);
  
  Serial.begin(115200);
  
  // Set pullup to D04
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Oled SSD1306 init I2C
  // work as terminal, update with newline
  // 16 character per line
  u8x8.begin();
  u8x8.setFont(u8x8_font_victoriamedium8_r);
  
  u8x8log.begin(u8x8, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
  u8x8log.setRedrawMode(1);		// 0: Update screen with newline, 1: Update screen for every char
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
      //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      configTime(0, 0, "pool.ntp.org");    // First connect to NTP server, with 0 TZ offset
      setTimezone("CET-1CEST,M3.5.0,M10.5.0/3");
      printLocalTime();

      internetStatus = true;

      //disconnect WiFi as it's no longer needed
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
    }

    
  }

  // ************************************************************************************ 
  // Init devices for MAX7219 
  // we have already set the number of devices when we created the LedControl
  int devices=lc.getDeviceCount();
  //we have to init all devices in a loop
  for(int address=0;address<devices;address++) {
    /*The MAX72XX is in power-saving mode on startup*/
    lc.shutdown(address,false);
    /* Set the brightness to a medium values */
    lc.setIntensity(address,15);
    /* and clear the display */
    lc.clearDisplay(address);
  }
  
  u8x8.clearDisplay();
 
}

// ************************************************************************************
//
//      Main Loop
//
// ************************************************************************************
  


void loop() {
  // put your main code here, to run repeatedly:
  int oldSeconds = seconds;
  
  getTime();
  
  if (oldSeconds == 59 && seconds == 0) {
    sendToDisp1(hours, minutes, seconds, B00001111,B11111111);
    sendSec(seconds, minutes);
  } else if (seconds > oldSeconds) {
    sendToDisp1(hours, minutes, seconds, B00001111,B11111111);
    sendSec(seconds, minutes);
  }

  // Read state of Button
  currentStateBtn = digitalRead(BUTTON_PIN);
  
  //Serial.println(currentStateBtn);
  if(lastStateBtn == LOW && currentStateBtn == HIGH){
    Serial.println("The state changed from LOW to HIGH");
    updateTimeNTP();
  }
  // save the last state
  lastStateBtn = currentStateBtn;  
  delay(100);
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


// ************************************************************************************
// Display to 1. led display
// DateTime, led colon, led dial
void sendToDisp1(int hour, int minute, int second, byte led1, byte led2) {
  //if (setting.getDisp2_ON() == false) led2 = 0;
  
  byte dispBuff[8] = {led1,0,0,0,0,0,0,led2};    // buffer send to 1. disp
  byte dispBuffTemp[8] = {0,0,0,0,0,0,0,0};      // temporary buff used to transpose

  
  dispBuff[1] = num[second % 10];   // Seconds 1
  dispBuff[2] = num[second / 10];   // Seconds 10
  dispBuff[3] = num[minute % 10];   // Minutes 1
  dispBuff[4] = num[minute / 10];   // Minutes 10
  dispBuff[5] = num[hour % 10];     // Hours   1
  dispBuff[6] = num[hour / 10];     // Hours   10

  //Transpose
  for(byte i=0;i<8;i++){
    for(byte j=0;j<8;j++){
      if( (dispBuff[i] & 1<<j) != 0)
        dispBuffTemp[j]=dispBuffTemp[j] | (1<<i);
    }
  }
  
  // send to display
  for(byte i=0;i<8;i++){
    lc.setRow(0,i,dispBuffTemp[i]);
  }
}

// Show LED RING 
void sendSec(int second, int minute){
  // Clear buffer
  byte disp[8]={B00000000,
                B00000000,
                B00000000,
                B00000000,
                B00000000,
                B00000000,
                B00000000,
                B00000000};

  // Get second as byte and check if is 0-59 
  if (second > 59){
    Serial.println("Chyba v sekundach!!");
    return;
  }
  
  byte row = second / 8;
  byte rowIndex = second % 8;
  Serial.println(second);
  Serial.print("Row = ");
  Serial.println(row);
  Serial.print("RowIndex = ");
  Serial.println(rowIndex);

  if (row == 0 && rowIndex == 0 ){
    
    for(byte i = 0; i <8; i++){
      disp[i] = B00000000;
    }
    // inner LED ring switch ON
    disp[7] |= B00001111;
    

  } else {
    for(byte i = 0; i < row; i++ ){
      disp[i] = B11111111;
    }
    disp[row] = ~(B11111111>>rowIndex);
    // inner LED ring switch ON
    disp[7] |= B00001111;
  }
  
  // if minutes is ODD, bitwise NOT
  if ( (minute % 2) != 0 ) {
    for (int i = 0; i < 8; i++){
      disp[i] = ~disp[i];
    }
    disp[7] |= B00001111;
  } 
  
  // send to led ring
  for(byte i = 0; i < 8; i++){
      if (i <7)
        lc.setColumn(1,i + 1, disp[i]);
      else
        lc.setColumn(1,0,disp[i]);
  }
}

void updateTimeNTP() {

  u8x8log.print("Connecting to: \n");
  u8x8log.print(ssid);
  u8x8log.print("\n");
  
  WiFi.begin(ssid, password);

  // try to log in to the internet, if it doesn't work in 10 seconds, close it.
  int i = 50;

  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);

    i--;

    Serial.print(".");
    u8x8log.print(".");
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
      // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      configTime(0, 0, "pool.ntp.org");    // First connect to NTP server, with 0 TZ offset
      setTimezone("CET-1CEST,M3.5.0,M10.5.0/3");


      printLocalTime();
      u8x8log.print("Time updated.\n");
      internetStatus = true;
      
      //disconnect WiFi as it's no longer needed
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
    }
    delay(500);
    u8x8.clearDisplay();
}


// Setup TIMEZONE
void setTimezone(String timezone){
  Serial.printf("  Setting Timezone to %s\n",timezone.c_str());
  setenv("TZ",timezone.c_str(),1);  //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
  tzset();
}