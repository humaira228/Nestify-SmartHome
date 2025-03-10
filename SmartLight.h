#ifndef SMARTLIGHT_H
#define SMARTLIGHT_H

#include "Device.h"

class SmartLight : public Device {
public:
    SmartLight(const std::string& name, double consumption, int brightness);

    // Override power methods
    void turnOn() override;
    void turnOff() override;

    // Additional functionality
    void setBrightness(int level);
    int getBrightness() const;

private:
    int brightness;
};

#endif // SMARTLIGHT_H
