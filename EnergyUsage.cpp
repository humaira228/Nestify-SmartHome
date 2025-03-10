#include "EnergyUsage.h"

EnergyUsage::EnergyUsage() : totalUsage(0.0)
{
}

void EnergyUsage::updateUsage(const std::vector<Device*>& devices) {
    totalUsage = 0.0;
    for (const auto& device : devices) {
        totalUsage += device->getConsumption();
    }
}

double EnergyUsage::getTotalUsage() const {
    return totalUsage;
}
