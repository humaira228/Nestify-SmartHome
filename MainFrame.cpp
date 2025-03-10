#include "MainFrame.h"
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <fstream>
#include <sstream>
#include <wx/datetime.h>
#include <wx/numdlg.h>  // Add this for number entry dialog

MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)),
    designer(nullptr), fireAlarmTimer(nullptr), selectedRoomIndex(wxNOT_FOUND),
    onHour(18), offHour(6) {

    // Initialize designer panel
    designer = new Designer(this);
    if (!designer || !designer->getPanel()) {
        wxMessageBox("Designer panel initialization failed!", "Error", wxOK | wxICON_ERROR);
        return;
    }


    // Initialize device power map
    devicePowerMap = {
        {"Light", 0.1},
        {"Smart Light", 0.15},
        {"Thermostat", 0.5},
        {"AC", 1.5},
        {"Fan", 0.3},
        {"Heater", 2.0}
    };

    // Initialize fire alarm timer
    fireAlarmTimer = new wxTimer(this);
    Bind(wxEVT_TIMER, &MainFrame::OnFireAlarmTimeout, this, fireAlarmTimer->GetId());

    // Time selection controls
    wxArrayString hours;
    for (int i = 1; i <= 23; i++) {
        hours.Add(wxString::Format("%02d:00", i));
    }

    // Initialize time controls
    onTimeChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, hours);
    offTimeChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, hours);
    applyTimeButton = new wxButton(this, wxID_ANY, "Apply Time Settings");
    onTimeChoice->SetSelection(17);  // 18:00
    offTimeChoice->SetSelection(5);   // 06:00

    // Electricity rate controls
    rateInput = new wxTextCtrl(this, wxID_ANY, "0.90");
    wxButton* applyRateBtn = new wxButton(this, wxID_ANY, "Apply Rate");

    // Main layout setup
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Time controls layout
    wxBoxSizer* timeSizer = new wxBoxSizer(wxHORIZONTAL);
    timeSizer->Add(new wxStaticText(this, wxID_ANY, "Lights ON:"), 0, wxALL, 5);
    timeSizer->Add(onTimeChoice, 0, wxALL, 5);
    timeSizer->Add(new wxStaticText(this, wxID_ANY, "Lights OFF:"), 0, wxALL, 5);
    timeSizer->Add(offTimeChoice, 0, wxALL, 5);
    timeSizer->Add(applyTimeButton, 0, wxALL, 5);

    // Rate controls layout
    wxBoxSizer* rateSizer = new wxBoxSizer(wxHORIZONTAL);
    rateSizer->Add(new wxStaticText(this, wxID_ANY, "Electricity Rate ($/kWh):"), 0, wxALL, 5);
    rateSizer->Add(rateInput, 0, wxALL, 5);
    rateSizer->Add(applyRateBtn, 0, wxALL, 5);

    mainSizer->Add(rateSizer, 0, wxALIGN_CENTER | wxALL, 10);
    mainSizer->Add(timeSizer, 0, wxALIGN_CENTER | wxALL, 10);

    // Login panel setup (simplified)
    loginPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(400, 250));
    loginPanel->SetBackgroundColour(wxColour(160, 220, 255));
    loginPanel->SetWindowStyle(wxBORDER_SIMPLE);

    wxBoxSizer* loginSizer = new wxBoxSizer(wxVERTICAL);

    // Header
    wxStaticText* header = new wxStaticText(loginPanel, wxID_ANY, "Smart Home Login");
    header->SetFont(wxFont(18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    header->SetForegroundColour(wxColour(50, 50, 50));
    loginSizer->Add(header, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP | wxBOTTOM, 15);

    // Username input
    usernameInput = new wxTextCtrl(loginPanel, wxID_ANY, "", wxDefaultPosition, wxSize(250, 32), wxBORDER_SIMPLE);
    usernameInput->SetHint("Enter your username");
    usernameInput->SetBackgroundColour(wxColour(248, 248, 248));
    loginSizer->Add(usernameInput, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

    // Password input
    passwordInput = new wxTextCtrl(loginPanel, wxID_ANY, "", wxDefaultPosition, wxSize(250, 32), wxTE_PASSWORD | wxBORDER_SIMPLE);
    passwordInput->SetHint("Enter your password");
    passwordInput->SetBackgroundColour(wxColour(248, 248, 248));
    loginSizer->Add(passwordInput, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);


    // Buttons
    loginButton = new wxButton(loginPanel, wxID_ANY, "SIGN IN", wxDefaultPosition, wxSize(200, 40));
    loginButton->SetBackgroundColour(wxColour(0, 140, 186));
    loginButton->SetForegroundColour(wxColour(255, 255, 255));
    loginButton->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

    registerButton = new wxButton(loginPanel, wxID_ANY, "CREATE ACCOUNT", wxDefaultPosition, wxSize(200, 40));
    registerButton->SetBackgroundColour(wxColour(229, 229, 229));
    registerButton->SetForegroundColour(wxColour(50, 50, 50));
    registerButton->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    wxBoxSizer* buttonSizer = new wxBoxSizer(wxVERTICAL);
    buttonSizer->Add(loginButton, 0, wxEXPAND | wxBOTTOM, 8);
    buttonSizer->Add(registerButton, 0, wxEXPAND | wxTOP, 8);
    loginSizer->Add(buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 15);

    loginPanel->SetSizer(loginSizer);

    // Add login panel to main sizer
    mainSizer->Add(loginPanel, 1, wxALIGN_CENTER | wxALL, 20);



    // Add designer panel (hidden initially)
    if (designer && designer->getPanel()) {
        mainSizer->Add(designer->getPanel(), 1, wxEXPAND | wxALL, 20);
        designer->getPanel()->Hide();
    }

    this->SetSizer(mainSizer);

    // Event bindings
    applyTimeButton->Bind(wxEVT_BUTTON, &MainFrame::OnApplyTime, this);
    applyRateBtn->Bind(wxEVT_BUTTON, &MainFrame::OnApplyRate, this);
    loginButton->Bind(wxEVT_BUTTON, &MainFrame::OnLogin, this);
    registerButton->Bind(wxEVT_BUTTON, &MainFrame::OnRegister, this);

    // Initialize timers
    timeSwitchTimer = new wxTimer(this);
    Bind(wxEVT_TIMER, &MainFrame::OnTimeSwitch, this, timeSwitchTimer->GetId());
    timeSwitchTimer->Start(5000);
    // Add after other button bindings
    if (designer && designer->getDeviceSettingsButton()) {
        designer->getDeviceSettingsButton()->Bind(
            wxEVT_BUTTON,
            &MainFrame::OnDeviceSettings,
            this
        );
    }
    // Bind room selection
    if (designer && designer->getRoomList()) {
        designer->getRoomList()->Bind(wxEVT_LISTBOX, &MainFrame::OnRoomSelected, this);
    }
}



MainFrame::~MainFrame() {
    delete designer;
    delete fireAlarmTimer;
    delete timeSwitchTimer;
}
// Rest of the implementation remains the same as in your original code
// (OnLogin, OnRegister, ShowHomeScreen, AuthenticateUser, etc.)
// Rest of the implementation remains the same as in your original code
// (OnLogin, OnRegister, ShowHomeScreen, AuthenticateUser, etc.)

// Login Handling
void MainFrame::OnLogin(wxCommandEvent& event) {
    wxString username = usernameInput->GetValue();
    wxString password = passwordInput->GetValue();

    if (username.IsEmpty() || password.IsEmpty()) {
        wxMessageBox("Username and password cannot be empty!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    if (AuthenticateUser(username.ToStdString(), password.ToStdString())) {
        wxMessageBox("Login successful!", "Welcome", wxOK | wxICON_INFORMATION);
        ShowHomeScreen();
    }
    else {
        wxMessageBox("Invalid credentials!", "Error", wxOK | wxICON_ERROR);
    }
}

// Registration Handling
void MainFrame::OnRegister(wxCommandEvent& event) {
    wxString username = usernameInput->GetValue();
    wxString password = passwordInput->GetValue();

    if (username.IsEmpty() || password.IsEmpty()) {
        wxMessageBox("Username and password cannot be empty!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    if (RegisterUser(username.ToStdString(), password.ToStdString())) {
        wxMessageBox("User registered successfully!", "Success", wxOK | wxICON_INFORMATION);
    }
    else {
        wxMessageBox("Username already exists!", "Error", wxOK | wxICON_ERROR);
    }
}


// Authenticate User
bool MainFrame::AuthenticateUser(const std::string& username, const std::string& password) {
    std::ifstream file("users.txt");
    std::string line, storedUser, storedPass;

    while (getline(file, line)) {
        std::istringstream ss(line);
        ss >> storedUser >> storedPass;
        if (storedUser == username && storedPass == password) {
            return true;
        }
    }
    return false;
}

// Register User
bool MainFrame::RegisterUser(const std::string& username, const std::string& password) {
    std::ifstream file("users.txt");
    std::string line, storedUser, storedPass;

    while (getline(file, line)) {
        std::istringstream ss(line);
        ss >> storedUser >> storedPass;
        if (storedUser == username) {
            return false;
        }
    }

    std::ofstream outFile("users.txt", std::ios::app);
    outFile << username << " " << password << "\n";
    return true;
}

// Event: Add Room
void MainFrame::OnAddRoom(wxCommandEvent& event) {
    if (!designer || !designer->getRoomChoice()) return;

    wxString roomName = designer->getRoomChoice()->GetStringSelection();
    if (roomName.IsEmpty()) {
        wxMessageBox("Select a room!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    // Save current selection
    int prevSelection = designer->getRoomList()->GetSelection();

    // Block events to prevent automatic selection changes
    wxEventBlocker blocker(designer->getRoomList());

    // Add new room
    rooms.push_back(std::make_unique<Room>(roomName.ToStdString()));
    designer->getRoomList()->Append(roomName);

    // Restore previous selection
    if (prevSelection != wxNOT_FOUND) {
        designer->getRoomList()->SetSelection(prevSelection);
        selectedRoomIndex = prevSelection;
    }

    // Update device list if a room was previously selected
    if (prevSelection != wxNOT_FOUND && prevSelection < static_cast<int>(rooms.size())) {
        UpdateDeviceList(rooms[prevSelection].get());
    } else {
        designer->getDeviceList()->Clear();
    }
}

// Event: Add Device
void MainFrame::OnApplyRate(wxCommandEvent& event) {
    double rate;
    if (rateInput->GetValue().ToDouble(&rate)) {
        Device::costPerKWh = rate;
        UpdateEnergyUsage();
        wxMessageBox("Electricity rate updated!", "Success", wxOK | wxICON_INFORMATION);
    }
    else {
        wxMessageBox("Invalid rate format!", "Error", wxOK | wxICON_ERROR);
    }
}

void MainFrame::OnAddDevice(wxCommandEvent& event) {
    if (!designer || selectedRoomIndex == wxNOT_FOUND) return;

    // Get selected room safely
    if (selectedRoomIndex >= static_cast<int>(rooms.size())) {
        wxMessageBox("Invalid room selection!", "Error", wxOK | wxICON_ERROR);
        return;
    }
    Room* room = rooms[selectedRoomIndex].get();

    wxString deviceName = designer->getDeviceChoice()->GetStringSelection();
    if (deviceName.empty()) return;

    // Create appropriate device type
    std::unique_ptr<Device> newDevice;
    if (deviceName == "Smart Light") {
        newDevice = std::make_unique<SmartLight>(
            deviceName.ToStdString(),
            devicePowerMap[deviceName.ToStdString()],
            50  // Default brightness
        );
    }
    else if (deviceName == "Thermostat") {
        newDevice = std::make_unique<Thermostat>(
            deviceName.ToStdString(),
            devicePowerMap[deviceName.ToStdString()],
            22.0  // Default temperature
        );
    }
    else {
        newDevice = std::make_unique<Device>(
            deviceName.ToStdString(),
            devicePowerMap[deviceName.ToStdString()]
        );
    }

    room->addDevice(std::move(newDevice));
    UpdateDeviceList(room);
    UpdateEnergyUsage();
}

// Event: Remove Device
void MainFrame::OnRemoveDevice(wxCommandEvent& event) {
    if (!designer) return;

    int selectedRoomIndex = designer->getRoomList()->GetSelection();
    int selectedDeviceIndex = designer->getDeviceList()->GetSelection();

    if (selectedRoomIndex == wxNOT_FOUND || selectedDeviceIndex == wxNOT_FOUND) {
        wxMessageBox("Please select a device to remove!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    if (selectedRoomIndex >= static_cast<int>(rooms.size())) {
        wxMessageBox("Invalid room selection!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    Room* selectedRoom = rooms[selectedRoomIndex].get();
    if (!selectedRoom || !selectedRoom->removeDevice(selectedDeviceIndex)) {
        wxMessageBox("Failed to remove device!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    UpdateDeviceList(selectedRoom);
    UpdateEnergyUsage();
}

// Event: Room Selection
void MainFrame::OnRoomSelected(wxCommandEvent& event) {
    if (!designer || !designer->getRoomList()) return;

    selectedRoomIndex = designer->getRoomList()->GetSelection();

    if (selectedRoomIndex == wxNOT_FOUND || selectedRoomIndex >= static_cast<int>(rooms.size())) {
        wxMessageBox("Invalid room selection!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    Room* selectedRoom = rooms[selectedRoomIndex].get();
    if (!selectedRoom) return;

    UpdateDeviceList(selectedRoom);

    if (designer->getDeviceStatusCheckbox()) {
        designer->getDeviceStatusCheckbox()->SetValue(false);
    }

    // Preserve focus and prevent accidental selection changes
    designer->getRoomList()->SetFocus();
    designer->getRoomList()->SetSelection(selectedRoomIndex);

    event.Skip(false); // Prevent event propagation

    // Ensure immediate UI feedback
    designer->getRoomList()->Refresh();
    designer->getDeviceList()->Refresh();
}


// Event: Device Selection
void MainFrame::OnDeviceSelected(wxCommandEvent& event) {
    int selectedRoomIndex = designer->getRoomList()->GetSelection();
    int selectedDeviceIndex = designer->getDeviceList()->GetSelection();

    if (selectedRoomIndex == wxNOT_FOUND || selectedDeviceIndex == wxNOT_FOUND) {
        // No device selected, reset the checkbox
        designer->getDeviceStatusCheckbox()->SetValue(false);
        return;
    }

    Room* selectedRoom = rooms[selectedRoomIndex].get();
    if (!selectedRoom) return;

    Device* device = selectedRoom->getDevice(selectedDeviceIndex);
    if (!device) return;

    // Update the checkbox to reflect the device's current power state
    designer->getDeviceStatusCheckbox()->SetValue(device->isOn());
}

void MainFrame::OnToggleDevice(wxCommandEvent& event) {
    if (!designer) return;

    int selectedRoomIndex = designer->getRoomList()->GetSelection();
    int selectedDeviceIndex = designer->getDeviceList()->GetSelection();

    if (selectedRoomIndex == wxNOT_FOUND || selectedDeviceIndex == wxNOT_FOUND) {
        wxMessageBox("Please select a device to toggle!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    if (selectedRoomIndex >= static_cast<int>(rooms.size())) {
        wxMessageBox("Invalid room selection!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    Room* selectedRoom = rooms[selectedRoomIndex].get();
    if (!selectedRoom) return;

    Device* device = selectedRoom->getDevice(selectedDeviceIndex);
    if (!device) return;

    bool newState = designer->getDeviceStatusCheckbox()->GetValue();
    device->setOn(newState);

    UpdateDeviceList(selectedRoom);
    UpdateEnergyUsage();
}




void MainFrame::ShowHomeScreen() {
    if (!designer || !designer->getPanel()) {
        wxMessageBox("Designer panel not available!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    loginPanel->Hide();
    designer->getPanel()->Show();
    Layout();
    UpdateLightsBasedOnTime();

    // Ensure UI components exist before binding
    if (auto btn = designer->getAddRoomButton()) {
        btn->Bind(wxEVT_BUTTON, &MainFrame::OnAddRoom, this);
    }
    if (auto btn = designer->getAddDeviceButton()) {
        btn->Bind(wxEVT_BUTTON, &MainFrame::OnAddDevice, this);
    }
    if (auto btn = designer->getRemoveDeviceButton()) {
        btn->Bind(wxEVT_BUTTON, &MainFrame::OnRemoveDevice, this);
    }
    if (auto btn = designer->getShowEnergyButton()) {
        btn->Bind(wxEVT_BUTTON, &MainFrame::OnShowEnergyUsage, this);
    }
    if (auto btn = designer->getFireAlarmButton()) {
        btn->Bind(wxEVT_BUTTON, &MainFrame::OnTriggerFireAlarm, this);
    }
    if (auto chk = designer->getDeviceStatusCheckbox()) {
        chk->Bind(wxEVT_CHECKBOX, &MainFrame::OnToggleDevice, this);
    }

    Layout();
}

// Event: Show Energy Usage
void MainFrame::OnShowEnergyUsage(wxCommandEvent& event) {
    // Create report header
    wxString report = wxString::Format("Energy Usage Report\n\n");
    report += wxString::Format("Current Rate: $%.2f/kWh\n\n", Device::costPerKWh);

    bool hasHistory = false;

    // Process each room
    for (const auto& room : rooms) {
        if (!room || room->getDevices().empty()) continue;

        report += wxString::Format("=== %s ===\n", room->getName());

        // Process each device
        for (const auto& device : room->getDevices()) {
            if (!device) continue;

            wxString deviceStatus = device->isOn() ? "ON" : "OFF";
            report += wxString::Format("• %s [%s]\n",
                device->getName(),
                deviceStatus);

            auto history = device->getUsageHistory();
            if (history.empty()) {
                report += "  └ No usage history\n";
            }
            else {
                hasHistory = true;
                for (const auto& session : history) {
                    wxDateTime startTime(session.start);
                    wxDateTime endTime(session.end);

                    // Calculate duration in hours
                    double hours = std::difftime(session.end, session.start) / 3600.0;

                    report += wxString::Format(
                        "  + %s to %s\n"
                        "    | Duration: %.2f hours\n"
                        "    | Energy: %.2f kWh\n"
                        "    + Cost: $%.2f\n",
                        startTime.Format("%Y-%m-%d %H:%M"),
                        endTime.Format("%Y-%m-%d %H:%M"),
                        hours,
                        session.energy,
                        session.energy * Device::costPerKWh
                    );
                }
            }
            report += "\n";
        }
        report += "\n";
    }

    // Handle no-history case
    if (!hasHistory) {
        report += "\nNo energy usage data available.\n";
        report += "Tip: Devices must be turned OFF to record usage history.";
    }

    // Create dialog
    wxDialog reportDialog(this, wxID_ANY, "Energy Usage Report",
        wxDefaultPosition, wxSize(600, 400));
    wxTextCtrl* reportText = new wxTextCtrl(&reportDialog, wxID_ANY, report,
        wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH);

    // Configure font
    wxFont font(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    reportText->SetFont(font);

    // Layout
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(reportText, 1, wxEXPAND | wxALL, 10);
    reportDialog.SetSizer(sizer);
    reportDialog.Centre();
    reportDialog.ShowModal();
}

// Event: Fire Alarm Trigger
void MainFrame::OnTriggerFireAlarm(wxCommandEvent& event) {
    wxMessageBox("Fire alarm triggered! You have 5 seconds to respond.",
        "Fire Alarm", wxOK | wxICON_WARNING);

    if (!fireAlarmTimer->IsRunning()) {
        fireAlarmTimer->Start(5000);
        wxLogMessage("Fire alarm timer started");
    }
}

// Event: Fire Alarm Timeout
void MainFrame::OnFireAlarmTimeout(wxTimerEvent& event) {
    wxMessageBox("FIRE ALARM! Evacuate immediately!",
        "Critical Alert", wxOK | wxICON_ERROR);
}

void MainFrame::UpdateEnergyUsage() {
    double total = 0.0;
    for (auto& room : rooms) {
        total += room->calculateEnergyUsage();
    }

    if (designer && designer->getEnergyUsageLabel()) {
        designer->getEnergyUsageLabel()->SetLabel(
            wxString::Format("Energy Usage: %.2f kWh ($%.2f)",
                total,
                total * Device::costPerKWh)
        );
    }
}

wxString MainFrame::FormatTimeSpan(const wxTimeSpan& span) const {
    return span.Format("%H:%M:%S");
}
// Helper: Update Device List
void MainFrame::UpdateDeviceList(Room* room) {
    if (!designer || !room) return;

    wxListBox* deviceList = designer->getDeviceList();
    if (!deviceList) return;

    // Freeze UI to avoid flickering (alternative to wxWindowUpdateLocker)
    deviceList->Freeze();

    deviceList->Clear();

    for (const auto& device : room->getDevices()) {
        wxString status;
        if (device->isOn()) {
            if (auto light = dynamic_cast<SmartLight*>(device.get())) {
                status = wxString::Format(" [ON - %d%%]", light->getBrightness());
            }
            else if (auto thermo = dynamic_cast<Thermostat*>(device.get())) {
                status = wxString::Format(" [ON - %.1f°C]", thermo->getTemperature());
            }
            else {
                status = " [ON]";
            }
        }
        else {
            status = " [OFF]";
        }
        deviceList->Append(device->getName() + status);
    }

    // Restore selection after update
    if (selectedRoomIndex != wxNOT_FOUND) {
        designer->getRoomList()->SetSelection(selectedRoomIndex);
        designer->getRoomList()->Refresh();
    }

    // Unfreeze to allow UI refresh
    deviceList->Thaw();
}

void MainFrame::OnApplyTime(wxCommandEvent& event) {
    onHour = onTimeChoice->GetSelection() + 1;  // Convert index to hour (1-23)
    offHour = offTimeChoice->GetSelection() + 1;

    wxMessageBox(wxString::Format("Lights ON at %02d:00\nLights OFF at %02d:00",
        onHour, offHour), "Time Settings Applied", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnTimeSwitch(wxTimerEvent& event) {
    UpdateLightsBasedOnTime();
}
void MainFrame::UpdateLightsBasedOnTime() {
    int currentHour = wxDateTime::Now().GetHour();
    bool shouldBeOn;

    // Handle overnight ranges
    if (onHour < offHour) {
        shouldBeOn = (currentHour >= onHour && currentHour < offHour);
    }
    else {
        shouldBeOn = (currentHour >= onHour || currentHour < offHour);
    }

    // Update all lights in all rooms (background logic)
    for (auto& room : rooms) {
        for (auto& device : room->getDevices()) {
            wxString deviceName = device->getName();
            wxString deviceNameLower = deviceName.Lower(); // FIX applied

            if (deviceNameLower.Contains("light")) {
                device->setOn(shouldBeOn);
            }
        }
    }

    // Only update UI for the currently selected room
    if (selectedRoomIndex != wxNOT_FOUND && selectedRoomIndex < static_cast<int>(rooms.size())) {
        UpdateDeviceList(rooms[selectedRoomIndex].get());

        // Preserve room selection
        if (designer && designer->getRoomList()) {
            designer->getRoomList()->SetSelection(selectedRoomIndex);
            designer->getRoomList()->Refresh();
        }
    }

    UpdateEnergyUsage();
}

void MainFrame::OnDeviceSettings(wxCommandEvent& event) {
    if (!designer || selectedRoomIndex == wxNOT_FOUND) return;

    int deviceIndex = designer->getDeviceList()->GetSelection();
    if (deviceIndex == wxNOT_FOUND) return;

    Room* room = rooms[selectedRoomIndex].get();
    Device* device = room->getDevice(deviceIndex);

    if (auto light = dynamic_cast<SmartLight*>(device)) {
        wxNumberEntryDialog dlg(
            this,
            "Enter brightness (0-100):",
            "Smart Light Settings",
            "Brightness",
            light->getBrightness(),
            0, 100
        );

        if (dlg.ShowModal() == wxID_OK) {
            light->setBrightness(dlg.GetValue());
            UpdateDeviceList(room);
            UpdateEnergyUsage();
        }
    }
    else if (auto thermo = dynamic_cast<Thermostat*>(device)) {
        wxNumberEntryDialog dlg(
            this,
            "Enter temperature (°C):",
            "Thermostat Settings",
            "Temperature",
            static_cast<int>(thermo->getTemperature()),
            -20, 50
        );

        if (dlg.ShowModal() == wxID_OK) {
            thermo->setTemperature(dlg.GetValue());
            UpdateDeviceList(room);
            UpdateEnergyUsage();
        }
    }
    else {
        wxMessageBox("No settings available for this device type", "Info", wxOK | wxICON_INFORMATION);
    }
}