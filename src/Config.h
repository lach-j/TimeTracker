#pragma once
#include <Arduino.h>
#include <EEPROM.h>

class Config
{
public:
    Config();
    void load();
    void save();
    bool hasWiFiCredentials();
    const char *getSSID();
    const char *getPassword();
    void setWiFiCredentials(const String &ssid, const String &password);
    void setActivityName(int index, const String &name);
    String getActivityName(int index);

private:
    String ssid;
    String password;
    String activityNames[4];
    static const int EEPROM_SIZE = 144;
};