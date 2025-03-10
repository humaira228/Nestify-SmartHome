#ifndef DESIGNER_H
#define DESIGNER_H

#include <wx/wx.h>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <string>

class Designer {
public:
    Designer(wxWindow* parent);
    ~Designer();

    wxPanel* getPanel() const;

    // Getters for UI components
    wxListBox* getRoomList() const;
    wxListBox* getDeviceList() const;
    wxCheckBox* getDeviceStatusCheckbox() const;
    wxChoice* getRoomChoice() const;
    wxChoice* getDeviceChoice() const;
    wxButton* getAddRoomButton() const;
    wxButton* getAddDeviceButton() const;
    wxButton* getRemoveDeviceButton() const;
    wxButton* getShowEnergyButton() const;
    wxButton* getFireAlarmButton() const;
    wxStaticText* getEnergyUsageLabel() const;
    wxButton* deviceSettingsButton;
    wxButton* getDeviceSettingsButton() const { return deviceSettingsButton; }



private:
    struct Device {
        std::string name;
        double power_kW;
        bool is_on;
        time_t last_status_change;
        Device(const std::string& n, double p);
    };

    void setupLayout();
    void addDevice();
    void removeSelectedDevice();
    void updateEnergyUsage();
    std::string generateRecommendations();

    // UI Components
    wxPanel* panel;
    wxChoice* roomChoice;
    
    wxListBox* roomList;
    wxListBox* deviceList;
    wxCheckBox* deviceStatusCheckbox;
    wxStaticText* energyUsageLabel;
    wxButton* addDeviceButton = nullptr;
    wxButton* addRoomButton = nullptr;
    wxChoice* deviceChoice = nullptr;
    wxButton* removeDeviceButton;
    wxButton* showEnergyButton;
    wxButton* fireAlarmButton;
    // Add to class declaration
    

    // Data structures
    std::unordered_map<std::string, std::vector<Device>> rooms;
    double electricity_rate = 0.15;
};

#endif // DESIGNER_H
