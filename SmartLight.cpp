#include "SmartLight.h"
#include <iostream>

SmartLight::SmartLight(const std::string& name, double consumption, int brightness)
    : Device(name, consumption), brightness(brightness) {
}

void SmartLight::turnOn() {
    if (!on) {
        on = true;
        startTime = std::time(nullptr);
        std::cout << name << " (Smart Light) is turned ON.\n";
    }
}

void SmartLight::turnOff() {
    if (on) {
        const time_t endTime = std::time(nullptr);
        const double duration = std::difftime(endTime, startTime) / 3600.0; 
        const double energyUsed = consumption * duration;
        usageHistory.push_back({ startTime, endTime, energyUsed });
        totalEnergyConsumed += energyUsed;

        on = false;
        std::cout << name << " (Smart Light) is turned OFF.\n";
    }
}


void SmartLight::setBrightness(int level) {
    if (level < 0) level = 0;
    if (level > 100) level = 100;
    brightness = level;
    std::cout << name << " brightness set to " << brightness << "%.\n";
}

int SmartLight::getBrightness() const {
    return brightness;
}
