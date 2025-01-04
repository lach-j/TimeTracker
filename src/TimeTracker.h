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
    void startStopActivity(int button);
    void update();
    void display(Adafruit_SSD1306 *display);
    void setActivityName(int button, const String &name);
    TrackedActivity getActivity(int activity);
    TrackedActivity getActiveActivity();

private:
    TrackedActivity activities[4];
    int activityIndex;
    void startActivity(int index);
    void stopActivity(int index);
    String formatDuration(unsigned long duration);
};