#include "MainFrame.h"
#include <wx/log.h>
#include<iomanip>
#include <wx/msgdlg.h>
#include <fstream>
#include <sstream>
#include <wx/datetime.h>
#include <wx/numdlg.h> 
 

MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)),
    designer(nullptr), fireAlarmTimer(nullptr), selectedRoomIndex(wxNOT_FOUND),
    onHour(18), offHour(6) {

   
    designer = new Designer(this);
    if (!designer || !designer->getPanel()) {
        wxMessageBox("Designer panel initialization failed!", "Error", wxOK | wxICON_ERROR);
        return;
    }



    devicePowerMap = {
        {"Light", 0.1},
        {"Smart Light", 0.15},
        {"Thermostat", 0.5},
        {"AC", 1.5},
        {"Fan", 0.3},
        {"Heater", 2.0},
        { "Television", 6.0 },
        {"Refrigerator", 2.5}
    };


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

    rateInput = new wxTextCtrl(this, wxID_ANY, "0.90");
    wxButton* applyRateBtn = new wxButton(this, wxID_ANY, "Apply Rate");

 
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);


    wxBoxSizer* timeSizer = new wxBoxSizer(wxHORIZONTAL);
    timeSizer->Add(new wxStaticText(this, wxID_ANY, "Lights ON:"), 0, wxALL, 5);
    timeSizer->Add(onTimeChoice, 0, wxALL, 5);
    timeSizer->Add(new wxStaticText(this, wxID_ANY, "Lights OFF:"), 0, wxALL, 5);
    timeSizer->Add(offTimeChoice, 0, wxALL, 5);
    timeSizer->Add(applyTimeButton, 0, wxALL, 5);

    wxBoxSizer* rateSizer = new wxBoxSizer(wxHORIZONTAL);
    rateSizer->Add(new wxStaticText(this, wxID_ANY, "Electricity Rate (BDT/kWh):"), 0, wxALL, 5);
    rateSizer->Add(rateInput, 0, wxALL, 5);
    rateSizer->Add(applyRateBtn, 0, wxALL, 5);

    mainSizer->Add(rateSizer, 0, wxALIGN_CENTER | wxALL, 10);
    mainSizer->Add(timeSizer, 0, wxALIGN_CENTER | wxALL, 10);

 
    loginPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(400, 250));
    loginPanel->SetBackgroundColour(wxColour(0, 150, 60));
    loginPanel->SetWindowStyle(wxBORDER_SIMPLE);

    wxBoxSizer* loginSizer = new wxBoxSizer(wxVERTICAL);

  
    wxStaticText* header = new wxStaticText(loginPanel, wxID_ANY, "Smart Home Login");
    header->SetFont(wxFont(18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    header->SetForegroundColour(wxColour(50, 50, 50));
    loginSizer->Add(header, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP | wxBOTTOM, 15);


    usernameInput = new wxTextCtrl(loginPanel, wxID_ANY, "", wxDefaultPosition, wxSize(250, 32), wxBORDER_SIMPLE);
    usernameInput->SetHint("Enter your username");
    usernameInput->SetBackgroundColour(wxColour(248, 248, 248));
    loginSizer->Add(usernameInput, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);


    passwordInput = new wxTextCtrl(loginPanel, wxID_ANY, "", wxDefaultPosition, wxSize(250, 32), wxTE_PASSWORD | wxBORDER_SIMPLE);
    passwordInput->SetHint("Enter your password");
    passwordInput->SetBackgroundColour(wxColour(248, 248, 248));
    loginSizer->Add(passwordInput, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);


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


    mainSizer->Add(loginPanel, 1, wxALIGN_CENTER | wxALL, 20);



    if (designer && designer->getPanel()) {
        mainSizer->Add(designer->getPanel(), 1, wxEXPAND | wxALL, 20);
        designer->getPanel()->Hide();
    }

    this->SetSizer(mainSizer);

    applyTimeButton->Bind(wxEVT_BUTTON, &MainFrame::OnApplyTime, this);
    applyRateBtn->Bind(wxEVT_BUTTON, &MainFrame::OnApplyRate, this);
    loginButton->Bind(wxEVT_BUTTON, &MainFrame::OnLogin, this);
    registerButton->Bind(wxEVT_BUTTON, &MainFrame::OnRegister, this);


    timeSwitchTimer = new wxTimer(this);
    Bind(wxEVT_TIMER, &MainFrame::OnTimeSwitch, this, timeSwitchTimer->GetId());
    timeSwitchTimer->Start(5000);


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

std::string MainFrame::GenerateSalt(size_t length) const {
    const std::string validChars =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> dist(0, validChars.size() - 1);

    std::string salt;
    salt.reserve(length);

    for (size_t i = 0; i < length; ++i) {
        salt += validChars[dist(generator)];
    }

    return salt;
}

std::string MainFrame::HashPassword(const std::string& password, const std::string& salt) const {
   
    std::string combined = salt + password;


    unsigned long hash = 5381;

    for (int i = 0; i < 100; ++i) { 
        for (char c : combined) {
            hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
        }
    
        for (char s : salt) {
            hash = ((hash << 3) + hash) + static_cast<unsigned char>(s);
        }
    }

    
    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << hash;
    return ss.str();
}


bool MainFrame::AuthenticateUser(const std::string& username, const std::string& password) {
    std::ifstream file("users.txt");
    std::string line, storedUser, storedSalt, storedHash;

    while (getline(file, line)) {
        std::istringstream ss(line);
        if (!(ss >> storedUser >> storedSalt >> storedHash)) continue;

        if (storedUser == username) {
            const std::string inputHash = HashPassword(password, storedSalt);
            return storedHash == inputHash;
        }
    }
    return false;
}


bool MainFrame::RegisterUser(const std::string& username, const std::string& password) {
    std::ifstream file("users.txt");
    std::string line, storedUser;


    while (getline(file, line)) {//if user exists
        std::istringstream ss(line);
        if (ss >> storedUser && storedUser == username) {
            return false;
        }
    }

  
    const std::string salt = GenerateSalt();
    const std::string hashedPassword = HashPassword(password, salt);

    // Storing all 3 val
    std::ofstream outFile("users.txt", std::ios::app);
    outFile << username << " " << salt << " " << hashedPassword << "\n";
    return true;
}



void MainFrame::OnAddRoom(wxCommandEvent& event) {
    if (!designer || !designer->getRoomChoice()) return;

    wxString roomName = designer->getRoomChoice()->GetStringSelection();
    if (roomName.IsEmpty()) {
        wxMessageBox("Select a room!", "Error", wxOK | wxICON_ERROR);
        return;
    }


    int prevSelection = designer->getRoomList()->GetSelection();//cur selection save

   
    wxEventBlocker blocker(designer->getRoomList());


    rooms.push_back(std::make_unique<Room>(roomName.ToStdString()));
    designer->getRoomList()->Append(roomName);

  
    if (prevSelection != wxNOT_FOUND) {//prev selection restore
        designer->getRoomList()->SetSelection(prevSelection);
        selectedRoomIndex = prevSelection;
    }

   
    if (prevSelection != wxNOT_FOUND && prevSelection < static_cast<int>(rooms.size())) { // Update device list if a room was previously selected
        UpdateDeviceList(rooms[prevSelection].get());
    } else {
        designer->getDeviceList()->Clear();
    }
}


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


    if (selectedRoomIndex >= static_cast<int>(rooms.size())) {
        wxMessageBox("Invalid room selection!", "Error", wxOK | wxICON_ERROR);
        return;
    }
    Room* room = rooms[selectedRoomIndex].get();

    wxString deviceName = designer->getDeviceChoice()->GetStringSelection();
    if (deviceName.empty()) return;

    
    std::unique_ptr<Device> newDevice;
    if (deviceName == "Smart Light") {
        newDevice = std::make_unique<SmartLight>(
            deviceName.ToStdString(),
            devicePowerMap[deviceName.ToStdString()],
            50  // Default 
        );
    }
    else if (deviceName == "Thermostat") {
        newDevice = std::make_unique<Thermostat>(
            deviceName.ToStdString(),
            devicePowerMap[deviceName.ToStdString()],
            22.0  // Default 
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

    
    designer->getRoomList()->SetFocus();// Preserve focus and prevent change abrupty
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

    wxEventBlocker blocker(designer->getDeviceStatusCheckbox());
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

    bool newState = !device->isOn(); // Directly toggle instead of using checkbox
    device->setOn(newState);
    designer->getDeviceStatusCheckbox()->SetValue(newState);

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


void MainFrame::OnShowEnergyUsage(wxCommandEvent& event) {
    // report header
    wxString report = wxString::Format("Energy Usage Report\n\n");
    report += wxString::Format("Current Rate: BDT%.2f/kWh\n\n", Device::costPerKWh);

    bool hasHistory = false;

    
    for (const auto& room : rooms) {
        if (!room || room->getDevices().empty()) continue;

        report += wxString::Format("=== %s ===\n", room->getName());

     
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

                    
                    double hours = std::difftime(session.end, session.start) / 3600.0;

                    report += wxString::Format(
                        "  + %s to %s\n"
                        "    | Duration: %.2f hours\n"
                        "    | Energy: %.2f kWh\n"
                        "    + Cost: BDT%.2f\n",
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
    if (!hasHistory) {
        report += "\nNo energy usage data available.\n";
        report += "Tip: Devices must be turned OFF to record usage history.";
    }

 
    wxDialog reportDialog(this, wxID_ANY, "Energy Usage Report",
        wxDefaultPosition, wxSize(600, 400));
    wxTextCtrl* reportText = new wxTextCtrl(&reportDialog, wxID_ANY, report,
        wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH);

   
    wxFont font(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    reportText->SetFont(font);

    // Layout
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(reportText, 1, wxEXPAND | wxALL, 10);
    reportDialog.SetSizer(sizer);
    reportDialog.Centre();
    reportDialog.ShowModal();
}


void MainFrame::OnTriggerFireAlarm(wxCommandEvent& event) {
    wxMessageBox("Fire alarm triggered! You have 5 seconds to respond.",
        "Fire Alarm", wxOK | wxICON_WARNING);

    if (!fireAlarmTimer->IsRunning()) {
        fireAlarmTimer->Start(5000);
        wxLogMessage("Fire alarm timer started");
    }
}


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
            wxString::Format("Energy Usage: %.2f kWh (BDT%.2f)",
                total,
                total * Device::costPerKWh)
        );
    }
}

wxString MainFrame::FormatTimeSpan(const wxTimeSpan& span) const {
    return span.Format("%H:%M:%S");
}


void MainFrame::UpdateDeviceList(Room* room) {
    if (!designer || !room) return;

    wxListBox* deviceList = designer->getDeviceList();
    if (!deviceList) return;

    int selectedDeviceIndex = deviceList->GetSelection();

    // Freeze UI 
    wxEventBlocker blocker(deviceList);
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

 
    if (selectedDeviceIndex != wxNOT_FOUND && selectedDeviceIndex < static_cast<int>(room->getDevices().size())) {
        deviceList->SetSelection(selectedDeviceIndex);
    }
 
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

    // overnight range
    if (onHour < offHour) {
        shouldBeOn = (currentHour >= onHour && currentHour < offHour);
    }
    else {
        shouldBeOn = (currentHour >= onHour || currentHour < offHour);
    }

 
    for (auto& room : rooms) {
        for (auto& device : room->getDevices()) {
            wxString deviceName = device->getName();
            wxString deviceNameLower = deviceName.Lower(); 

            if (deviceNameLower.Contains("light")) {
                device->setOn(shouldBeOn);
            }
        }
    }


    if (selectedRoomIndex != wxNOT_FOUND && selectedRoomIndex < static_cast<int>(rooms.size())) {
        UpdateDeviceList(rooms[selectedRoomIndex].get());

     
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







