#include "TimeTracker.h"

TimeTracker::TimeTracker()
{
    for (int i = 0; i < 4; i++)
    {
        activityIndex = -1;
        activities[i] = {"Activity " + String(i + 1), 0, 0};
    }
}

void TimeTracker::startStopActivity(int button)
{
    if (button >= 0 && button < 4)
    {
        if (button == activityIndex)
        {
            stopActivity(button);
        }
        else
        {
            startActivity(button);
        }
    }
}

void TimeTracker::startActivity(int index)
{
    if (activityIndex != -1)
    {
        stopActivity(activityIndex);
    }
    activities[index].startTime = millis();
    activityIndex = index;
}

void TimeTracker::stopActivity(int index)
{
    activities[index].duration += millis() - activities[index].startTime;
    activityIndex = -1;
}

void TimeTracker::update()
{
    if (activityIndex != -1)
    {
        unsigned long currentDuration = millis() - activities[activityIndex].startTime;
        activities[activityIndex].duration = currentDuration;
    }
}

String TimeTracker::formatDuration(unsigned long duration)
{
    unsigned long seconds = duration / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;

    char buffer[9];
    sprintf(buffer, "%02lu:%02lu:%02lu", hours, minutes % 60, seconds % 60);
    return String(buffer);
}

void TimeTracker::display(Adafruit_SSD1306 *display)
{
    display->clearDisplay();
    display->setTextSize(2);
    display->setTextColor(SSD1306_WHITE);
    display->cp437(true);

    display->setCursor(0, 0);

    if (activityIndex == -1)
    {
        display->println("v1.0.0.1");
    }
    else
    {
        display->println(activities[activityIndex].name);

        display->println(formatDuration(activities[activityIndex].duration));
    }

    display->display();
}

void TimeTracker::setActivityName(int button, const String &name)
{
    if (button >= 0 && button < 4)
    {
        activities[button].name = name;
    }
}

TrackedActivity TimeTracker::getActivity(int activity)
{
    return activities[activity];
}

TrackedActivity TimeTracker::getActiveActivity()
{
    if (activityIndex == -1)
        return {"", 0, 0};
    return activities[activityIndex];
}