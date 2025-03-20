#ifndef DEVICE_H
#define DEVICE_H

#include <string>
#include <ctime>
#include <vector>
#include <memory>



class Device {
public:
  
    Device(const std::string& name, double consumption);
    virtual ~Device() = default; 

    static double costPerKWh;


    virtual void turnOn();    
    virtual void turnOff();
    bool isOn() const;
    void setOn(bool state);


    virtual double getTotalEnergy() const;
    virtual double getTotalCost() const;
    double getCurrentSessionEnergy() const;

    // usage hist
    struct EnergyRecord {
        std::time_t start;
        std::time_t end;
        double energy;
    };
    std::vector<EnergyRecord> getUsageHistory() const;

   
    std::string getName() const;
    double getConsumption() const;
    double getCurrentSessionDuration() const;

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
