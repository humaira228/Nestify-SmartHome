#include "EnergyMonitor.h"
#include <algorithm>
#include <iomanip>
#include <sstream>

EnergyMonitor::EnergyMonitor(double costPerKWh) : costPerKWh(costPerKWh) {}

void EnergyMonitor::addDevice(const Device& device) {
    devices.push_back(device);
}

double EnergyMonitor::calculateTotalEnergy() const {
    double total = 0.0;
    for (const auto& device : devices) {
        total += device.getTotalEnergy();
    }
    return total;
}

double EnergyMonitor::calculateTotalCost() const {
    return calculateTotalEnergy() * costPerKWh;
}

std::vector<std::string> EnergyMonitor::generateRecommendations() const {
    std::vector<std::string> recommendations;

    // Check for long-running devices
    for (const auto& device : devices) {
        if (device.isOn()) {
            double hours = device.getCurrentSessionDuration();
            if (hours > 2.0) {
                std::ostringstream oss;
                oss << device.getName() << " has been on for "
                    << std::fixed << std::setprecision(1) << hours
                    << " hours. Consider turning it off.";
                recommendations.push_back(oss.str());
            }

            if (device.getConsumption() > 0.5) { // High consumption threshold
                recommendations.push_back("Reduce usage of high-power device: " + device.getName());
            }
        }
    }

    // Top energy consumers
    if (!devices.empty()) {
        std::vector<Device> sorted = devices;
        std::sort(sorted.begin(), sorted.end(),
            [](const Device& a, const Device& b) {
                return a.getTotalEnergy() > b.getTotalEnergy();
            });

        recommendations.push_back("Top Energy Consumers:");
        for (size_t i = 0; i < 3 && i < sorted.size(); ++i) {
            std::ostringstream oss;
            oss << (i + 1) << ". " << sorted[i].getName() << " ("
                << std::fixed << std::setprecision(1) << sorted[i].getTotalEnergy()
                << " kWh)";
            recommendations.push_back(oss.str());
        }
    }

    return recommendations;
}