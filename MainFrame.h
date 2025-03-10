#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>
#include <wx/timer.h>
#include <map>
#include <ctime>
#include <vector>
#include <memory>
#include <unordered_map>
#include "Designer.h"
#include "Room.h"
#include "Device.h"
#include "SmartLight.h"
#include "Thermostat.h"

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title);
    ~MainFrame();

private:
    Designer* designer;
    std::vector<std::unique_ptr<Room>> rooms;
    wxTimer* fireAlarmTimer;
    wxTimer* timeSwitchTimer;
    wxChoice* onTimeChoice;
    wxChoice* offTimeChoice;
    wxButton* applyTimeButton;
  
    int onHour;
    int offHour;
    int selectedRoomIndex;

    // Login UI
    wxPanel* loginPanel;
    wxTextCtrl* usernameInput;
    wxTextCtrl* passwordInput;
    wxButton* loginButton;
    wxButton* registerButton;
    wxTextCtrl* rateInput;
    // Energy-related members
    double electricityCostPerKWh;
    std::unordered_map<std::string, double> devicePowerMap;

    // Event Handlers
    void OnLogin(wxCommandEvent& event);
    void OnRegister(wxCommandEvent& event);
    void OnAddRoom(wxCommandEvent& event);
    void OnAddDevice(wxCommandEvent& event);
    void OnRemoveDevice(wxCommandEvent& event);
    void OnRoomSelected(wxCommandEvent& event);
    void OnDeviceSelected(wxCommandEvent& event);
    void OnToggleDevice(wxCommandEvent& event);
    void OnShowEnergyUsage(wxCommandEvent& event);
    void OnTriggerFireAlarm(wxCommandEvent& event);
    void OnFireAlarmTimeout(wxTimerEvent& event);
    void RefreshRoomListSelection();
    void  SyncRoomSelection();

    // Helper Methods
    bool AuthenticateUser(const std::string& username, const std::string& password);
    bool RegisterUser(const std::string& username, const std::string& password);
    void ShowHomeScreen();
    void UpdateEnergyUsage();
    void UpdateDeviceList(Room* room);
    void OnApplyTime(wxCommandEvent& event);
    void OnTimeSwitch(wxTimerEvent& event);
    void UpdateLightsBasedOnTime();
    void OnApplyRate(wxCommandEvent& event);
    void OnDeviceSettings(wxCommandEvent& event);

    // New energy calculation methods
    double CalculateDeviceConsumption(const Device& device) const;
    wxString GenerateEnergyRecommendations() const;
    wxString FormatTimeSpan(const wxTimeSpan& span) const;

    // Public getter for electricity rate
public:
    double GetElectricityRate() const { return electricityCostPerKWh; }
};

#endif // MAINFRAME_H