/*
    Alarm
*/

#ifndef Alarm_h
#define Alarm_h

#include "arduino.h"

class Alarm
{
private:
    byte year;
    byte month;
    byte day;
    byte dow;
    byte hour;
    byte minute;
    byte second;
    bool monday;
    bool tuesday;
    bool wednesday;
    bool thursday;
    bool friday;
    bool saturday;
    bool sunday;
    bool active;
public:
    Alarm(/* args */);
    ~Alarm();
    byte getYear(void);
    byte getMonth(void);
    byte getDay(void);
    byte getDow(void);
    byte getHour(void);
    byte getMinute(void);
    byte getSecond(void);
    byte getDayOn(void);
    bool getActive(void);
    String getFormattedTime(void);
    bool getMonday(void);
    bool getTuesday(void);
    bool getWednesday(void);
    bool getThursday(void);
    bool getFriday(void);
    bool getSaturday(void);
    bool getSunday(void);
    String toString();

    void setYear(byte);
    void setMonth(byte);
    void setDay(byte);
    void setDow(byte);
    void setHour(byte);
    void setMinute(byte);
    void setSecond(byte);
    void setMonday(bool);
    void setTuesday(bool);
    void setWednesday(bool);
    void setThursday(bool);
    void setFriday(bool);
    void setSaturday(bool);
    void setSunday(bool);
    void setActiv(bool);
    void setTime(String time);
};

Alarm::Alarm(/* args */)
{
    year = 0;;
    month = 0;
    day = 0;
    dow = 0;
    hour = 0;
    minute = 0;
    second = 0;
    monday = false;
    tuesday = false;
    wednesday = false;
    thursday = false;
    friday = false;
    saturday = false;
    sunday = false;
    active = false;
}

Alarm::~Alarm()
{
}

byte Alarm::getYear(){
    return year;
}
byte Alarm::getMonth(){
    return month;
}
byte Alarm::getDay(){
    return day;
}
byte Alarm::getDow(){
    return dow;
}
byte Alarm::getHour(){
    return hour;
}
byte Alarm::getMinute(){
    return minute;
}
byte Alarm::getSecond(){
    return second;
}

bool Alarm::getMonday(){
    return monday;
}
bool Alarm::getTuesday(){
    return tuesday;
}
bool Alarm::getWednesday(){
    return wednesday;
}
bool Alarm::getThursday(){
    return thursday;
}
bool Alarm::getFriday(){
    return friday;
}
bool Alarm::getSaturday(){
    return saturday;
}
bool Alarm::getSunday(){
    return sunday;
}

byte Alarm::getDayOn(){
    byte send = 0;
    if (monday)    send |= 1<<0;
    if (tuesday)   send |= 1<<1;     
    if (wednesday) send |= 1<<2;
    if (thursday)  send |= 1<<3; 
    if (friday)    send |= 1<<4; 
    if (saturday)  send |= 1<<5;
    if (sunday)    send |= 1<<6;
    return send;
}
bool Alarm::getActive(){
    return active;
}
String Alarm::getFormattedTime(){
    return (hour < 10 ? "0" + String(hour) : String(hour)) + ":" + (minute < 10 ? "0" + String(minute) : String(minute));
}


void Alarm::setYear(byte _year){
    year = _year;
}
void Alarm::setMonth(byte _month){
    month = _month;
}
void Alarm::setDay(byte _day){
    day = _day;
}
void Alarm::setDow(byte _dow){
    dow = _dow;
}
void Alarm::setHour(byte _hour){
    hour = _hour;
}
void Alarm::setMinute(byte _minute){
    minute = _minute;
}
void Alarm::setSecond(byte _second){
    second = _second;
}
void Alarm::setMonday(bool _monday){
    monday = _monday;
}
void Alarm::setTuesday(bool _tuesday){
    tuesday = _tuesday;
}
void Alarm::setWednesday(bool _wednesday){
    wednesday = _wednesday;
}
void Alarm::setThursday(bool _thursday){
    thursday = _thursday;
}
void Alarm::setFriday(bool _friday){
    friday = _friday;
}
void Alarm::setSaturday(bool _saturday){
    saturday = _saturday;
}
void Alarm::setSunday(bool _sunday){
    sunday = _sunday;
}
void Alarm::setActiv(bool _active){
    active = _active;
}
// input string HH:MM
void Alarm::setTime(String time){
    hour = time.substring(0, 2).toInt();
    minute = time.substring(3, 5).toInt();   
}

String Alarm::toString(){
    return  "20" + (year < 10 ? "0" + String(year) : String(year)) + "-" + 
            (month < 10 ? "0" + String(month) : String(month)) + "-" + 
            (day < 10 ? "0" + String(day) : String(day)) + "T" + 
            (hour < 10 ? "0" + String(hour) : String(hour)) + ":" + 
            (minute < 10 ? "0" + String(minute) : String(minute)) + ":" + 
            (second < 10 ? "0" + String(second) : String(second)) + "Z, Alarm active: " + 
            (active ? "YES" : "NO") + ", Monday: "  + 
            (monday ? "YES" : "NO") + ", Tuesday: " +
            (tuesday ? "YES" : "NO") + ", Wednesday: "  + 
            (wednesday ? "YES" : "NO") + ", Thursday: " +
            (thursday ? "YES" : "NO") + ", Friday: "  + 
            (friday ? "YES" : "NO") + ", Saturday " +
            (saturday ? "YES" : "NO") + ", Sunday: "  + 
            (sunday ? "YES" : "NO");
}


#endif