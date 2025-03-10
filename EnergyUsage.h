#ifndef ENERGYUSAGE_H
#define ENERGYUSAGE_H

#include <vector>
#include "Device.h"

class EnergyUsage {
public:
    EnergyUsage();
    void updateUsage(const std::vector<Device*>& devices);
    double getTotalUsage() const;

private:
    double totalUsage;
};

#endif // ENERGYUSAGE_H
