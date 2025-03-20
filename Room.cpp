#include "Room.h"


Room::Room(const std::string& name)
    : name(name) {
}


void Room::addDevice(std::unique_ptr<Device> device) {
    devices.push_back(std::move(device)); // Transfer ownership
}


bool Room::removeDevice(size_t index) {
    if (index >= devices.size()) {
        return false;
    }
    devices.erase(devices.begin() + index); // uniq ptr auto del device
    return true;
}

//get device by idx
Device* Room::getDevice(int index) {
    if (index >= 0 && index < static_cast<int>(devices.size())) {
        return devices[index].get(); // return raw ptr from uniq ptr
    }
    return nullptr;
}

// Get all
const std::vector<std::unique_ptr<Device>>& Room::getDevices() const {
    return devices;
}


double Room::calculateEnergyUsage() const {
    double total = 0.0;
    for (const auto& device : devices) {
        if (device->isOn()) {
            total += device->getConsumption();
        }
    }
    return total;
}

//get room name
std::string Room::getName() const {
    return name;
}
