#pragma once
#include <Arduino.h>
#include <Adafruit_SSD1306.h>

struct TrackedActivity
{
    String name;
    unsigned long startTime;
    unsigned long duration;
};

class TimeTracker
{
public:
    TimeTracker();
    void handleButton(int button);
    void update();
    void display(Adafruit_SSD1306 *display);
    void setActivityName(int button, const String &name);
    String getActivityString(int activity);

private:
    TrackedActivity activities[4];
    int activityIndex;
    void startActivity(int index);
    void stopActivity(int index);
    String formatDuration(unsigned long duration);
};