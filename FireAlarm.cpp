#include "FireAlarm.h"

FireAlarm::FireAlarm(const std::string& alarmName)
    : Device(alarmName, 0.0)  // Pass a default consumption of 0.0
{
}

void FireAlarm::triggerAlarm() {
    std::cout << "FIRE ALARM ACTIVATED!" << std::endl;
    std::cout << "All devices will be turned off for safety.\n";
}
