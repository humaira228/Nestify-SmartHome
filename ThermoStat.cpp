#include "Thermostat.h"
#include <iostream>

Thermostat::Thermostat(const std::string& name, double consumption, double temperature)
    : Device(name, consumption), temperature(temperature) {
}

void Thermostat::turnOn() {
    if (!on) {
        on = true;
        startTime = std::time(nullptr);
        std::cout << name << " (Thermostat) is turned ON at " << temperature << "°C.\n";
    }
}

void Thermostat::turnOff() {
    if (on) {
        const time_t endTime = std::time(nullptr);
        const double duration = std::difftime(endTime, startTime) / 3600.0;
        const double energyUsed = consumption * duration;

        // Record usage history
        usageHistory.push_back({ startTime, endTime, energyUsed });
        totalEnergyConsumed += energyUsed;

        on = false;
        std::cout << name << " (Thermostat) is turned OFF.\n";
    }
}

void Thermostat::setTemperature(double temp) {
    temperature = temp;
    std::cout << name << " temperature set to " << temperature << "°C.\n";
}

double Thermostat::getTemperature() const {
    return temperature;
}
