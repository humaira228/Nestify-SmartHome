#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include "Device.h"

class Thermostat : public Device {
public:
    Thermostat(const std::string& name, double consumption, double temperature);

    // Override power methods
    void turnOn() override;
    void turnOff() override;

    // Temperature control
    void setTemperature(double temp);
    double getTemperature() const;

private:
    double temperature;
};

#endif // THERMOSTAT_H
