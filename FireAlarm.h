#pragma once
#include "Device.h"
#include <string>
#include <iostream>

class FireAlarm : public Device {
public:
    // The fire alarm has no energy consumption so we pass 0.0.
    FireAlarm(const std::string& alarmName);

    // Triggers the alarm.
    void triggerAlarm();
};
