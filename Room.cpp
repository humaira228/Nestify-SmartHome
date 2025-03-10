#include "Room.h"

// Constructor
Room::Room(const std::string& name)
    : name(name) {
}

// Add a device to the room
void Room::addDevice(std::unique_ptr<Device> device) {
    devices.push_back(std::move(device)); // Transfer ownership
}

// Remove a device from the room by index
bool Room::removeDevice(size_t index) {
    if (index >= devices.size()) {
        return false;
    }
    devices.erase(devices.begin() + index); // unique_ptr auto-deletes the device
    return true;
}

// Get a device by index
Device* Room::getDevice(int index) {
    if (index >= 0 && index < static_cast<int>(devices.size())) {
        return devices[index].get(); // Return raw pointer from unique_ptr
    }
    return nullptr;
}

// Get all devices in the room
const std::vector<std::unique_ptr<Device>>& Room::getDevices() const {
    return devices;
}

// Calculate total energy usage for the room
double Room::calculateEnergyUsage() const {
    double total = 0.0;
    for (const auto& device : devices) {
        if (device->isOn()) {
            total += device->getConsumption();
        }
    }
    return total;
}

// Get the name of the room
std::string Room::getName() const {
    return name;
}
