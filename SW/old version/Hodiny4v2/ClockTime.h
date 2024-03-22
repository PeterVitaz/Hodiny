/*
    Time keeping
*/

#ifndef ClockTime_h
#define ClockTime_h

#include "arduino.h"

class ClockTime
{
private:
    byte year;
    byte month;
    byte day;
    byte dow;
    byte hour;
    byte minute;
    byte second;
    bool syncNTP;
public:
    ClockTime(/* args */);
    ~ClockTime();
    byte getYear(void);
    byte getMonth(void);
    byte getDay(void);
    byte getDow(void);
    byte getHour(void);
    byte getMinute(void);
    byte getSecond(void);
    byte getDayOn(void);
    bool getSyncNTP(void);
    String getFormattedTime(void);
    String getFormattedDateTime(void);
    String toString();

    void setYear(byte);
    void setMonth(byte);
    void setDay(byte);
    void setDow(byte);
    void setHour(byte);
    void setMinute(byte);
    void setSecond(byte);
    void setSyncNTP(bool);
    void setDateTime(String);
};

ClockTime::ClockTime(/* args */)
{
    year = 0;;
    month = 1;
    day = 1;
    dow = 0;
    hour = 0;
    minute = 0;
    second = 0;
    syncNTP = true;
}

ClockTime::~ClockTime()
{
}

byte ClockTime::getYear(){
    return year;
}
byte ClockTime::getMonth(){
    return month;
}
byte ClockTime::getDay(){
    return day;
}
byte ClockTime::getDow(){
    return dow;
}
byte ClockTime::getHour(){
    return hour;
}
byte ClockTime::getMinute(){
    return minute;
}
byte ClockTime::getSecond(){
    return second;
}
bool ClockTime::getSyncNTP(){
    return syncNTP;
}
String ClockTime::getFormattedTime(){
    return (hour < 10 ? "0" + String(hour) : String(hour)) + ":" + (minute < 10 ? "0" + String(minute) : String(minute));
}
String ClockTime::getFormattedDateTime(){
    return "20" + (year < 10 ? "0" + String(year) : String(year)) + "-" + 
            (month < 10 ? "0" + String(month) : String(month)) + "-" + 
            (day < 10 ? "0" + String(day) : String(day)) + "T" + 
            (hour < 10 ? "0" + String(hour) : String(hour)) + ":" + 
            (minute < 10 ? "0" + String(minute) : String(minute));
}

// ***************************************************************************
// "set" methods with input control

// store last two value of Year between 00 - 99
void ClockTime::setYear(byte _year){
    if (_year >= 0 && _year < 100){
        year = _year;
    } else {
        // error
        year = 0xEE;
    }
}

void ClockTime::setMonth(byte _month){
    if (_month > 0 && _month < 13 ){
        month = _month;
    } else {
        // error
        month = 0xEE;
    }
}

void ClockTime::setDay(byte _day){
    if (_day > 0 && _day < 32) {
        day = _day;
    } else {
        // error
        day = 0xEE;
    }
}

void ClockTime::setDow(byte _dow){
    if (_dow >= 0 && _dow < 7) {
        dow = _dow;
    } else {
        // error
        dow = 0xEE;
    }
}

void ClockTime::setHour(byte _hour){
    if (_hour >= 0 && _hour < 24) {
        hour = _hour;
    } else {
        // error
        hour = 0xEE;
    }
}

void ClockTime::setMinute(byte _minute){
    if (_minute >= 0 && _minute < 60) {
        minute = _minute;
    } else {
        // error
        minute = 0xEE;
    }
}

void ClockTime::setSecond(byte _second){
    if (_second >= 0 && _second < 60) {
        second = _second;
    } else {
        // error
        second = 0xEE;
    }
}

void ClockTime::setSyncNTP(bool _syncNTP){
    syncNTP = _syncNTP;
}

void ClockTime::setDateTime(String dateTime){
    //2019-01-01T10:00
    year = dateTime.substring(2, 4).toInt();
    month = dateTime.substring(5, 7).toInt();
    day = dateTime.substring(8, 10).toInt();
    hour = dateTime.substring(11, 13).toInt();
    minute = dateTime.substring(14, 16).toInt();
    second = 0;
        
    int adjustment, mm, yy;
 
	adjustment = (14 - month) / 12;
	mm = month + 12 * adjustment - 2;
	yy = year - adjustment;
	dow = (day + (13 * mm - 1) / 5 + yy + yy / 4 - yy / 100 + yy / 400) % 7;
    
}

String ClockTime::toString(){
    return  this->getFormattedDateTime() + "Z, SyncNTP active: " + (syncNTP ? "YES" : "NO");
}
#endif