#include "Device.h"
#include <cmath>
#include <wx/datetime.h>

double Device::costPerKWh = 0.9;  // Default

Device::Device(const std::string& name, double consumption)
    : name(name), consumption(consumption), on(false),
    startTime(0), totalEnergyConsumed(0.0) {
}

bool Device::isOn() const {
    return on;
}
void Device::setOn(bool state) {
    if (state) turnOn();
    else turnOff();
}

void Device::turnOn() {
    if (!on) {
        on = true;
        startTime = std::time(nullptr);
    }
}

void Device::turnOff() {
    if (on) {
        const time_t endTime = std::time(nullptr);
        const double duration = std::difftime(endTime, startTime) / 3600.0; // Hours
        const double energyUsed = consumption * duration;

        // Add to history
        usageHistory.push_back({ startTime, endTime, energyUsed });
        totalEnergyConsumed += energyUsed;

        on = false;
    }
}

double Device::getTotalEnergy() const {
    double energy = totalEnergyConsumed;
    if (on) {
        energy += consumption * (std::difftime(std::time(nullptr), startTime) / 3600.0);
    }
    return energy;
}

double Device::getCurrentSessionEnergy() const {
    if (!on) return 0.0;
    return consumption * (std::difftime(std::time(nullptr), startTime) / 3600.0);
}

double Device::getTotalCost() const {
    return getTotalEnergy() * costPerKWh;
}

std::vector<Device::EnergyRecord> Device::getUsageHistory() const {
    return usageHistory;
}

std::string Device::getName() const { return name; }
double Device::getConsumption() const { return consumption; }

double Device::getCurrentSessionDuration() const {
    return on ? std::difftime(std::time(nullptr), startTime) / 3600.0 : 0.0;
}

void Device::resetEnergy() {
    totalEnergyConsumed = 0.0;
    usageHistory.clear();
}
wxString FormatTimeSpan(const wxTimeSpan& span) {
    return span.Format("%H hours, %M minutes, %S seconds");
}