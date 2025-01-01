#include "Config.h"
#include <nvs_flash.h>
Config::Config()
{
    nvs_flash_init();
    EEPROM.begin(EEPROM_SIZE);
}

void Config::load()
{
    int addr = 0;
    int ssidLength = EEPROM.read(addr++);
    ssid = "";
    for (int i = 0; i < ssidLength; i++)
    {
        ssid += (char)EEPROM.read(addr++);
    }

    int passwordLength = EEPROM.read(addr++);
    password = "";
    for (int i = 0; i < passwordLength; i++)
    {
        password += (char)EEPROM.read(addr++);
    }

    for (int i = 0; i < 4; i++)
    {
        int nameLength = EEPROM.read(addr++);
        activityNames[i] = "";
        for (int j = 0; j < nameLength; j++)
        {
            activityNames[i] += (char)EEPROM.read(addr++);
        }
    }
}

void Config::save()
{
    int addr = 0;

    Serial.println("saving SSID " + ssid);
    EEPROM.write(addr++, ssid.length());
    for (char c : ssid)
    {
        EEPROM.write(addr++, c);
    }

    EEPROM.write(addr++, password.length());
    for (char c : password)
    {
        EEPROM.write(addr++, c);
    }

    for (int i = 0; i < 4; i++)
    {
        EEPROM.write(addr++, activityNames[i].length());
        for (char c : activityNames[i])
        {
            EEPROM.write(addr++, c);
        }
    }

    EEPROM.commit();
}

bool Config::hasWiFiCredentials()
{
    return ssid.length() > 0 && password.length() > 0;
}

const char *Config::getSSID()
{
    return ssid.c_str();
}

const char *Config::getPassword()
{
    return password.c_str();
}

void Config::setWiFiCredentials(const String &newSSID, const String &newPassword)
{
    ssid = newSSID;
    password = newPassword;
    save();
}

void Config::setActivityName(int index, const String &name)
{
    if (index >= 0 && index < 4)
    {
        activityNames[index] = name;
        save();
    }
}

String Config::getActivityName(int index)
{
    if (index >= 0 && index < 4)
    {
        return activityNames[index];
    }
    return "Unknown";
}