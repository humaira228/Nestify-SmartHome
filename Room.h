#ifndef ROOM_H
#define ROOM_H

#include <string>
#include <vector>
#include <memory>
#include "Device.h"

class Room {
public:
    Room(const std::string& name);

    // Device management
    void addDevice(std::unique_ptr<Device> device);
    bool removeDevice(size_t index);
    Device* getDevice(int index);
    const std::vector<std::unique_ptr<Device>>& getDevices() const; // FIXED return type

    // Energy calculation
    double calculateEnergyUsage() const;

    // Room information
    std::string getName() const;

private:
    std::string name;
    std::vector<std::unique_ptr<Device>> devices;
};

#endif // ROOM_H
