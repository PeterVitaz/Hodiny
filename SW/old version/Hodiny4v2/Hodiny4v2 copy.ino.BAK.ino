/*
 *  Hodiny v4.2
 *  verzia bez BME280
 *  LEDControl library has been modified
 */

// ************************************************************************************
// Import required libraries
#include "FS.h"
#include "SD.h"
#include <SPI.h>

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans9pt7b.h>

#include <Adafruit_Sensor.h>
//#include <Adafruit_BME280.h>

// Library for MAX7219
#include "LedControl.h"

#include "uRTCLib.h"

//#include "5WaySwitch.h"

#include "alarm.h"
#include "clockTime.h"
#include "setting.h"

// ************************************************************************************
//#define SEALEVELPRESSURE_HPA (1013.25)

// ************************************************************************************
// Buttun def
//#define BUTTON_PIN  34
//#define SENSOR_PIN  35
//#define BUZZER_PIN  33

// Buzzer setting
//int freq = 800;
//int channel = 0;
//int resolution = 8;
//int dutyCycle = 25;

// 5 way switch
//Button button;

// auxiliary variables for keeping time
ClockTime clockTime;
//Alarm alarm1;
//Alarm alarm2;
Setting setting;

// Placeholders on HTML page
const char* PARAM_INPUT_1  = "timeSetting";
const char* PARAM_INPUT_2  = "timeSync";
// const char* PARAM_INPUT_3  = "alarm1";
// const char* PARAM_INPUT_4  = "A1_on";
// const char* PARAM_INPUT_5  = "A1_mon";
// const char* PARAM_INPUT_6  = "A1_tue";
// const char* PARAM_INPUT_7  = "A1_wed";
// const char* PARAM_INPUT_8  = "A1_thu";
// const char* PARAM_INPUT_9  = "A1_fri";
// const char* PARAM_INPUT_10 = "A1_sat";
// const char* PARAM_INPUT_11 = "A1_sun";
// const char* PARAM_INPUT_12 = "alarm2";
// const char* PARAM_INPUT_13 = "A2_on";
// const char* PARAM_INPUT_14 = "A2_mon";
// const char* PARAM_INPUT_15 = "A2_tue";
// const char* PARAM_INPUT_16 = "A2_wed";
// const char* PARAM_INPUT_17 = "A2_thu";
// const char* PARAM_INPUT_18 = "A2_fri";
// const char* PARAM_INPUT_19 = "A2_sat";
// const char* PARAM_INPUT_20 = "A2_sun";
const char* PARAM_INPUT_30 = "setting";
const char* PARAM_INPUT_31 = "Disp1_on";
const char* PARAM_INPUT_32 = "Disp2_on";
//const char* PARAM_INPUT_33 = "Disp3_on";
const char* PARAM_INPUT_34 = "Disp1Bright";
const char* PARAM_INPUT_35 = "Disp2Bright";
//const char* PARAM_INPUT_36 = "Disp3Bright";



// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Replaces placeholder on HTML page
String processor(const String& var){
  Serial.println(var);
  if(var == "TIMEVALUE")            return clockTime.getFormattedDateTime();
  else if (var == "TIMESYNCON")     return clockTime.getSyncNTP() ? "checked" : " ";
  // else if (var == "A1TIME")         return alarm1.getFormattedTime();
  // else if (var == "A1ON")           return alarm1.getActive() ? "checked" : " ";
  // else if (var == "A1MON")          return alarm1.getMonday() ? "checked" : " ";
  // else if (var == "A1TUE")          return alarm1.getTuesday() ? "checked" : " ";
  // else if (var == "A1WED")          return alarm1.getWednesday() ? "checked" : " ";
  // else if (var == "A1THU")          return alarm1.getThursday() ? "checked" : " ";
  // else if (var == "A1FRI")          return alarm1.getFriday() ? "checked" : " ";
  // else if (var == "A1SAT")          return alarm1.getSaturday() ? "checked" : " ";
  // else if (var == "A1SUN")          return alarm1.getSunday() ? "checked" : " ";
  // else if (var == "A2TIME")         return alarm2.getFormattedTime();
  // else if (var == "A2ON")           return alarm2.getActive() ? "checked" : " ";
  // else if (var == "A2MON")          return alarm2.getMonday() ? "checked" : " ";
  // else if (var == "A2TUE")          return alarm2.getTuesday() ? "checked" : " ";
  // else if (var == "A2WED")          return alarm2.getWednesday() ? "checked" : " ";
  // else if (var == "A2THU")          return alarm2.getThursday() ? "checked" : " ";
  // else if (var == "A2FRI")          return alarm2.getFriday() ? "checked" : " ";
  // else if (var == "A2SAT")          return alarm2.getSaturday() ? "checked" : " ";
  // else if (var == "A2SUN")          return alarm2.getSunday() ? "checked" : " ";
  else if (var == "DISP1ON")        return setting.getDisp1_ON() ? "checked" : " ";
  else if (var == "DISP2ON")        return setting.getDisp2_ON() ? "checked" : " ";
  //else if (var == "DISP3ON")        return setting.getDisp3_ON() ? "checked" : " ";
  else if (var == "DISP1BRIGHT")    return String(setting.getDisp1Brightness());
  else if (var == "DISP2BRIGHT")    return String(setting.getDisp2Brightness());
  //else if (var == "DISP3BRIGHT")    return String(setting.getDisp3Brightness());
  else  return String();
}

// typedef struct MyDate {
//   byte year;
//   byte month;
//   byte day;
//   byte dow;
//   byte hour;
//   byte minute;
//   byte second;
// };

// MyDate myDate = { 0, 0, 0, 0, 0, 0, 0 };
byte oldSecond = 0;
byte oldMinute = 0;
uint storeSecond = 0;
uint tempSecond = 0;
//uint photoresistor = 0;

// Create bme Object
//Adafruit_BME280 bme; // I2C



// 7 segment code
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
              
// Variable to store Name and Password WIFI
char ssid[15];
char password[15];
  

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;


// OLED Display def
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// DS3232
uRTCLib rtc(0x68, URTCLIB_MODEL_DS3232);

/*
 * 7segment Led Control 
 pin 27 is connected to the DataIn 
 pin 25 is connected to the CLK 
 pin 26 is connected to LOAD 
 5x MAX7219
 */
 
LedControl lc=LedControl(27,25,26,2);



// ************************************************************************************
void setup() {
  // Wait after power Up
  delay (2000);
  
  // Initialize Serial Monitor 
  Serial.begin(115200);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Display init... OK!"));  
  display.display();
  // Initialize SD card
  if ( !SD.begin() ) {
    Serial.println("Card Mount Failed");
    display.println(F("Card Mount Failed"));
    display.display();
    return;
    }

  uint8_t cardType = SD.cardType();
  
  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    display.println(F("No SD card attached"));
    display.display();
    return;
  }

  Serial.print("SD Card Type: ");
  
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    
    

    // Read ssid and password from SD Card
    readFile(SD, "/wifi.txt", ssid, password);
//    Serial.println("Som von z SD");
//    Serial.println(ssid);
//    Serial.println(password);
//    display.println(ssid);
//    display.println(password);
//    display.display();
  
  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to WIFI
  Serial.print("Connecting to ");
  display.print(F("Connecting to "));
  display.println(ssid);
  
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // counter wifi status read repeat
  int i = 0;

  setting.setIsWifi(true);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    i++;
    Serial.print(".");
    display.print(F("."));
    // after 10s break
    if (i > 20) {                                          
      display.print(F("Connection failed!"));
      setting.setIsWifi(false);
      break;
    }
  }
  
  // Run this code only if wifi is connected
  if (setting.getIsWifi()) {
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  
    display.println(F(""));
    display.println(F("WiFi connected."));
    display.println(F("IP address: "));
    display.println(WiFi.localIP());
    display.display();

   
    // Initialize a NTPClient to get time
    timeClient.begin();
    // Set offset time in seconds to adjust for your timezone, for example:
    // GMT +1 = 3600
    // GMT +8 = 28800
    // GMT -1 = -3600
    // GMT 0 = 0
    // GMT +2 = 7200
    timeClient.setTimeOffset(3600);

    while(!timeClient.update()) {
    timeClient.forceUpdate();
    }
    
    //  RTCLib::set(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)
    formattedDate = timeClient.getFormattedDate();
    Serial.println(formattedDate);
    clockTime.setDateTime(formattedDate);
    Serial.println(clockTime.toString());

    byte year = (byte)(formattedDate.toInt() - 2000);
    byte month = (byte)formattedDate.substring(5).toInt();
    byte day = (byte)formattedDate.substring(8).toInt();
  
    rtc.set(timeClient.getSeconds(), timeClient.getMinutes(), timeClient.getHours(), timeClient.getDay(), day, month, year);
    rtc.refresh();
  }
    storeSecond = rtc.minute() * 60 + rtc.second();

  //we have already set the number of devices when we created the LedControl
  int devices=lc.getDeviceCount();
  for(int address=0;address<devices;address++) {
    /*The MAX72XX is in power-saving mode on startup*/
    lc.shutdown(address,true);
    
  }
  //Serial.println(devices);
  //we have to init all devices in a loop
  for(int address=0;address<devices;address++) {
    /*The MAX72XX is in power-saving mode on startup*/
    lc.shutdown(address,false);
    /* Set the brightness to a medium values */
    lc.setIntensity(address,1);
    /* and clear the display */
    lc.clearDisplay(address);
  }
    lc.setIntensity(0,15);  // Main display
    lc.setIntensity(1,10);  // Led ring
 
  //bool status;

  // Connect to BME280
  // status = bme.begin(0x76);  
  // if (!status) {
  //   Serial.println("Could not find a valid BME280 sensor, check wiring!");
  //   while (1);
  // }
  
  
  Serial.print("RTC DateTime: ");

	Serial.print(rtc.year());
	Serial.print('/');
	Serial.print(rtc.month());
	Serial.print('/');
	Serial.print(rtc.day());

	Serial.print(' ');

	Serial.print(rtc.hour());
	Serial.print(':');
	Serial.print(rtc.minute());
	Serial.print(':');
	Serial.print(rtc.second());

	Serial.print(" DOW: ");
	Serial.print(rtc.dayOfWeek());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });
  
  // Route to Sync NTP now
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
  
    while(!timeClient.update()) {
      timeClient.forceUpdate();
      }
      
      // The formattedDate comes with the following format:
      // 2018-05-28T16:00:13Z
      formattedDate = timeClient.getFormattedDate();
  
      byte year = (byte)(formattedDate.toInt() - 2000);
      byte month = (byte)formattedDate.substring(5).toInt();
      byte day = (byte)formattedDate.substring(8).toInt();
  
      rtc.set(timeClient.getSeconds(), timeClient.getMinutes(), timeClient.getHours(), timeClient.getDay(), day, month, year);
      Serial.println("Update");
    request->send(200, "text/html", "Time was synchronized with NTP server <br>" +
                                     formattedDate + "<br><a href=\"/\">Return to Home Page</a>");
    //request->redirect("/");
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    
    // GET input1 value on <ESP_IP>/get?timeSetting=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      clockTime.setDateTime(request->getParam(PARAM_INPUT_1)->value());
      
      if (request->hasParam(PARAM_INPUT_2)){
        clockTime.setSyncNTP(request->getParam(PARAM_INPUT_2)->value());
      } else {
        clockTime.setSyncNTP(false);
      }
      rtc.set(clockTime.getSecond(), clockTime.getMinute(), clockTime.getHour(), clockTime.getDow(), clockTime.getDay(), clockTime.getMonth(), clockTime.getYear() );
    // GET input1 value on <ESP_IP>/get?alarm1=<inputMessage>    
    } else if (request->hasParam(PARAM_INPUT_30)) {
        String param;
        if (request->hasParam(PARAM_INPUT_31))
          setting.setDisp1_ON(request->getParam(PARAM_INPUT_31)->value());
        else 
          setting.setDisp1_ON(false);

        if (request->hasParam(PARAM_INPUT_32))
          setting.setDisp2_ON(request->getParam(PARAM_INPUT_32)->value());
        else 
          setting.setDisp2_ON(false);
        
               
        if (request->hasParam(PARAM_INPUT_34)){
          param = request->getParam(PARAM_INPUT_34)->value();
          setting.setDisp1Brightness(param.toInt());
        }
        if (request->hasParam(PARAM_INPUT_35)){
          param = request->getParam(PARAM_INPUT_35)->value();
          setting.setDisp2Brightness(param.toInt());
        }
        
          
        
        setDispSetting();

    } else {
      Serial.println("Chyba?   ");
    }
    Serial.println(clockTime.toString());
    Serial.println(alarm1.toString());
    Serial.println(alarm2.toString());
    request->send(200, "text/html", "New settings are <br>" +
                                     clockTime.toString() + "<br>" + 
                                     alarm1.toString() + "<br>" + 
                                     alarm2.toString() + "<br>" + 
                                     setting.toString() + "<br><a href=\"/\">Return to Home Page</a>");
  });
  // Start server
  server.begin();

  // Buzzer init
  //ledcSetup(channel, freq, resolution);
  //ledcAttachPin(BUZZER_PIN, channel);

}

// ************************************************************************************
void loop() {
  rtc.refresh();
  byte actualSecond = rtc.second();
  //uint actualPhotoresistor = analogRead(SENSOR_PIN);
  
  if (tempSecond == storeSecond + 5) {
    display.clearDisplay();
    display.display();
    storeSecond = 5000;
    Serial.println("Vypinam");
    //ledcWrite(channel, 0);
    //ledcDetachPin(BUZZER_PIN);
  }
  // read actual time
  // clockTime.setDateTime(formattedDate);
  // myDate.second = rtc.second();
  // Serial.print("OldSecond ");
  // Serial.println(oldSecond);
  // Serial.print("Actual ");
  // Serial.println(clockTime.getSecond());

  // Refres Display every 1 second
  if (oldSecond != actualSecond) {
    

    tempSecond = rtc.minute() * 60 + rtc.second();
    clockTime.setYear(rtc.year());
    clockTime.setMonth(rtc.month());
    clockTime.setDay(rtc.day());
    clockTime.setDow(rtc.dayOfWeek());
    clockTime.setHour(rtc.hour());
    clockTime.setMinute(rtc.minute());
    clockTime.setSecond(rtc.second());
    
    Serial.println(clockTime.getFormattedDateTime());
    // Every day in 01:01:01 synch with NTP
    if( clockTime.getMinute() == 01 && clockTime.getHour() == 01 && clockTime.getSecond() == 01 && clockTime.getSyncNTP()) {
      while(!timeClient.update()) {
      timeClient.forceUpdate();
      }
      
      // The formattedDate comes with the following format:
      // 2018-05-28T16:00:13Z
      formattedDate = timeClient.getFormattedDate();
  
      byte year = (byte)(formattedDate.toInt() - 2000);
      byte month = (byte)formattedDate.substring(5).toInt();
      byte day = (byte)formattedDate.substring(8).toInt();
  
      rtc.set(timeClient.getSeconds(), timeClient.getMinutes(), timeClient.getHours(), timeClient.getDay(), day, month, year);
    }
    
    // Show Time to disp1
    sendToDisp1(clockTime, B00001111,B11111111);
    // Led Ring update
    sendSec(clockTime.getSecond(), clockTime.getMinute());
    
    // Show date
    //sendDate(clockTime);

    // Check photoresistor state
    // if (actualPhotoresistor > photoresistor + 100 ){
    //   display.setCursor(0,0);

    //   display.println(F("IP address: "));
    //   display.println(WiFi.localIP());
    //   display.display();
    //   storeSecond = rtc.minute() * 60 + rtc.second();
    //   Serial.print("Temp Second ");
    //   Serial.println(tempSecond);
    //   //ledcAttachPin(BUZZER_PIN, channel);
    //   //ledcWrite(channel, dutyCycle);
    // }

    // photoresistor = actualPhotoresistor;

    // if (oldMinute != clockTime.getMinute()){
    //   // Show temperature, humidity and pressure every 1 minute
    //   sendBme280();
    //   oldMinute = clockTime.getMinute();
    // }
    


    oldSecond = clockTime.getSecond();
  }
  
    // switch (button.getKey()) {
    //   case 'n':
    //     break;
    //   case 'u':
    //     break;
    //   case 'd':
    //     break;
    //   case 'l':
    //     break;
    //   case 'r':
    //     break;
    //   case 'c':
    //     break;
    //   case 'e':
    //     break;
    //   default:
    //     break;
    // }
    

  


  
  
  
  
  
  
  
  
  
  
  
  
  

  //delay(1000);
}

// ************************************************************************************
// Display to 1. led display
// DateTime, led colon, led dial
void sendToDisp1(ClockTime clockTime, byte led1, byte led2) {
  if (setting.getDisp2_ON() == false) led2 = 0;
  byte dispBuff[8] = {led1,0,0,0,0,0,0,led2};    // buffer send to 1. disp
  byte dispBuffTemp[8] = {0,0,0,0,0,0,0,0};      // temporary buff used to transpose

  
  dispBuff[1] = num[clockTime.getSecond() % 10];   // Seconds 1
  dispBuff[2] = num[clockTime.getSecond() / 10];   // Seconds 10
  dispBuff[3] = num[clockTime.getMinute() % 10];   // Minutes 1
  dispBuff[4] = num[clockTime.getMinute() / 10];   // Minutes 10
  dispBuff[5] = num[clockTime.getHour() % 10];     // Hours   1
  dispBuff[6] = num[clockTime.getHour() / 10];     // Hours   10

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
void sendSec(byte seconds, byte minute){
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
  if (seconds > 59){
    Serial.println("Chyba v sekundach!!");
    return;
  }
  
  byte row = seconds / 8;
  byte rowIndex = seconds % 8;

  if (row == 0 && rowIndex == 0 ){
    
    for(byte i = 0; i <8; i++){
      disp[i] = B00000000;
    }
    disp[7] |= B00001111;

  } else {
    for(byte i = 0; i < row; i++ ){
      disp[i] = B11111111;
    }
    disp[row] = ~(B11111111>>rowIndex);
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


// ************************************************************************************
// read from file on SD card
void readFile(fs::FS &fs, const char * path, char ssid[], char password[]){

    String LineString = "";
    
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.println("Read from file: ");
    Serial.println();
    
    while(file.available()){
//      LineString = file.readString();
//      Serial.print(LineString);
        LineString = file.readStringUntil(',');
        LineString.toCharArray(ssid, LineString.length()+1);
//        Serial.println(LineString);
//        Serial.println(sizeof(LineString));
//        Serial.println(LineString.length());
        Serial.println(ssid);
        LineString = file.readStringUntil(',');
        LineString.toCharArray(password, LineString.length()+1); 
//        Serial.println(LineString);
//        Serial.println(sizeof(LineString));
//        Serial.println(LineString.length());
        Serial.println(password); 
    }
    //Serial.println("Koniec citana");
    file.close();
    //Serial.println(ssid);
    //Serial.println(password);
}

// ************************************************************************************
void setDispSetting(){
    // Display address
    // 0 - Hours
    // 2 - LED Ring

    /*The MAX72XX is in power-saving mode on startup*/
    lc.shutdown(0,!(setting.getDisp1_ON()));
    lc.shutdown(1,!(setting.getDisp2_ON()));
    
    /* Set the brightness to a medium values */
    lc.setIntensity(0,setting.getDisp1Brightness());
    lc.setIntensity(1,setting.getDisp2Brightness());
    

}
