#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>
#include <wx/timer.h>
#include <random>
#include <algorithm>
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


    double GetElectricityRate() const { return electricityCostPerKWh; }

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

    
    wxPanel* loginPanel;
    wxTextCtrl* usernameInput;
    wxTextCtrl* passwordInput;
    wxButton* loginButton;
    wxButton* registerButton;
    wxTextCtrl* rateInput;
  
    double electricityCostPerKWh;
    std::unordered_map<std::string, double> devicePowerMap;

   
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

   
    wxString FormatTimeSpan(const wxTimeSpan& span) const;


    std::string GenerateSalt(size_t length = 16) const ;
    std::string HashPassword(const std::string& password, const std::string& salt) const; 


};

#endif // MAINFRAME_H