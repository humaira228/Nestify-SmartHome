#ifndef DEVICE_H
#define DEVICE_H

#include <string>
#include <ctime>
#include <vector>
#include <memory> // For smart pointers

class Device {
public:
    // Constructor
    Device(const std::string& name, double consumption);
    virtual ~Device() = default; // Virtual destructor for polymorphism

    // Static electricity cost
    static double costPerKWh;

    // State management
    virtual void turnOn();    // Virtual functions for polymorphism
    virtual void turnOff();
    bool isOn() const;
    void setOn(bool state);

    // Energy calculations
    virtual double getTotalEnergy() const;
    virtual double getTotalCost() const;
    double getCurrentSessionEnergy() const;

    // Struct for usage history
    struct EnergyRecord {
        std::time_t start;
        std::time_t end;
        double energy;
    };
    std::vector<EnergyRecord> getUsageHistory() const;

    // Getters
    std::string getName() const;
    double getConsumption() const;
    double getCurrentSessionDuration() const;

    // Reset energy usage
    void resetEnergy();

protected:
    std::string name;
    double consumption;
    bool on;
    time_t startTime;
    double totalEnergyConsumed;
    std::vector<EnergyRecord> usageHistory;
};

#endif // DEVICE_H
