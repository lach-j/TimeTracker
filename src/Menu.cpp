#include "Menu.h"
#include "Config.h"
#include "TimeTracker.h"
#include "Pins.h"

extern Config config;
extern TimeTracker timeTracker;

Menu::Menu(Adafruit_SSD1306 *display)
    : display(display),
      currentScreen(MenuScreen::MAIN),
      active(false),
      selectedOption(0),
      ssidInput(32),    // Max SSID length
      passwordInput(64) // Max password length
{
    for (int i = 0; i < 4; i++)
    {
        activityInputs[i] = TextInput(12);
    }
}

void Menu::enter()
{
    active = true;
    currentScreen = MenuScreen::MAIN;
    selectedOption = 0;
    ssidInput.reset();
    passwordInput.reset();
    for (int i = 0; i < 4; i++)
    {
        activityInputs[i].reset();
    }
}

void Menu::exit()
{
    active = false;
}

bool Menu::isActive()
{
    return active;
}

void Menu::handleButton(int button, bool isDown)
{
    switch (currentScreen)
    {
    case MenuScreen::MAIN:
        handleMainMenu(button);
        break;

    case MenuScreen::WIFI_SSID:
        if (ssidInput.handleInput(button, isDown))
        {
            if (button != BACK_BUTTON)
            {
                currentScreen = MenuScreen::WIFI_PASSWORD;
                passwordInput.reset();
            }
            else
            {
                currentScreen = MenuScreen::MAIN;
            }
        }
        break;

    case MenuScreen::WIFI_PASSWORD:
        if (passwordInput.handleInput(button, isDown))
        {
            if (button != BACK_BUTTON)
            {
                config.setWiFiCredentials(ssidInput.getValue(), passwordInput.getValue());
            }
            currentScreen = MenuScreen::MAIN;
        }
        break;

    case MenuScreen::ACTIVITY_NAMES:
        if (activityInputs[selectedOption].handleInput(button, isDown))
        {
            if (button != BACK_BUTTON)
            {
                selectedOption++;
                if (selectedOption >= 4)
                {
                    for (int i = 0; i < 4; i++)
                    {
                        config.setActivityName(i, activityInputs[i].getValue());
                        timeTracker.setActivityName(i, activityInputs[i].getValue());
                    }
                    currentScreen = MenuScreen::MAIN;
                    selectedOption = 0;
                }
            }
            else if (selectedOption > 0)
            {
                selectedOption--;
            }
            else
            {
                currentScreen = MenuScreen::MAIN;
            }
        }
        break;
    }
}

void Menu::handleMainMenu(int button)
{
    const int NUM_OPTIONS = 3;

    switch (button)
    {
    case DOWN_BUTTON:
        selectedOption = (selectedOption + 1) % NUM_OPTIONS;
        break;

    case UP_BUTTON:
        selectedOption = (selectedOption - 1 + NUM_OPTIONS) % NUM_OPTIONS;
        break;

    case CONFIRM_BUTTON:
        switch (selectedOption)
        {
        case 0: // WiFi
            currentScreen = MenuScreen::WIFI_SSID;
            ssidInput.reset();
            break;

        case 1: // Activity Names
            currentScreen = MenuScreen::ACTIVITY_NAMES;
            selectedOption = 0;
            for (int i = 0; i < 4; i++)
            {
                activityInputs[i].reset();
            }
            break;

        case 2: // Exit
            exit();
            break;
        }
        break;

    case BACK_BUTTON:
        exit();
        break;
    }
}

void Menu::update()
{
    switch (currentScreen)
    {
    case MenuScreen::MAIN:
        break;
    case MenuScreen::WIFI_SSID:
        ssidInput.update();
        if (ssidInput.isCompleted)
        {
            currentScreen = MenuScreen::WIFI_PASSWORD;
            passwordInput.reset();
        }
        break;

    case MenuScreen::WIFI_PASSWORD:
        passwordInput.update();
        if (passwordInput.isCompleted)
        {
            config.setWiFiCredentials(ssidInput.getValue(), passwordInput.getValue());
            currentScreen = MenuScreen::MAIN;
        }
        break;

    case MenuScreen::ACTIVITY_NAMES:
        activityInputs[selectedOption].update();
        if (activityInputs[selectedOption].isCompleted)
        {
            selectedOption++;
            if (selectedOption >= 4)
            {
                for (int i = 0; i < 4; i++)
                {
                    config.setActivityName(i, activityInputs[i].getValue());
                    timeTracker.setActivityName(i, activityInputs[i].getValue());
                }
                currentScreen = MenuScreen::MAIN;
                selectedOption = 0;
            }
        }
        break;
    }
}

void Menu::displayMenu()
{
    display->clearDisplay();

    switch (currentScreen)
    {
    case MenuScreen::MAIN:
        displayMainMenu();
        break;

    case MenuScreen::WIFI_SSID:
        ssidInput.display(display, "Enter SSID:", 0);
        display->setCursor(0, 40);
        display->println("Hold CONFIRM to save");
        break;

    case MenuScreen::WIFI_PASSWORD:
        passwordInput.display(display, "Enter Password:", 0);
        display->setCursor(0, 40);
        display->println("Hold CONFIRM to save");
        break;

    case MenuScreen::ACTIVITY_NAMES:
        display->setTextSize(1);
        display->setCursor(0, 0);
        display->println("Activity Names:");

        activityInputs[selectedOption].display(display,
                                               String("Activity " + String(selectedOption + 1) + ":").c_str(),
                                               12);

        display->setCursor(0, 40);
        display->println("Hold CONFIRM to save");
        break;
    }

    display->display();
}

void Menu::displayMainMenu()
{
    display->setTextSize(1);
    const char *menuItems[] = {"WiFi Setup", "Activity Names", "Exit"};

    for (int i = 0; i < 3; i++)
    {
        display->setCursor(0, i * 12);
        if (i == selectedOption)
        {
            display->print("> ");
        }
        else
        {
            display->print("  ");
        }
        display->println(menuItems[i]);
    }
}