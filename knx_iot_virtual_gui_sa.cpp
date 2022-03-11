/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 Copyright (c) 2022 Cascoda Ltd
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#define NO_MAIN
#include "knx_iot_virtual_pb.h"

enum
{
  BUTTON_1 = wxID_HIGHEST + 1, // declares an id which will be used to call our button
  BUTTON_2 = wxID_HIGHEST + 2, // declares an id which will be used to call our button
  BUTTON_3 = wxID_HIGHEST + 3, // declares an id which will be used to call our button
  BUTTON_4 = wxID_HIGHEST + 4, // declares an id which will be used to call our button
  TIMER_ID = BUTTON_4 + 1,
  CHECK_1 = TIMER_ID + 1,
  CHECK_2 = CHECK_1 + 1,
  CHECK_3 = CHECK_2 + 1,
  CHECK_4 = CHECK_3 + 1,
  FAULT_1 = CHECK_4 + 1,
  FAULT_2 = FAULT_1 + 1,
  FAULT_3 = FAULT_2 + 1,
  FAULT_4 = FAULT_3 + 1

};


class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
    MyFrame();
private:
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnFault1(wxCommandEvent& event);
    void OnFault2(wxCommandEvent& event);
    void OnFault3(wxCommandEvent& event);
    void OnFault4(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event);

    void updateInfoCheckBoxes();
    void updateInfoButtons();
    void bool2text(bool on_off, char* text);

    wxTimer m_timer;

    wxButton* m_btn_1;
    wxButton* m_btn_2;
    wxButton* m_btn_3;
    wxButton* m_btn_4;

    wxCheckBox* m_check_1;
    wxCheckBox* m_check_2;
    wxCheckBox* m_check_3;
    wxCheckBox* m_check_4;

    wxCheckBox* m_fault_1;
    wxCheckBox* m_fault_2;
    wxCheckBox* m_fault_3;
    wxCheckBox* m_fault_4;
};

wxIMPLEMENT_APP(MyApp);
bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame();
    frame->Show(true);
    return true;
}
MyFrame::MyFrame()
    : wxFrame(NULL, wxID_ANY, "KNX virtual Switch Actuator")
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(wxID_EXIT);
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar( menuBar );
    CreateStatusBar();
    SetStatusText("Welcome to KNX Virtual!");
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);

    m_btn_1 = new wxButton(this, BUTTON_1, _T("Button 1"), wxPoint(10, 10 ), wxSize(80, 25), 0);
    m_btn_1->Enable(false);
    m_btn_2 = new wxButton(this, BUTTON_2, _T("Button 2"), wxPoint(10, 10 + 25), wxSize(80, 25), 0);
    m_btn_2->Enable(false);
    m_btn_3 = new wxButton(this, BUTTON_3, _T("Button 3"), wxPoint(10, 10 + 50), wxSize(80, 25), 0);
    m_btn_3->Enable(false);
    m_btn_4 = new wxButton(this, BUTTON_4, _T("Button 4"), wxPoint(10, 10 + 75), wxSize(80, 25), 0);
    m_btn_4->Enable(false);

    m_check_1 = new 	wxCheckBox(this, BUTTON_1, _T("FeedBack 1"), wxPoint(100, 10), wxSize(80, 25), 0);
    m_check_1->Enable(false);
    m_check_2 = new 	wxCheckBox(this, BUTTON_2, _T("FeedBack 2"), wxPoint(100, 10 + 25), wxSize(80, 25), 0);
    m_check_2->Enable(false);
    m_check_3 = new 	wxCheckBox(this, BUTTON_3, _T("FeedBack 3"), wxPoint(100, 10 + 50), wxSize(80, 25), 0);
    m_check_3->Enable(false);
    m_check_4 = new 	wxCheckBox(this, BUTTON_4, _T("FeedBack 4"), wxPoint(100, 10 + 75), wxSize(80, 25), 0);
    m_check_4->Enable(false);

    m_fault_1 = new 	wxCheckBox(this, BUTTON_1, _T("Fault 1"), wxPoint(190, 10), wxSize(80, 25), 0);
    m_fault_1->Bind(wxEVT_CHECKBOX, &MyFrame::OnFault1, this);
    m_fault_2 = new 	wxCheckBox(this, BUTTON_2, _T("Fault 2"), wxPoint(190, 10 + 25), wxSize(80, 25), 0);
    m_fault_2->Bind(wxEVT_CHECKBOX, &MyFrame::OnFault2, this);
    m_fault_3 = new 	wxCheckBox(this, BUTTON_3, _T("Fault 3"), wxPoint(190, 10 + 50), wxSize(80, 25), 0);
    m_fault_3->Bind(wxEVT_CHECKBOX, &MyFrame::OnFault3, this);
    m_fault_4 = new 	wxCheckBox(this, BUTTON_4, _T("Fault 4"), wxPoint(190, 10 + 75), wxSize(80, 25), 0);
    m_fault_4->Bind(wxEVT_CHECKBOX, &MyFrame::OnFault4, this);


    app_initialize_stack();

    char text[500];

    strcpy(text, "Device Serial Number: ");
    oc_device_info_t* device = oc_core_get_device_info(0);
    strcat(text, oc_string(device->serialnumber));

    wxTextCtrl* Statictext;
    Statictext = new wxTextCtrl(this, wxID_ANY, text, wxPoint(10, 10 + 100), wxSize(80 * 3, 25), 0);
    Statictext->SetEditable(false);

    this->updateInfoCheckBoxes();

    m_timer.Bind(wxEVT_TIMER, &MyFrame::OnTimer, this);
    m_timer.Start(1, wxTIMER_CONTINUOUS);  // 1 mili interval

}
void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}
void MyFrame::OnAbout(wxCommandEvent& event)
{
  char text[500];

  strcpy(text, "KNX-IOT virtual for switch actuator\n");
  strcat(text, "\nDevice Serial Number: ");
  oc_device_info_t* device = oc_core_get_device_info(0);
  strcat(text, oc_string(device->serialnumber));

  strcat(text, "\n\n (c) Cascoda Ltd\n (c) KNX.org");

  wxMessageBox(text, "KNX-IOT virtual Switch Actuator",
    wxOK | wxICON_NONE);
}

void MyFrame::OnFault1(wxCommandEvent& event)
{
  char url[] = "/p/1";
  char my_text[100];
  bool p1 = app_retrieve_bool_variable(url);
  app_set_fault_variable(url, p1);

  sprintf(my_text,"Actuator 1 Fault: %d", (int)p1);
  SetStatusText(my_text);
}

void MyFrame::OnFault2(wxCommandEvent& event)
{
  char url[] = "/p/3";
  char my_text[100];
  bool p1 = app_retrieve_bool_variable(url);
  app_set_fault_variable(url, p1);

  sprintf(my_text, "Actuator 2 Fault: %d", (int)p1);
  SetStatusText(my_text);
}

void MyFrame::OnFault3(wxCommandEvent& event)
{
  char url[] = "/p/5";
  char my_text[100];
  bool p1 = app_retrieve_bool_variable(url);
  app_set_fault_variable(url, p1);

  sprintf(my_text, "Actuator 3 Fault: %d", (int)p1);
  SetStatusText(my_text);
}

void MyFrame::OnFault4(wxCommandEvent& event)
{
  char url[] = "/p/7";
  char my_text[100];
  bool p1 = app_retrieve_bool_variable(url);
  app_set_fault_variable(url, p1);

  sprintf(my_text, "Actuator 4 Fault: %d", (int)p1);
  SetStatusText(my_text);
}

void MyFrame::OnTimer(wxTimerEvent& event)
{
  // do whatever you want to do every millisecond here
  oc_clock_time_t next_event;
  next_event = oc_main_poll();
  //wxLogMessage("on pressed 4!");
  //SetStatusText(".");
  this->updateInfoCheckBoxes();
  this->updateInfoButtons();
}


void  MyFrame::updateInfoCheckBoxes()
{

  bool p = app_retrieve_bool_variable("/p/2");
  m_check_1->SetValue(p);
  p = app_retrieve_bool_variable("/p/4");
  m_check_2->SetValue(p);
  p = app_retrieve_bool_variable("/p/6");
  m_check_3->SetValue(p);
  p = app_retrieve_bool_variable("/p/8");
  m_check_4->SetValue(p);
}

void MyFrame::bool2text(bool on_off, char* text)
{
  strcpy(text, "Button 1 ");
  if (on_off) {
    strcat(text, " On");
  }
  else {
    strcat(text, " Off");
  }
}

void  MyFrame::updateInfoButtons()
{
  char text[200];
  bool p = app_retrieve_bool_variable("/p/1");
  strcpy(text, "Button 1 ");
  this->bool2text(p, text);
  m_btn_1->SetLabel(text);

  p = app_retrieve_bool_variable("/p/3");
  strcpy(text, "Button 2 ");
  this->bool2text(p, text);
  m_btn_2->SetLabel(text);
  
  p = app_retrieve_bool_variable("/p/5");
  strcpy(text, "Button 3 ");
  this->bool2text(p, text);
  m_btn_3->SetLabel(text);

  p = app_retrieve_bool_variable("/p/7");
  strcpy(text, "Button 4 ");
  this->bool2text(p, text);
  m_btn_4->SetLabel(text);
}