/*
*/
#ifndef Settings_h
#define Settings_h

#include "arduino.h"

class Setting
{
private:
    bool disp1_ON;
    bool disp2_ON;
    bool disp3_ON;
    byte disp1Brightness;
    byte disp2Brightness;
    byte disp3Brightness;
    bool isWifi;
    String ipAddress;
public:
    Setting(/* args */);
    ~Setting();
    bool getDisp1_ON(void);
    bool getDisp2_ON(void);
    bool getDisp3_ON(void);
    byte getDisp1Brightness(void);
    byte getDisp2Brightness(void);
    byte getDisp3Brightness(void);
    bool getIsWifi(void);
    String getIpAddress(void);

    void setDisp1_ON(bool);
    void setDisp2_ON(bool);
    void setDisp3_ON(bool);
    void setDisp1Brightness(byte);
    void setDisp2Brightness(byte);
    void setDisp3Brightness(byte);
    void setIsWifi(bool);
    void setIpAddress(String _ipAddress);
    String toString();
};

Setting::Setting(/* args */)
{
    disp1_ON = true;
    disp2_ON = true;
    disp3_ON = true;
    disp1Brightness = 6;
    disp2Brightness = 6;
    disp2Brightness = 6;
    isWifi = false;
    ipAddress = "000.000.0.000";
}

Setting::~Setting()
{
}

bool Setting::getDisp1_ON(){
    return disp1_ON;
}
bool Setting::getDisp2_ON(){
    return disp2_ON;
}
bool Setting::getDisp3_ON(){
    return disp3_ON;
}
byte Setting::getDisp1Brightness(){
    return disp1Brightness;
}
byte Setting::getDisp2Brightness(){
    return disp2Brightness;
}
byte Setting::getDisp3Brightness(){
    return disp3Brightness;
}
bool Setting::getIsWifi(){
    return isWifi;
}
String Setting::getIpAddress(){
    return ipAddress;
}

void Setting::setDisp1_ON(bool _disp1_ON){
    disp1_ON = _disp1_ON;
}
void Setting::setDisp2_ON(bool _disp2_ON){
    disp2_ON = _disp2_ON;
}
void Setting::setDisp3_ON(bool _disp3_ON){
    disp3_ON = _disp3_ON;
}
void Setting::setDisp1Brightness(byte _disp1Brightness){
    if (_disp1Brightness >= 0 && _disp1Brightness < 16)
        disp1Brightness = _disp1Brightness;
}
void Setting::setDisp2Brightness(byte _disp2Brightness){
    if (_disp2Brightness >= 0 && _disp2Brightness < 16)
        disp2Brightness = _disp2Brightness;
}
void Setting::setDisp3Brightness(byte _disp3Brightness){
    if (_disp3Brightness >= 0 && _disp3Brightness < 16)
        disp3Brightness = _disp3Brightness;
}
void Setting::setIsWifi(bool _isWifi){
    isWifi = _isWifi;
}
void Setting::setIpAddress(String _ipAddress){
    ipAddress = _ipAddress;
}

String Setting::toString(){
    return "Display 1 " + String(disp1_ON ? "YES" : "NO") + " Brightness = " + String(disp1Brightness) +
           ", Display 2 " + String(disp2_ON ? "YES" : "NO") + " Brightness = " + String(disp2Brightness) +
           ", Display 3 " + String(disp3_ON ? "YES" : "NO") + " Brightness = " + String(disp3Brightness);
}

#endif