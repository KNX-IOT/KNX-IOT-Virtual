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
#include <wx/cmdline.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#define NO_MAIN
#include "knx_iot_virtual_pb.h"

#include "api/oc_knx_dev.h"
#include "api/oc_knx_fp.h"

enum
{
  BUTTON_1 = wxID_HIGHEST + 1, // ID for button 1
  BUTTON_2 = wxID_HIGHEST + 2, // ID for button 2
  BUTTON_3 = wxID_HIGHEST + 3, // ID for button 3
  BUTTON_4 = wxID_HIGHEST + 4, // ID for button 4
  TIMER_ID = BUTTON_4 + 1,   // ID for timer
  CHECK_PM = TIMER_ID + 1,   // programming mode check in menu bar
  CHECK_1 = CHECK_PM + 1,  // ID for check 1
  CHECK_2 = CHECK_1 + 1,  // ID for check 2
  CHECK_3 = CHECK_2 + 1,  // ID for check 3
  CHECK_4 = CHECK_3 + 1,  // ID for check 4
  RESET = CHECK_4 + 1,  // ID for reset button in the menu
  IA_TEXT = RESET + 1, // ID for internal address text 
  IID_TEXT = IA_TEXT + 1,// ID for installation id text 
  PM_TEXT = IID_TEXT + 1,// ID for programming mode text 
  LS_TEXT = PM_TEXT + 1,// ID for load status text 
  HOSTNAME_TEXT = LS_TEXT + 1,// ID for hostname text 
  SECURED_TEXT = HOSTNAME_TEXT + 1, // ID for secured text 
  GOT_TABLE_ID = SECURED_TEXT + 1, // ID for the Group object window button
  PARAMETER_LIST_ID = GOT_TABLE_ID + 1 // ID for the parameter list window button

};


class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
  MyFrame(char* serial_number);
private:
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnGroupObjectTable(wxCommandEvent& event);
    void OnParameterList(wxCommandEvent& event);
    void OnProgrammingMode(wxCommandEvent& event);
    void OnReset(wxCommandEvent& event);

    void OnPressed1(wxCommandEvent& event);
    void OnPressed2(wxCommandEvent& event);
    void OnPressed3(wxCommandEvent& event);
    void OnPressed4(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event);

    void updateInfoCheckBoxes();
    void updateTextButtons();

    wxMenu* m_menuFile;
    wxTimer m_timer;        // timer for the oc_poll call for the stack
    wxCheckBox* m_pm_1;  // menu toggle
    wxCheckBox* m_check_1;  // push button 1
    wxCheckBox* m_check_2;  // push button 2
    wxCheckBox* m_check_3;  // push button 3
    wxCheckBox* m_check_4;  // push button 4

    wxTextCtrl* m_ia_text;   // text control for internal address
    wxTextCtrl* m_iid_text; // text control for installation id
    wxTextCtrl* m_pm_text; // text control for programming mode
    wxTextCtrl* m_ls_text; // text control for load state
    wxTextCtrl* m_hostname_text; // text control for host name
    wxTextCtrl* m_secured_text; // text for secured/not secured
    wxTextCtrl* m_debug_text; // Debug console output
};


static const wxCmdLineEntryDesc g_cmdLineDesc[] =
{
    { wxCMD_LINE_OPTION, "s", "serialnumber", "serial number", wxCMD_LINE_VAL_STRING },
    { wxCMD_LINE_NONE }
};

wxCmdLineParser* g_cmd;

wxIMPLEMENT_APP(MyApp);
bool MyApp::OnInit()
{
  int argc = wxAppConsole::argc;
  wxChar** argv = wxAppConsole::argv;

  g_cmd = new wxCmdLineParser(argc, argv);
  g_cmd->SetDesc(g_cmdLineDesc);
  g_cmd->Parse(true);

  wxString serial_number;
  if (g_cmd->Found("s", &serial_number)) {

  }

    MyFrame *frame = new MyFrame((char*)(serial_number.c_str()).AsChar());
    //frame->Fit();
    frame->SetSize(wxSize(320, 800));  // length x height
    frame->Show(true);
    return true;
}
MyFrame::MyFrame(char* str_serial_number)
    : wxFrame(NULL, wxID_ANY, "KNX-IOT virtual Push Button")
{
    m_menuFile = new wxMenu;
    m_menuFile->Append(GOT_TABLE_ID, "List Group Object Table", "List the Group object table", false);
    //m_menuFile->Append(PARAMETER_LIST_ID, "List Parameters", "List the parameters of the device", false);
    m_menuFile->Append(CHECK_PM, "Programming Mode", "Sets the application in programming mode", true);
    m_menuFile->Append(RESET, "Reset (ex-factory)", "Reset the Device to ex-factory state", false);
    m_menuFile->AppendSeparator();
    m_menuFile->Append(wxID_EXIT);
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(m_menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar( menuBar );
    CreateStatusBar();
    SetStatusText("Welcome to KNX Virtual!");
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &MyFrame::OnReset, this, RESET);
    Bind(wxEVT_MENU, &MyFrame::OnGroupObjectTable, this, GOT_TABLE_ID);
    Bind(wxEVT_MENU, &MyFrame::OnParameterList, this, PARAMETER_LIST_ID);
    Bind(wxEVT_MENU, &MyFrame::OnProgrammingMode, this, CHECK_PM);

    wxButton* btn_1 = new wxButton(this, BUTTON_1, _T("Button 1 ('/p/1')"), wxPoint(10, 10 ), wxSize(130, 25), 0);
    btn_1->Bind(wxEVT_BUTTON, &MyFrame::OnPressed1, this);
    wxButton* btn_2 = new wxButton(this, BUTTON_2, _T("Button 2 ('/p/3')"), wxPoint(10, 10 + 25), wxSize(130, 25), 0);
    btn_2->Bind(wxEVT_BUTTON, &MyFrame::OnPressed2, this);
    wxButton* btn_3 = new wxButton(this, BUTTON_3, _T("Button 3 ('/p/5')"), wxPoint(10, 10 + 50), wxSize(130, 25), 0);
    btn_3->Bind(wxEVT_BUTTON, &MyFrame::OnPressed3, this);
    wxButton* btn_4 = new wxButton(this, BUTTON_4, _T("Button 4 ('/p/7')"), wxPoint(10, 10 + 75), wxSize(130, 25), 0);
    btn_4->Bind(wxEVT_BUTTON, &MyFrame::OnPressed4, this);


    m_check_1 = new 	wxCheckBox(this, BUTTON_1, _T("FeedBack 1 ('/p/2')"), wxPoint(150, 10), wxSize(130, 25), 0);
    m_check_1->Enable(false);
    m_check_2 = new 	wxCheckBox(this, BUTTON_2, _T("FeedBack 2 ('/p/4')"), wxPoint(150, 10 + 25), wxSize(130, 25), 0);
    m_check_2->Enable(false);
    m_check_3 = new 	wxCheckBox(this, BUTTON_3, _T("FeedBack 3 ('/p/6')"), wxPoint(150, 10 + 50), wxSize(130, 25), 0);
    m_check_3->Enable(false);
    m_check_4 = new 	wxCheckBox(this, BUTTON_4, _T("FeedBack 4 ('/p/8')"), wxPoint(150, 10 + 75), wxSize(130, 25), 0);
    m_check_4->Enable(false);

    if (strlen(str_serial_number) > 1) {
      app_set_serial_number(str_serial_number);
    }
    app_initialize_stack();

    // serial number
    char text[500];
    strcpy(text, "Device Serial Number: -sn ");
    oc_device_info_t* device = oc_core_get_device_info(0);
    strcat(text, oc_string(device->serialnumber));
    wxTextCtrl* Statictext;
    Statictext = new wxTextCtrl(this, wxID_ANY, text, wxPoint(10, 10 + 100), wxSize(150 + 130, 25), 0);
    Statictext->SetEditable(false);

    // internal address
    sprintf(text, "IA: %d", device->ia);
    m_ia_text = new wxTextCtrl(this, IA_TEXT, text, wxPoint(10, 10 + 125), wxSize(140 , 25), 0);
    m_ia_text->SetEditable(false);

    // installation id
    sprintf(text, "IID: %d", device->iid);
    m_iid_text = new wxTextCtrl(this, IID_TEXT, text, wxPoint(10 + 140, 10 + 125), wxSize(140, 25), 0);
    m_iid_text->SetEditable(false);

    // programming mode
    sprintf(text, "Programming Mode: %d", device->pm);
    m_pm_text = new wxTextCtrl(this, PM_TEXT, text, wxPoint(10, 10 + 150), wxSize(140, 25), 0);
    m_pm_text->SetEditable(false);

    // installation id
    sprintf(text, "LoadState: %s", oc_core_get_lsm_state_as_string(device->lsm_s));
    m_ls_text = new wxTextCtrl(this, LS_TEXT, text, wxPoint(10 + 140, 10 + 150), wxSize(140, 25), 0);
    m_ls_text->SetEditable(false);

    // hostname
    sprintf(text, "host name: %s", oc_string(device->hostname));
    m_hostname_text = new wxTextCtrl(this, LS_TEXT, text, wxPoint(10 , 10 + 175), wxSize(140, 25), 0);
    m_hostname_text->SetEditable(false);

    if (app_is_secure()) {
      strcpy(text, "secured");
    } else {
      strcpy(text, "unsecured");
    }
    m_secured_text = new wxTextCtrl(this, LS_TEXT, text, wxPoint(10 + 140 , 10 + 175), wxSize(140, 25), wxTE_RICH);
    m_secured_text->SetEditable(false);
    if (app_is_secure() == false) {
      m_secured_text->SetStyle(0, 100, (wxTextAttr(*wxRED)));
    }

    m_debug_text = new wxTextCtrl(this, LS_TEXT, text, wxPoint(10, 10 + 200), wxSize(280, 500), wxTE_RICH | wxTE_MULTILINE);
    m_debug_text->SetEditable(false);

    this->updateInfoCheckBoxes();
    this->updateTextButtons();

    m_timer.Bind(wxEVT_TIMER, &MyFrame::OnTimer, this);
    m_timer.Start(1, wxTIMER_CONTINUOUS);  // 1 mili interval

}
void MyFrame::OnExit(wxCommandEvent& event)
{
  oc_main_shutdown();
  Close(true);
}

void MyFrame::updateTextButtons()
{

  char text[500];
  size_t device_index = 0;

  // get the device data structure
  oc_device_info_t* device = oc_core_get_device_info(device_index);
  if (device == NULL) {
    return;
  }
  // update the text labels
  sprintf(text, "IA: %d", device->ia);
  m_ia_text->SetLabelText(text);
  sprintf(text, "LoadState: %s", oc_core_get_lsm_state_as_string(device->lsm_s));
  m_pm_text->SetLabelText(text);
  sprintf(text, "Programming Mode : % d", device->pm);
  m_ls_text->SetLabelText(text);
  sprintf(text, "IID: %d", device->iid);
  m_iid_text->SetLabelText(text);
  sprintf(text, "host name: %s", oc_string(device->hostname));
  m_hostname_text->SetLabelText(text);
}
void MyFrame::OnGroupObjectTable(wxCommandEvent& event)
{
  int device_index = 0;
  char text[1024 * 5];
  char line[200];
  char line2[200];
  char windowtext[200];

  strcpy(text, "");

  oc_device_info_t* device = oc_core_get_device_info(device_index);
  if (device == NULL) {
    return;
  }

  int total = oc_core_get_group_object_table_total_size();
  for (int index = 0; index < total; index++) {
    oc_group_object_table_t* entry = oc_core_get_group_object_table_entry(index);
    if (entry->ga_len > 0) {
      sprintf(line, "Index %d \n", index);
      strcat(text, line);
      sprintf(line, "  id: '%d'  ", entry->id);
      strcat(text, line);
      sprintf(line, "  url: '%s' ", oc_string(entry->href));
      strcat(text, line);
      sprintf(line, "  cflags : '%d' ", entry->cflags);
      oc_cflags_as_string(line, entry->cflags);
      strcat(text, line);
      strcpy(line, "  ga : [");
      for (int i = 0; i < entry->ga_len; i++) {
        sprintf(line2, " %d", entry->ga[i]);
        strcat(line, line2);
      }
      strcat(line, " ]\n");
      strcat(text, line);
    }
  }

  strcpy(windowtext, "Group Object Table  ");
  strcat(windowtext, oc_string(device->serialnumber));

  wxMessageBox(text, windowtext,
    wxOK | wxICON_NONE);

  SetStatusText("List Group Object Table");

}


void MyFrame::OnParameterList(wxCommandEvent& event)
{
  int device_index = 0;
  char text[1024 * 5];
  char line[200];
  char windowtext[200];

  strcpy(text, "");

  oc_device_info_t* device = oc_core_get_device_info(device_index);
  if (device == NULL) {
    return;
  }

  int index = 1;
  char* url = app_get_parameter_url(index);
  while (url)
  {
    sprintf(line, "Index %d \n", index);
    strcat(text, line);
    sprintf(line, "  name: '%s'  ", app_get_parameter_name(index));
    strcat(text, line);
    sprintf(line, "  url : '%s'  ", url);
    strcat(text, line);
    if (app_is_bool_url(url)) {
      sprintf(line, "  value : '%d'  ", app_retrieve_bool_variable(url));
      strcat(text, line);
    }
    if (app_is_int_url(url)) {
      sprintf(line, "  value : '%d'  ", app_retrieve_int_variable(url));
      strcat(text, line);
    }
    if (app_is_string_url(url)) {
      sprintf(line, "  value : '%s'  ", app_retrieve_string_variable(url));
      strcat(text, line);
    }
    index++;
    url = app_get_parameter_url(index);
  }

  strcpy(windowtext, "Parameter List ");
  strcat(windowtext, oc_string(device->serialnumber));

  wxMessageBox(text, windowtext,
    wxOK | wxICON_NONE);

  SetStatusText("List Parameters and their current set values");

}



void MyFrame::OnProgrammingMode(wxCommandEvent& event)
{
  int device_index = 0;
  SetStatusText("Changing programming mode");

  bool my_val = m_menuFile->IsChecked(CHECK_PM);
  oc_device_info_t* device = oc_core_get_device_info(0);
  device->pm = my_val;

  // update the UI
  this->updateTextButtons();
}

void MyFrame::OnReset(wxCommandEvent& event)
{
  int device_index = 0;
  SetStatusText("Device Reset");

  // reset the device
  oc_knx_device_storage_reset(device_index, 2);
  // update the UI
  this->updateTextButtons();
}


void MyFrame::OnAbout(wxCommandEvent& event)
{
  char text[500];

  strcpy(text, "KNX-IOT virtual for push button\n");
  strcat(text, "\nDevice Serial Number: ");
  oc_device_info_t* device = oc_core_get_device_info(0);
  strcat(text, oc_string(device->serialnumber));

  strcat(text, "\n\n (c) Cascoda Ltd\n (c) KNX.org");

  wxMessageBox(text, "KNX-IOT virtual Push Button",
    wxOK | wxICON_NONE);
}

void MyFrame::OnPressed1(wxCommandEvent& event)
{
  char url[] = "/p/1";
  char my_text[100];
  bool p1 = app_retrieve_bool_variable(url);

  if (p1 == true) {
    p1 = false;
  } else {
    p1 = true;
  }
  app_set_bool_variable(url, p1);
  oc_do_s_mode_with_scope(2, url, "w");
  oc_do_s_mode_with_scope(5, url, "w");
  sprintf(my_text,"Button 1 ('%s') pressed: %d", url, (int)p1);
  SetStatusText(my_text);
}

void MyFrame::OnPressed2(wxCommandEvent& event)
{
  char url[] = "/p/3";
  char my_text[100];
  bool p7 = app_retrieve_bool_variable(url);

  if (p7 == true) {
    p7 = false;
  }
  else {
    p7 = true;
  }
  app_set_bool_variable(url, p7);
  oc_do_s_mode_with_scope(2, url, "w");
  oc_do_s_mode_with_scope(5, url, "w");
  sprintf(my_text, "Button 2 ('%s') pressed: %d", url, (int)p7);
  SetStatusText(my_text);
}

void MyFrame::OnPressed3(wxCommandEvent& event)
{
  char url[] = "/p/5";
  char my_text[100];
  bool p7 = app_retrieve_bool_variable(url);

  if (p7 == true) {
    p7 = false;
  }
  else {
    p7 = true;
  }
  app_set_bool_variable(url, p7);
  oc_do_s_mode_with_scope(2, url, "w");
  oc_do_s_mode_with_scope(5, url, "w");
  sprintf(my_text, "Button 3 ('%s') pressed: %d", url, (int)p7);
  SetStatusText(my_text);
}

void MyFrame::OnPressed4(wxCommandEvent& event)
{
  char url[] = "/p/7";
  char my_text[100];
  bool p7 = app_retrieve_bool_variable(url);

  if (p7 == true) {
    p7 = false;
  }
  else {
    p7 = true;
  }
  app_set_bool_variable(url, p7);
  oc_do_s_mode_with_scope(2, url, "w");
  oc_do_s_mode_with_scope(5, url, "w");
  sprintf(my_text, "Button 4 ('%s') pressed: %d", url, (int)p7);
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
  this->updateTextButtons();
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