#ifndef ENERGY_MONITOR_H
#define ENERGY_MONITOR_H

#include <vector>
#include <string>
#include "Device.h"

class EnergyMonitor {
public:
    EnergyMonitor(double costPerKWh);
    void addDevice(const Device& device);
    double calculateTotalEnergy() const;
    double calculateTotalCost() const;
    std::vector<std::string> generateRecommendations() const;

private:
    std::vector<Device> devices;
    double costPerKWh;
};

#endif // ENERGY_MONITOR_H