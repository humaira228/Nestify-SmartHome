
#include "Designer.h"
#include <iomanip>
#include <sstream>

const std::unordered_map<std::string, double> DEVICE_POWER = {
    {"Light", 0.1},
    {"Smart Light", 0.15},  // Added
    {"Thermostat", 0.5},
    {"Fan", 0.3},
    {"AC", 1.5},
    {"Heater", 2.0}
};

Designer::Designer(wxWindow* parent) {
    if (!parent) {
        wxLogError("Error: Parent window is null!");
        return;
    }

    panel = new wxPanel(parent, wxID_ANY);
    if (!panel) {
        wxLogError("Failed to create wxPanel!");
        return;
    }

    panel->SetBackgroundColour(wxColour(33, 33, 33));
    setupLayout();
}

Designer::~Designer() {
    if (panel) panel->Destroy();
}

wxPanel* Designer::getPanel() const { return panel; }
wxListBox* Designer::getRoomList() const { return roomList; }
wxListBox* Designer::getDeviceList() const { return deviceList; }
wxCheckBox* Designer::getDeviceStatusCheckbox() const { return deviceStatusCheckbox; }
wxChoice* Designer::getRoomChoice() const { return roomChoice; }
wxChoice* Designer::getDeviceChoice() const { return deviceChoice; }
wxButton* Designer::getAddRoomButton() const { return addRoomButton; }
wxButton* Designer::getAddDeviceButton() const { return addDeviceButton; }
wxButton* Designer::getRemoveDeviceButton() const { return removeDeviceButton; }
wxButton* Designer::getShowEnergyButton() const { return showEnergyButton; }
wxButton* Designer::getFireAlarmButton() const { return fireAlarmButton; }
wxStaticText* Designer::getEnergyUsageLabel() const { return energyUsageLabel; }


// Device constructor
Designer::Device::Device(const std::string& n, double p)
    : name(n), power_kW(p), is_on(false), last_status_change(time(nullptr)) {
}

void Designer::addDevice() {
    wxString room_name = roomChoice->GetStringSelection();
    wxString device_name = deviceChoice->GetStringSelection();
    bool status = deviceStatusCheckbox->GetValue();

    Device new_dev(device_name.ToStdString(), DEVICE_POWER.at(device_name.ToStdString()));
    new_dev.is_on = status;
    new_dev.last_status_change = time(nullptr);

    rooms[room_name.ToStdString()].push_back(new_dev);
    deviceList->AppendString(wxString::Format("%s - %s [%s]",
        room_name, device_name, status ? "ON" : "OFF"));

    updateEnergyUsage();
}

void Designer::updateEnergyUsage() {
    double total_energy = 0.0;
    time_t now = time(nullptr);

    for (auto& room_pair : rooms) {
        auto& devices = room_pair.second;
        for (auto& dev : devices) {
            if (dev.is_on) {
                double hours = difftime(now, dev.last_status_change) / 3600.0;
                total_energy += dev.power_kW * hours;
            }
        }
    }

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2)
        << "Energy Usage: " << total_energy << " kWh\n"
        << "Estimated Cost: $" << (total_energy * electricity_rate) << "\n"
        << generateRecommendations();

    energyUsageLabel->SetLabel(ss.str());
}

std::string Designer::generateRecommendations() {
    std::vector<std::string> recommendations;
    int high_power_count = 0;

    for (const auto& room_pair : rooms) {
        const auto& devices = room_pair.second;
        for (const auto& dev : devices) {
            if (dev.power_kW > 1.0 && dev.is_on) {
                if (++high_power_count > 2) {
                    recommendations.emplace_back("⚠️ Multiple high-power devices running!");
                }
            }

            if (dev.name == "Heater" && dev.is_on) {
                time_t duration = time(nullptr) - dev.last_status_change;
                if (duration > 3600) {
                    recommendations.emplace_back("♨️ Heater running for over 1 hour");
                }
            }
        }
    }

    if (recommendations.empty()) return "Recommendations: All systems optimal ✅";

    std::string result = "Recommendations:\n";
    for (const auto& msg : recommendations) result += "• " + msg + "\n";
    return result;
}

void Designer::setupLayout() {

    // Font settings
    wxFont font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    wxFont boldFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

    // Vertical box sizer for layout
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    // Electricity Rate Input
    wxBoxSizer* rateSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* rateLabel = new wxStaticText(panel, wxID_ANY, "Electricity Rate ($/kWh):");
    rateLabel->SetFont(boldFont);
    rateLabel->SetForegroundColour(wxColour(255, 255, 255));

    wxTextCtrl* rateInput = new wxTextCtrl(panel, wxID_ANY, "0.15");
    rateInput->SetFont(font);
    rateInput->SetBackgroundColour(wxColour(50, 50, 50));
    rateInput->SetForegroundColour(wxColour(255, 255, 255));

    rateInput->Bind(wxEVT_TEXT, [this](wxCommandEvent& event) {
        event.GetString().ToDouble(&electricity_rate);
        });

    rateSizer->Add(rateLabel, 0, wxRIGHT, 5);
    rateSizer->Add(rateInput, 0, wxEXPAND);
    vbox->Add(rateSizer, 0, wxEXPAND | wxALL, 10);

    // Room Input with Dropdown
    wxBoxSizer* roomSizer = new wxBoxSizer(wxHORIZONTAL);
    roomChoice = new wxChoice(panel, wxID_ANY);
    roomChoice->Append("Living Room");
    roomChoice->Append("Bedroom");
    roomChoice->Append("Kitchen");
    roomChoice->Append("Bathroom");
    roomChoice->SetSelection(0); // Default selection
    roomChoice->SetFont(font);
    roomChoice->SetBackgroundColour(wxColour(50, 50, 50));
    roomChoice->SetForegroundColour(wxColour(255, 255, 255));

    addRoomButton = new wxButton(panel, wxID_ANY, "Add Room");
    addRoomButton->SetBackgroundColour(wxColour(0, 204, 204));  // More vibrant color
    addRoomButton->SetFont(boldFont);

    wxStaticText* roomLabel = new wxStaticText(panel, wxID_ANY, "Room:");
    roomLabel->SetForegroundColour(wxColour(255, 255, 255));
    roomLabel->SetFont(boldFont);

    roomSizer->Add(roomLabel, 0, wxRIGHT, 5);
    roomSizer->Add(roomChoice, 1, wxEXPAND | wxRIGHT, 5);
    roomSizer->Add(addRoomButton, 0);
    vbox->Add(roomSizer, 0, wxEXPAND | wxALL, 10);

    // Device Input with Dropdown
    wxBoxSizer* deviceSizer = new wxBoxSizer(wxHORIZONTAL);
    deviceChoice = new wxChoice(panel, wxID_ANY);
    deviceChoice->Append("Light");
    deviceChoice->Append("Smart Light");
    deviceChoice->Append("Thermostat");
    deviceChoice->Append("Fan");
    deviceChoice->Append("AC");
    deviceChoice->Append("Heater");
    deviceChoice->SetSelection(0);
    deviceChoice->SetFont(font);
    deviceChoice->SetBackgroundColour(wxColour(50, 50, 50));
    deviceChoice->SetForegroundColour(wxColour(255, 255, 255));

    addDeviceButton = new wxButton(panel, wxID_ANY, "Add Device");
    addDeviceButton->SetBackgroundColour(wxColour(255, 20, 147));  // Sleek button color
    addDeviceButton->SetFont(boldFont);

    deviceStatusCheckbox = new wxCheckBox(panel, wxID_ANY, "On/Off");
    deviceStatusCheckbox->SetFont(font);
    deviceStatusCheckbox->SetBackgroundColour(wxColour(50, 50, 50));
    deviceStatusCheckbox->SetForegroundColour(wxColour(255, 255, 255));

    wxStaticText* deviceLabel = new wxStaticText(panel, wxID_ANY, "Device:");
    deviceLabel->SetForegroundColour(wxColour(255, 255, 255));
    deviceLabel->SetFont(boldFont);

    deviceSizer->Add(deviceLabel, 0, wxRIGHT, 5);
    deviceSizer->Add(deviceChoice, 1, wxEXPAND | wxRIGHT, 5);
    deviceSizer->Add(addDeviceButton, 0);
    deviceSizer->Add(deviceStatusCheckbox, 0, wxLEFT, 10);
    vbox->Add(deviceSizer, 0, wxEXPAND | wxALL, 10);

    // Room & Device List
    wxBoxSizer* listSizer = new wxBoxSizer(wxHORIZONTAL);
    roomList = new wxListBox(panel, wxID_ANY);
    roomList->SetBackgroundColour(wxColour(50, 50, 50));
    roomList->SetForegroundColour(wxColour(255, 255, 0));
    roomList->SetFont(font);

    deviceList = new wxListBox(panel, wxID_ANY);
    deviceList->SetBackgroundColour(wxColour(50, 50, 50));
    deviceList->SetForegroundColour(wxColour(255, 255, 0));
    deviceList->SetFont(font);

    listSizer->Add(roomList, 1, wxEXPAND | wxRIGHT, 10);
    listSizer->Add(deviceList, 1, wxEXPAND);
    vbox->Add(listSizer, 1, wxEXPAND | wxALL, 10);

    // Energy Usage Section with Dynamic Text
    wxBoxSizer* energySizer = new wxBoxSizer(wxHORIZONTAL);
    energyUsageLabel = new wxStaticText(panel, wxID_ANY, "Energy Usage: 0.00 kWh");
    energyUsageLabel->SetForegroundColour(wxColour(0, 255, 255));
    energyUsageLabel->SetFont(boldFont);

    showEnergyButton = new wxButton(panel, wxID_ANY, "Show Usage");
    showEnergyButton->SetBackgroundColour(wxColour(0, 255, 255));
    showEnergyButton->SetFont(boldFont);

    showEnergyButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) {
        updateEnergyUsage();
        });

    energySizer->Add(energyUsageLabel, 1, wxEXPAND | wxRIGHT, 10);
    energySizer->Add(showEnergyButton, 0);
    vbox->Add(energySizer, 0, wxEXPAND | wxALL, 10);

    // Action Buttons with Better Styling
    wxBoxSizer* actionSizer = new wxBoxSizer(wxHORIZONTAL);
    removeDeviceButton = new wxButton(panel, wxID_ANY, "Remove Device");
    removeDeviceButton->SetBackgroundColour(wxColour(255, 69, 0));
    removeDeviceButton->SetFont(boldFont);

    deviceSettingsButton = new wxButton(panel, wxID_ANY, "Settings");
    deviceSettingsButton->SetBackgroundColour(wxColour(255, 215, 0));
    deviceSettingsButton->SetFont(boldFont);

    fireAlarmButton = new wxButton(panel, wxID_ANY, "Trigger Fire Alarm");
    fireAlarmButton->SetBackgroundColour(wxColour(255, 0, 0));
    fireAlarmButton->SetFont(boldFont);

    actionSizer->Add(removeDeviceButton, 1, wxEXPAND | wxRIGHT, 10);
    actionSizer->Add(deviceSettingsButton, 1, wxEXPAND | wxRIGHT, 10);
    actionSizer->Add(fireAlarmButton, 1, wxEXPAND);
    vbox->Add(actionSizer, 0, wxEXPAND | wxALL, 10);

    panel->SetSizer(vbox);
}
