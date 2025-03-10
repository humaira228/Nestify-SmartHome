#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread> // For sleep()
#include "EnergyMonitor.h"
#include "Device.h"
#define _CRT_SECURE_NO_WARNINGS

int main() {
    // Create devices
    Device ac("Air Conditioner", 1.5);  // 1.5 kW
    Device tv("Television", 0.2);
    Device fridge("Refrigerator", 0.4);

    EnergyMonitor monitor(0.12); // $0.12/kWh

    // Add devices to monitor
    monitor.addDevice(ac);
    monitor.addDevice(tv);
    monitor.addDevice(fridge);

    // Simulate usage
    ac.turnOn();
    tv.turnOn();
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate 2 seconds of operation

    // Turn off some devices
    tv.turnOff();

    // Display results
    std::cout << "Total Energy Used: "
        << monitor.calculateTotalEnergy() << " kWh\n";
    std::cout << "Estimated Cost: $"
        << std::fixed << std::setprecision(2)
        << monitor.calculateTotalCost() << "\n";

    // Get recommendations
    std::cout << "\nRecommendations:\n";
    for (const auto& rec : monitor.generateRecommendations()) {
        std::cout << "- " << rec << "\n";
    }

    return 0;
}