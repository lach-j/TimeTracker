
#pragma once
#include <Arduino.h>
#include <Adafruit_SSD1306.h>

class TextInput
{
private:
    String value;
    int maxLength;
    int cursorPosition;
    unsigned long lastBlinkTime;
    bool cursorVisible;
    bool isHoldingConfirm;
    int isHoldingArrow;
    unsigned long confirmHoldStartTime;
    unsigned long arrowHoldStartTime;
    static const unsigned long BLINK_INTERVAL = 500;        // Cursor blink interval in ms
    static const unsigned long LONG_PRESS_DURATION = 1000;  // Total duration for long press to register
    static const unsigned long SHORT_PRESS_THRESHOLD = 200; // Minimum duration required to be considered a long press
    static const String chars;

public:
    TextInput(int maxLen = 16);
    void reset();
    bool handleInput(int button, bool isDown);
    void display(Adafruit_SSD1306 *display, const char *label, int startY);
    String getValue() const;
    void setValue(const String &newValue);
    void update();
    bool isCompleted;
};