#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "TextInput.h"
#include "Pins.h"

TextInput::TextInput(int maxLen) : maxLength(maxLen),
                                   cursorPosition(0),
                                   lastBlinkTime(0),
                                   cursorVisible(true),
                                   isHoldingConfirm(false),
                                   isHoldingArrow(-1),
                                   confirmHoldStartTime(0),
                                   arrowHoldStartTime(0),
                                   isCompleted(false),
                                   value(" ")
{
    value.reserve(maxLen);
}

const String TextInput::chars = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()~`_-=[]{}\\|;':\"<>?,./";

void TextInput::reset()
{
    value = " ";
    cursorPosition = 0;
    cursorVisible = true;
    isHoldingConfirm = false;
    isHoldingArrow = -1;
    isCompleted = false;
}

void TextInput::update()
{
    unsigned long currentTime = millis();

    unsigned long arrowHoldTime = currentTime - arrowHoldStartTime;

    if (isHoldingArrow != -1 && arrowHoldTime >= (LONG_PRESS_DURATION + SHORT_PRESS_THRESHOLD))
    {
        if (isHoldingArrow == 1)
        {
            int index = chars.indexOf(value[cursorPosition]);
            index = (index + 1) % chars.length();
            value[cursorPosition] = chars[index];
        }
        if (isHoldingArrow == 2)
        {
            int index = chars.indexOf(value[cursorPosition]);
            index = (index - 1 + chars.length()) % chars.length();
            value[cursorPosition] = chars[index];
        }
    }
    if (!isCompleted && isHoldingConfirm)
    {
        unsigned long holdTime = currentTime - confirmHoldStartTime;
        if (holdTime >= (LONG_PRESS_DURATION + SHORT_PRESS_THRESHOLD))
        {
            isCompleted = true;
        }
    }

    if (currentTime - lastBlinkTime >= BLINK_INTERVAL)
    {
        cursorVisible = !cursorVisible;
        lastBlinkTime = currentTime;
    }
}

bool TextInput::handleInput(int button, bool isDown)
{
    bool completed = false;

    unsigned long currentTime = millis();

    if (button == -1)
    {
        if (isHoldingArrow != -1 && !isDown)
        {
            isHoldingArrow = -1;
        }

        if (isHoldingConfirm && !isDown)
        {
            unsigned long holdDuration = currentTime - confirmHoldStartTime;
            isHoldingConfirm = false;

            if (holdDuration >= (LONG_PRESS_DURATION + SHORT_PRESS_THRESHOLD))
            {
                return true;
            }
            else if (holdDuration <= SHORT_PRESS_THRESHOLD)
            {
                if (cursorPosition < maxLength - 1)
                {
                    cursorPosition++;
                    if (cursorPosition == value.length())
                    {
                        value += ' ';
                    }
                }
            }
        }
        return false;
    }

    switch (button)
    {
    case UP_BUTTON:
        if (isDown && isHoldingArrow != 1)
        {
            isHoldingArrow = 1;
            arrowHoldStartTime = currentTime;
        }
        if (value.length() <= cursorPosition)
        {
            value += ' ';
        }
        else
        {
            int index = chars.indexOf(value[cursorPosition]);
            index = (index + 1) % chars.length();
            value[cursorPosition] = chars[index];
        }
        cursorVisible = true;
        lastBlinkTime = currentTime;
        break;

    case DOWN_BUTTON:
        if (isDown && isHoldingArrow != 2)
        {
            isHoldingArrow = 2;
            arrowHoldStartTime = currentTime;
        }
        if (value.length() <= cursorPosition)
        {
            value += ' ';
        }
        else
        {
            int index = chars.indexOf(value[cursorPosition]);
            index = (index - 1 + chars.length()) % chars.length();
            value[cursorPosition] = chars[index];
        }
        cursorVisible = true;
        lastBlinkTime = currentTime;
        break;

    case CONFIRM_BUTTON:
        if (isDown && !isHoldingConfirm)
        {
            isHoldingConfirm = true;
            confirmHoldStartTime = currentTime;
        }
        break;

    case BACK_BUTTON:
        if (cursorPosition > 0)
        {
            cursorPosition--;
        }
        else
        {
            completed = true;
        }
        break;
    }

    return completed;
}

void TextInput::display(Adafruit_SSD1306 *display, const char *label, int startY)
{
    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);

    display->setCursor(0, startY);
    display->println(label);

    display->setCursor(0, startY + 12);
    display->println(value);

    if (cursorVisible)
    {
        display->fillRect(cursorPosition * 6, startY + 12, 6, 8, SSD1306_INVERSE);
    }

    if (isHoldingConfirm)
    {
        unsigned long holdTime = millis() - confirmHoldStartTime;
        if (holdTime >= SHORT_PRESS_THRESHOLD && holdTime < (LONG_PRESS_DURATION + SHORT_PRESS_THRESHOLD))
        {
            int progressWidth = ((holdTime - SHORT_PRESS_THRESHOLD) * 128) / (LONG_PRESS_DURATION + SHORT_PRESS_THRESHOLD);
            display->drawRect(0, startY + 25, 128, 3, SSD1306_WHITE);
            display->fillRect(0, startY + 25, progressWidth, 3, SSD1306_WHITE);
        }
    }
}

String TextInput::getValue() const
{
    return value;
}

void TextInput::setValue(const String &newValue)
{
    value = newValue.substring(0, maxLength);
    cursorPosition = value.length();
}