#pragma once
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "TextInput.h"

enum class MenuScreen
{
    MAIN,
    WIFI_SSID,
    WIFI_PASSWORD,
    ACTIVITY_NAMES,
    CONFIRM
};

class Menu
{
public:
    Menu(Adafruit_SSD1306 *display);
    void enter();
    void exit();
    bool isActive();
    void handleButton(int button, bool isDown);
    void update();
    void displayMenu();

private:
    Adafruit_SSD1306 *display;
    MenuScreen currentScreen;
    bool active;
    int selectedOption;
    TextInput ssidInput;
    TextInput passwordInput;
    TextInput activityInputs[4];

    void handleMainMenu(int button);
    void displayMainMenu();
};