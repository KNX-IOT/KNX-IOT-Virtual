/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 Copyright (c) 2022-2023 Cascoda Ltd
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
// 2023-04-17 14:49:14.324366

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#include <wx/cmdline.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#define NO_MAIN
#include "knx_iot_virtual_pb.h"
#include "api/oc_knx_dev.h"
#include "api/oc_knx_sec.h"
#include "api/oc_knx_fp.h"
#include "port/dns-sd.h"

enum
{
  RESET = wxID_HIGHEST + 1,   // ID for reset button in the menu
  RESET_TABLE = RESET + 1,    // ID for clear table button in the menu
  IA_TEXT = RESET_TABLE + 1,  // ID for internal address text 
  IID_TEXT = IA_TEXT + 1,     // ID for installation id text 
  PM_TEXT = IID_TEXT + 1,     // ID for programming mode text 
  LS_TEXT = PM_TEXT + 1,      // ID for load status text 
  HOSTNAME_TEXT = LS_TEXT + 1, // ID for hostname text 
  GOT_TABLE_ID = HOSTNAME_TEXT + 1, // ID for the Group object window
  PUB_TABLE_ID = GOT_TABLE_ID + 1, // ID for the publisher table window
  REC_TABLE_ID = PUB_TABLE_ID + 1, // ID for the recipient table window
  PARAMETER_LIST_ID = REC_TABLE_ID + 1, // ID for the parameter window
  AT_TABLE_ID = PARAMETER_LIST_ID + 1, // ID for the auth/at window
  CHECK_GA_DISPLAY = AT_TABLE_ID + 1 , // ga display check
  CHECK_IID_DISPLAY = CHECK_GA_DISPLAY + 1, // iid display check
  CHECK_GRPID_DISPLAY = CHECK_IID_DISPLAY + 1, // grpid display check
  CHECK_SLEEPY = CHECK_GRPID_DISPLAY + 1 , // sleepy check
  CHECK_PM = CHECK_SLEEPY + 1 , // programming mode check in menu bar
  DP_ID_ONOFF_1 = CHECK_PM + 1, // OnOff_1 for /p/o_1_1
  DP_ID_INFOONOFF_1 = CHECK_PM + 2, // InfoOnOff_1 for /p/o_2_2
  DP_ID_ONOFF_2 = CHECK_PM + 3, // OnOff_2 for /p/o_3_3
  DP_ID_INFOONOFF_2 = CHECK_PM + 4, // InfoOnOff_2 for /p/o_4_4
  DP_ID_ONOFF_3 = CHECK_PM + 5, // OnOff_3 for /p/o_5_5
  DP_ID_INFOONOFF_3 = CHECK_PM + 6, // InfoOnOff_3 for /p/o_6_6
  DP_ID_ONOFF_4 = CHECK_PM + 7, // OnOff_4 for /p/o_7_7
  DP_ID_INFOONOFF_4 = CHECK_PM + 8 // InfoOnOff_4 for /p/o_8_8
};

static const wxCmdLineEntryDesc g_cmdLineDesc[] =
{
  { wxCMD_LINE_OPTION, "s", "serialnumber", "serial number", wxCMD_LINE_VAL_STRING },
  { wxCMD_LINE_NONE }
};

wxCmdLineParser* g_cmd;



class CustomDialog : public wxDialog
{
public:
  CustomDialog(const wxString& title, const wxString& text);

private:
  void on_close(wxCommandEvent& event);
};

void CustomDialog::on_close(wxCommandEvent& event)
{
  this->Destroy();
}

CustomDialog::CustomDialog(const wxString& title, const wxString& text)
  : wxDialog(NULL, -1, title, wxDefaultPosition, wxSize(550, 300))
{
  int size_x = 520;
  int size_y = 300;

  wxPanel* panel = new wxPanel(this, -1);

  wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

  wxTextCtrl* tc = new wxTextCtrl(panel, -1, text, wxPoint(10, 10),
    wxSize(size_x, size_y), wxTE_MULTILINE | wxTE_READONLY);

  wxButton* closeButton = new wxButton(this, -1, wxT("Close"),
    wxDefaultPosition, wxDefaultSize);
  closeButton->Bind(wxEVT_BUTTON, &CustomDialog::on_close, this);

  hbox->Add(closeButton, 1, wxLEFT, 5);
  vbox->Add(panel, 1);
  vbox->Add(hbox, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

  SetSizerAndFit(vbox);
  Centre();
  ShowModal();
  Destroy();
}

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
  void OnGroupObjectTable(wxCommandEvent& event);
  void OnPublisherTable(wxCommandEvent& event);
  void OnRecipientTable(wxCommandEvent& event);
  void OnParameterList(wxCommandEvent& event);
  void OnAuthTable(wxCommandEvent& event);
  void OnProgrammingMode(wxCommandEvent& event);
  void OnSleepyMode(wxCommandEvent& event);
  void OnReset(wxCommandEvent& event);
  void OnClearTables(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnTimer(wxTimerEvent& event);
  void OnPressed_OnOff_1(wxCommandEvent& event); 
  void OnPressed_OnOff_2(wxCommandEvent& event); 
  void OnPressed_OnOff_3(wxCommandEvent& event); 
  void OnPressed_OnOff_4(wxCommandEvent& event); 

  void updateInfoCheckBoxes();
  void updateInfoButtons();
  void updateTextButtons();
  void bool2text(bool on_off, char* text);
  void int2text(int value, char* text, bool as_ets=true);
  void int2grpidtext(uint64_t value, char* text, bool as_ets);
  void int2scopetext(uint32_t value, char* text);
  void double2text(double value, char* text);

  wxMenu* m_menuFile;
  wxMenu* m_menuDisplay;
  wxMenu* m_menuOptions;
  wxTimer m_timer;
  
  // sleepy information
  int m_sleep_counter = 0;
  int m_sleep_seconds = 20;

  wxTextCtrl* m_ia_text;  // text control for internal address
  wxTextCtrl* m_iid_text; // text control for installation id
  wxTextCtrl* m_pm_text;  // text control for programming mode
  wxTextCtrl* m_ls_text;  // text control for load state
  wxTextCtrl* m_hostname_text; // text control for host name
  wxTextCtrl* m_secured_text; // text secure/not secure
  //DP_ID_ONOFF_1 bool
  wxButton* m_ONOFF_1; // OnOff_1 if.s 
  //DP_ID_INFOONOFF_1 bool
  wxCheckBox* m_INFOONOFF_1 ; // InfoOnOff_1 if.a 
  //DP_ID_ONOFF_2 bool
  wxButton* m_ONOFF_2; // OnOff_2 if.s 
  //DP_ID_INFOONOFF_2 bool
  wxCheckBox* m_INFOONOFF_2 ; // InfoOnOff_2 if.a 
  //DP_ID_ONOFF_3 bool
  wxButton* m_ONOFF_3; // OnOff_3 if.s 
  //DP_ID_INFOONOFF_3 bool
  wxCheckBox* m_INFOONOFF_3 ; // InfoOnOff_3 if.a 
  //DP_ID_ONOFF_4 bool
  wxButton* m_ONOFF_4; // OnOff_4 if.s 
  //DP_ID_INFOONOFF_4 bool
  wxCheckBox* m_INFOONOFF_4 ; // InfoOnOff_4 if.a 

};

wxIMPLEMENT_APP(MyApp);

/**
 * @brief initialization of the application
 * 
 * @return true 
 * @return false 
 */
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

  MyFrame* frame = new MyFrame((char*)(serial_number.c_str()).AsChar());

  frame->Fit();
  frame->Show(true);
  return true;
}

/**
 * @brief Construct a new My Frame:: My Frame object
 * 
 * @param str_serial_number 
 */
MyFrame::MyFrame(char* str_serial_number)
    : wxFrame(NULL, wxID_ANY, "KNX virtual Push Button")
{
  m_menuFile = new wxMenu;
  m_menuFile->Append(GOT_TABLE_ID, "List Group Object Table", "List the Group object table", false);
  m_menuFile->Append(PUB_TABLE_ID, "List Publisher Table", "List the Publisher table", false);
  m_menuFile->Append(REC_TABLE_ID, "List Recipient Table", "List the Recipient table", false);
  m_menuFile->Append(PARAMETER_LIST_ID, "List Parameters", "List the parameters of the device", false);
  m_menuFile->Append(AT_TABLE_ID, "List Auth/AT Table", "List the security data of the device", false);
  m_menuFile->Append(CHECK_PM, "Programming Mode", "Sets the application in programming mode", true);
  m_menuFile->Append(RESET_TABLE, "Reset (7) (Tables)", "Reset 7 (Reset to default without IA).", false);
  m_menuFile->Append(RESET, "Reset (2)(ex-factory)", "Reset 2 (Reset to default state)", false);
  m_menuFile->AppendSeparator();
  m_menuFile->Append(wxID_EXIT);
  // display menu
   // display menu
  m_menuDisplay = new wxMenu;
  m_menuDisplay->Append(CHECK_GA_DISPLAY, "GA 3-level (ETS)", "Displays the group addresses as GA 3-Level or as integer", true);
  m_menuDisplay->Check(CHECK_GA_DISPLAY, true);
  m_menuDisplay->Append(CHECK_GRPID_DISPLAY, "GRPID as partial ipv6 address (ETS)", "Displays the grpid as integer", true);
  m_menuDisplay->Check(CHECK_GRPID_DISPLAY, true);
  m_menuDisplay->Append(CHECK_IID_DISPLAY, "IID as partial ipv6 address (ETS)", "Displays the iid as integer", true);
  m_menuDisplay->Check(CHECK_IID_DISPLAY, true);
  // Option menu
  m_menuOptions = new wxMenu;
  m_menuOptions->Append(CHECK_SLEEPY, "Act as Sleepy Device", "Sleeps for 20 seconds", true);
  m_menuOptions->Check(CHECK_SLEEPY, false);
  // help menu
  wxMenu *menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);
  // full menu bar
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(m_menuFile, "&File");
  menuBar->Append(m_menuDisplay, "&Display");
  menuBar->Append(m_menuOptions, "&Options");
  menuBar->Append(menuHelp, "&Help");
  SetMenuBar( menuBar );
  CreateStatusBar();
  SetStatusText("Welcome to KNX virtual Push Button!");
  Bind(wxEVT_MENU, &MyFrame::OnReset, this, RESET);
  Bind(wxEVT_MENU, &MyFrame::OnClearTables, this, RESET_TABLE);
  Bind(wxEVT_MENU, &MyFrame::OnGroupObjectTable, this, GOT_TABLE_ID);
  Bind(wxEVT_MENU, &MyFrame::OnPublisherTable, this, PUB_TABLE_ID);
  Bind(wxEVT_MENU, &MyFrame::OnRecipientTable, this, REC_TABLE_ID);
  Bind(wxEVT_MENU, &MyFrame::OnParameterList, this, PARAMETER_LIST_ID);
  Bind(wxEVT_MENU, &MyFrame::OnAuthTable, this, AT_TABLE_ID);
  Bind(wxEVT_MENU, &MyFrame::OnProgrammingMode, this, CHECK_PM);
  Bind(wxEVT_MENU, &MyFrame::OnSleepyMode, this, CHECK_SLEEPY);
  Bind(wxEVT_MENU, &MyFrame::OnReset, this, RESET);
  Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
  int x_width = 230; /* width of the widgets */
  int x_height = 25; /* height of the widgets */
  int max_instances = 4;
  int max_dp_count = 2;
  int index;
  int row;
  int column;
  int column_offset = 0;
  index = 1-1;
  row = 1 -1;
  column = (index % max_dp_count) - column_offset;
  //DP_ID_ONOFF_1
  // if.s  ==> sensor == possible to change value in UI
  m_ONOFF_1 = new wxButton(this, DP_ID_ONOFF_1, _T("OnOff_1 ('/p/o_1_1')"), wxPoint(10 + column*x_width, 10 + (x_height*row)), wxSize(x_width, x_height), 0); 
  m_ONOFF_1->Bind(wxEVT_BUTTON, &MyFrame::OnPressed_OnOff_1, this);       
  index = 2-1;
  row = 1 -1;
  column = (index % max_dp_count) - column_offset;
  //DP_ID_INFOONOFF_1 
  m_INFOONOFF_1 = new wxCheckBox(this, DP_ID_INFOONOFF_1, _T("InfoOnOff_1 ('/p/o_2_2')"), wxPoint(10 + column*x_width, 10 + (x_height*row)), wxSize(x_width, x_height), 0); 
  m_INFOONOFF_1->Enable(false);      
  index = 3-1;
  row = 2 -1;
  column = (index % max_dp_count) - column_offset;
  //DP_ID_ONOFF_2
  // if.s  ==> sensor == possible to change value in UI
  m_ONOFF_2 = new wxButton(this, DP_ID_ONOFF_2, _T("OnOff_2 ('/p/o_3_3')"), wxPoint(10 + column*x_width, 10 + (x_height*row)), wxSize(x_width, x_height), 0); 
  m_ONOFF_2->Bind(wxEVT_BUTTON, &MyFrame::OnPressed_OnOff_2, this);       
  index = 4-1;
  row = 2 -1;
  column = (index % max_dp_count) - column_offset;
  //DP_ID_INFOONOFF_2 
  m_INFOONOFF_2 = new wxCheckBox(this, DP_ID_INFOONOFF_2, _T("InfoOnOff_2 ('/p/o_4_4')"), wxPoint(10 + column*x_width, 10 + (x_height*row)), wxSize(x_width, x_height), 0); 
  m_INFOONOFF_2->Enable(false);      
  index = 5-1;
  row = 3 -1;
  column = (index % max_dp_count) - column_offset;
  //DP_ID_ONOFF_3
  // if.s  ==> sensor == possible to change value in UI
  m_ONOFF_3 = new wxButton(this, DP_ID_ONOFF_3, _T("OnOff_3 ('/p/o_5_5')"), wxPoint(10 + column*x_width, 10 + (x_height*row)), wxSize(x_width, x_height), 0); 
  m_ONOFF_3->Bind(wxEVT_BUTTON, &MyFrame::OnPressed_OnOff_3, this);       
  index = 6-1;
  row = 3 -1;
  column = (index % max_dp_count) - column_offset;
  //DP_ID_INFOONOFF_3 
  m_INFOONOFF_3 = new wxCheckBox(this, DP_ID_INFOONOFF_3, _T("InfoOnOff_3 ('/p/o_6_6')"), wxPoint(10 + column*x_width, 10 + (x_height*row)), wxSize(x_width, x_height), 0); 
  m_INFOONOFF_3->Enable(false);      
  index = 7-1;
  row = 4 -1;
  column = (index % max_dp_count) - column_offset;
  //DP_ID_ONOFF_4
  // if.s  ==> sensor == possible to change value in UI
  m_ONOFF_4 = new wxButton(this, DP_ID_ONOFF_4, _T("OnOff_4 ('/p/o_7_7')"), wxPoint(10 + column*x_width, 10 + (x_height*row)), wxSize(x_width, x_height), 0); 
  m_ONOFF_4->Bind(wxEVT_BUTTON, &MyFrame::OnPressed_OnOff_4, this);       
  index = 8-1;
  row = 4 -1;
  column = (index % max_dp_count) - column_offset;
  //DP_ID_INFOONOFF_4 
  m_INFOONOFF_4 = new wxCheckBox(this, DP_ID_INFOONOFF_4, _T("InfoOnOff_4 ('/p/o_8_8')"), wxPoint(10 + column*x_width, 10 + (x_height*row)), wxSize(x_width, x_height), 0); 
  m_INFOONOFF_4->Enable(false);      

  if (strlen(str_serial_number) > 1) {
    app_set_serial_number(str_serial_number);
  }
  app_initialize_stack();

  int width_size = 180; /* size of the knx info widgets */
  // serial number
  char text[500];
  strcpy(text, "Device Serial Number: -sn ");
  oc_device_info_t* device = oc_core_get_device_info(0);
  strcat(text, oc_string(device->serialnumber));
  wxTextCtrl* Statictext;
  Statictext = new wxTextCtrl(this, wxID_ANY, text, wxPoint(10, 10 + ((max_instances + 1) * x_height)), wxSize(width_size*2, x_height), 0);
  Statictext->SetEditable(false);
  
  /* QR code
    KNX:S:serno;P:password
    where:
    KNX: is a fixed prefix
    S: means a KNX serial number follows, serno itself is encodced as 
       12 upper-case hexadecimal characters
    P: means a password follows, password itself is just 
       the KNX IoT Point API password; 
       this works as the allowed password characters do not interfere 
       with the separator characters colon and semicolon and are in the Alphanumeric range.
  */
  char qrtext[500];
  strcpy(qrtext, "QR info:   KNX:S:");
  strcat(qrtext, oc_string(device->serialnumber));
  strcat(qrtext, ";P:");
  strcat(qrtext, app_get_password());
  wxTextCtrl* Statictext2;
  Statictext2 = new wxTextCtrl(this, wxID_ANY, qrtext, wxPoint(10, 10 + ((max_instances + 2) * x_height)), wxSize(width_size*2, x_height), 0);
  Statictext2->SetEditable(false);

  // internal address
  sprintf(text, "IA: %d", device->ia);
  m_ia_text = new wxTextCtrl(this, IA_TEXT, text, wxPoint(10, 10 + ((max_instances + 3) * x_height)), wxSize(width_size, x_height), 0);
  m_ia_text->SetEditable(false);
  // installation id
  sprintf(text, "IID: %lld", device->iid);
  m_iid_text = new wxTextCtrl(this, IID_TEXT, text, wxPoint(10 + width_size, 10 + ((max_instances + 3) * x_height)), wxSize(width_size, x_height), 0);
  m_iid_text->SetEditable(false);
  // programming mode
  sprintf(text, "Programming Mode: %d", device->pm);
  m_pm_text = new wxTextCtrl(this, PM_TEXT, text, wxPoint(10, 10 + ((max_instances + 4) * x_height)), wxSize(width_size, x_height), 0);
  m_pm_text->SetEditable(false);
  // installation id
  sprintf(text, "LoadState: %s", oc_core_get_lsm_state_as_string(device->lsm_s));
  m_ls_text = new wxTextCtrl(this, LS_TEXT, text, wxPoint(10 + width_size, 10 + ((max_instances + 4) * 25)), wxSize(width_size, 25), 0);
  m_ls_text->SetEditable(false);
  // host name
  sprintf(text, "host name: %s", oc_string(device->hostname));
  m_hostname_text = new wxTextCtrl(this, LS_TEXT, text, wxPoint(10, 10 + ((max_instances + 5) * 25)), wxSize(width_size, 25), 0);
  m_hostname_text->SetEditable(false);
  if (app_is_secure()) {
    strcpy(text, app_get_password());
    //strcpy(text, "secured");
  }
  else {
    strcpy(text, "unsecured");
  }
  m_secured_text = new wxTextCtrl(this, LS_TEXT, text, wxPoint(10 + width_size,  10 + ((max_instances + 5) * 25)), wxSize(width_size, 25), wxTE_RICH);
  m_secured_text->SetEditable(false);
  if (app_is_secure() == false) {
    m_secured_text->SetStyle(0, 100, (wxTextAttr(*wxRED)));
  }
  // update the UI
  this->updateInfoCheckBoxes();
  this->updateTextButtons();
  this->updateInfoCheckBoxes();
  // start the timer for UI updates and stack polls
  m_timer.Bind(wxEVT_TIMER, &MyFrame::OnTimer, this);
  m_timer.Start(1, wxTIMER_CONTINUOUS);  // 1 millisecond interval
}

/**
 * @brief exit the application
 * 
 * @param event command triggered by the framework
 */
void MyFrame::OnExit(wxCommandEvent& event)
{
  Close(true);
}

/**
 * @brief checks/unchecks the programming mode
 * 
 * @param event command triggered by the menu button
 */
void MyFrame::OnProgrammingMode(wxCommandEvent& event)
{
  int device_index = 0;
  SetStatusText("Changing programming mode");

  bool my_val = m_menuFile->IsChecked(CHECK_PM);
  oc_device_info_t* device = oc_core_get_device_info(0);
  device->pm = my_val;

  // update the UI
  this->updateTextButtons();
  // update mdns
  knx_publish_service(oc_string(device->serialnumber), device->ia, device->iid, device->pm);
}


/**
 * @brief checks/unchecks the sleepy mode
 * 
 * @param event command triggered by the menu button
 */
void MyFrame::OnSleepyMode(wxCommandEvent& event)
{
 int device_index = 0;
  SetStatusText("Changing sleepy mode");

  bool my_sleepy = m_menuOptions->IsChecked(CHECK_SLEEPY);
  oc_device_info_t* device = oc_core_get_device_info(0);
  
  if (my_sleepy) {
    knx_service_sleep_period(20);
  } else {
    knx_service_sleep_period(0);
  }
  // update mdns
  knx_publish_service(oc_string(device->serialnumber), device->ia, device->iid, device->pm);
}

/**
 * @brief update the text buttons
 * - IA
 * - Loadstate
 * - programming mode
 * - IID
 * - Hostname
 */
void MyFrame::updateTextButtons()
{

  char text[500];
  size_t device_index = 0;
  bool iid_conversion = m_menuDisplay->IsChecked(CHECK_IID_DISPLAY);

  // get the device data structure
  oc_device_info_t* device = oc_core_get_device_info(device_index);
  // update the text labels
  // ia_0 == AAxxxxxx = AA
  // ia_1 == xxAAxxxx = AA
  // ia_2 == xxxxAAAA = AAAA
  uint32_t ia = device->ia;
  uint32_t ia_o = (ia >> 12);
  uint32_t ia_1 = (ia >> 8) & 0xF;
  uint32_t ia_2 = (ia & 0x000000FF);
  sprintf(text, "IA: %d.%d.%d   [%d]", ia_o, ia_1, ia_2, device->ia);
  m_ia_text->SetLabelText(text);
  sprintf(text, "LoadState: %s", oc_core_get_lsm_state_as_string(device->lsm_s));
  m_pm_text->SetLabelText(text);
  sprintf(text, "Programming Mode : % d", device->pm);
  m_ls_text->SetLabelText(text);
  strcpy(text, "IID: ");
  this->int2grpidtext(device->iid, text, iid_conversion);
  m_iid_text->SetLabelText(text);
  sprintf(text, "host name: %s", oc_string(device->hostname));
  m_hostname_text->SetLabelText(text);
  // reset the programming mode to what the device has
  m_menuFile->Check(CHECK_PM, device->pm);
}

/**
 * @brief clear the tables of the device
 * 
 * @param event command triggered by button in the menu
 */
void MyFrame::OnClearTables(wxCommandEvent& event)
{
  int device_index = 0;
  SetStatusText("Clear Tables");
  // reset the device
  oc_knx_device_storage_reset(device_index, 7);
  // update the UI
  this->updateTextButtons();
}

/**
 * @brief reset the device
 * 
 * @param event command triggered by button in the menu
 */
void MyFrame::OnReset(wxCommandEvent& event)
{
  int device_index = 0;
  SetStatusText("Device Reset");
  // reset the device
  oc_knx_device_storage_reset(device_index, 2);
  // update the UI
  this->updateTextButtons();
}

/**
 * @brief shows the group object table in a window
 * 
 * @param event command triggered by a menu button
 */
void MyFrame::OnGroupObjectTable(wxCommandEvent& event)
{
  int device_index = 0;
  char text[1024 * 5];
  char line[200];
  char windowtext[200];
  bool ga_conversion = m_menuDisplay->IsChecked(CHECK_GA_DISPLAY);

  strcpy(text, "");
  oc_device_info_t* device = oc_core_get_device_info(device_index);
  if (device == NULL) {
    return;
  }
  int total = oc_core_get_group_object_table_total_size();
  for (int index = 0; index < total; index++) {
    oc_group_object_table_t* entry = oc_core_get_group_object_table_entry(index);

    if (entry && entry->ga_len > 0) {
      sprintf(line, "Index %d \n", index);
      strcat(text, line);
      sprintf(line, "  id: '%d'  ", entry->id);
      strcat(text, line);
      sprintf(line, "  url: '%s' ", oc_string(entry->href));
      strcat(text, line);
      sprintf(line, "  cflags : '%d' ", (int)entry->cflags);
      oc_cflags_as_string(line, entry->cflags);
      strcat(text, line);
      strcpy(line,"  ga : [");
      for (int i = 0; i < entry->ga_len; i++) {
        this->int2text(entry->ga[i], line, ga_conversion);
      }
      strcat(line," ]\n");
      strcat(text, line);
    }
  }
  strcpy(windowtext, "Group Object Table  ");
  strcat(windowtext, oc_string(device->serialnumber));
  CustomDialog(windowtext, text);
  SetStatusText("List Group Object Table");
}

/**
 * @brief shows the Publisher table in a window
 * 
 * @param event command triggered by a menu button
 */
void MyFrame::OnPublisherTable(wxCommandEvent& event)
{
  int device_index = 0;
  char text[1024 * 5];
  char line[200];
  char windowtext[200];
  bool ga_conversion = m_menuDisplay->IsChecked(CHECK_GA_DISPLAY);
  bool grpid_conversion = m_menuDisplay->IsChecked(CHECK_GRPID_DISPLAY);
  bool iid_conversion = m_menuDisplay->IsChecked(CHECK_IID_DISPLAY);

  strcpy(text, "");
  oc_device_info_t* device = oc_core_get_device_info(device_index);
  if (device == NULL) {
    return;
  }
  
  int total =  oc_core_get_publisher_table_size();
  for (int index = 0; index < total; index++) {
    oc_group_rp_table_t* entry = oc_core_get_publisher_table_entry(index);

    if (entry && entry->id >= 0) {
      sprintf(line, "Index %d \n", index);
      strcat(text, line);
      sprintf(line, "  id: '%d'  ", entry->id);
      strcat(text, line);
       if ( entry->ia >= 0) {
        sprintf(line, "  ia: '%d' ", entry->ia);
        strcat(text, line);
      }
      if ( entry->iid >= 0) {
        strcpy(line, "  iid: ");
        this->int2grpidtext(entry->iid, line, iid_conversion);
        strcat(text, line);
      }
      if ( entry->fid >= 0) {
        sprintf(line, "  fid: '%lld' ", entry->fid);
        strcat(text, line);
      }
      if ( entry->grpid > 0) {
        //sprintf(line, "  grpid: '%u' ", entry->grpid);
        strcpy(line, "  grpid: ");
        this->int2grpidtext(entry->grpid, line, grpid_conversion);
        strcat(text, line);
      }
      if (oc_string_len(entry->url) > 0) {
        sprintf(line, "  url: '%s' ", oc_string(entry->url));
        strcat(text, line);
      }
      if (oc_string_len(entry->path) > 0) {
        sprintf(line, "  path: '%s' ", oc_string(entry->path));
        strcat(text, line);
      }
      if ( entry->ga_len > 0) {
        strcpy(line,"  ga : [");
        for (int i = 0; i < entry->ga_len; i++) {
          this->int2text(entry->ga[i], line, ga_conversion);
        }
        strcat(line," ]\n");
        strcat(text, line);
      }
    }
  }
  strcpy(windowtext, "Publisher Table  ");
  strcat(windowtext, oc_string(device->serialnumber));
  CustomDialog(windowtext, text);
  SetStatusText("List Publisher Table");
}

/**
 * @brief shows the Recipient table in a window
 * 
 * @param event command triggered by a menu button
 */
void MyFrame::OnRecipientTable(wxCommandEvent& event)
{
  int device_index = 0;
  char text[1024 * 5];
  char line[200];
  char windowtext[200];
  bool ga_conversion = m_menuDisplay->IsChecked(CHECK_GA_DISPLAY);
  bool grpid_conversion = m_menuDisplay->IsChecked(CHECK_GRPID_DISPLAY);
  bool iid_conversion = m_menuDisplay->IsChecked(CHECK_IID_DISPLAY);

  strcpy(text, "");
  oc_device_info_t* device = oc_core_get_device_info(device_index);
  if (device == NULL) {
    return;
  }

  int total =  oc_core_get_recipient_table_size();
  for (int index = 0; index < total; index++) {
    oc_group_rp_table_t* entry = oc_core_get_recipient_table_entry(index);

    if (entry && entry->id >= 0) {
      sprintf(line, "Index %d \n", index);
      strcat(text, line);
      sprintf(line, "  id: '%d'  ", entry->id);
      strcat(text, line);
      if ( entry->ia >= 0) {
        sprintf(line, "  ia: '%d' ", entry->ia);
        strcat(text, line);
      }
      if ( entry->iid >= 0) {
        strcpy(line, "  iid: ");
        this->int2grpidtext(entry->iid, line, iid_conversion);
        strcat(text, line);
      }
      if ( entry->fid >= 0) {
        sprintf(line, "  fid: '%lld' ", entry->fid);
        strcat(text, line);
      }
      if ( entry->grpid >= 0) {
        strcpy(line, "  grpid: ");
        this->int2grpidtext(entry->grpid, line, grpid_conversion);
        strcat(text, line);
      }
      if (oc_string_len(entry->url) > 0) {
        sprintf(line, "  url: '%s' ", oc_string(entry->url));
        strcat(text, line);
      }
      if (oc_string_len(entry->path) > 0){
        sprintf(line, "  path: '%s' ", oc_string(entry->path));
        strcat(text, line);
      }
      if ( entry->ga_len > 0) {
        strcpy(line,"  ga : [");
        for (int i = 0; i < entry->ga_len; i++) {
          this->int2text(entry->ga[i], line, ga_conversion);
        }
        strcat(line," ]\n");
        strcat(text, line);
      }
    }
  }
  strcpy(windowtext, "Recipient Table  ");
  strcat(windowtext, oc_string(device->serialnumber));
  CustomDialog(windowtext, text);
  SetStatusText("List Recipient Table");
}
/**
 * @brief shows a window containing the parameters and current values of the application
 * 
 * @param event command triggered by a menu button
 */
void MyFrame::OnParameterList(wxCommandEvent& event)
{
  int device_index = 0;
  char text[1024 + (200*0)];
  char line[200];
  char windowtext[200];

  strcpy(text, "");

  oc_device_info_t* device = oc_core_get_device_info(device_index);
  if (device == NULL) {
    return;
  }

  int index = 1;
  char* url = app_get_parameter_url(index);
  if (url == NULL) {
    strcat(text, "no parameters in this device");
  }
  while (url) {
    sprintf(line, "\nIndex %02d ", index);
    strcat(text, line);
    sprintf(line, "  url : '%s'  ", url);
    strcat(text, line);
    char* name = app_get_parameter_name(index);
    if (name) {
      sprintf(line, "  name: '%s'  ", app_get_parameter_name(index));
      strcat(text, line);
    }
    if (app_is_bool_url(url)) {
      sprintf(line, "  value : '%d'  ", app_retrieve_bool_variable(url));
      strcat(text, line);
    }
    if (app_is_int_url(url)) {
      sprintf(line, "  value : '%d'  ", app_retrieve_int_variable(url));
      strcat(text, line);
    }
    if (app_is_double_url(url)) {
      sprintf(line, "  value : '%f'  ", (float)app_retrieve_double_variable(url));
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
  //wxMessageBox(text, windowtext,
  //  wxOK | wxICON_NONE);
  CustomDialog(windowtext, text);
  SetStatusText("List Parameters and their current set values");
}

/**
 * @brief shows the (loaded) auth/at table
 * 
 * @param event command triggered by a menu button
 */
void MyFrame::OnAuthTable(wxCommandEvent& event)
{
  int device_index = 0;
  char text[1024 * 10];
  char line[500];
  bool ga_conversion = m_menuDisplay->IsChecked(CHECK_GA_DISPLAY);
  char windowtext[200];
  int max_entries = oc_core_get_at_table_size();
  int index = 1;

  oc_device_info_t* device = oc_core_get_device_info(device_index);
  if (device == NULL) {
    return;
  }

  strcpy(text, "");
  for (index = 0; index < max_entries; index++) {

    oc_auth_at_t* my_entry = oc_get_auth_at_entry(device_index, index);
    if (my_entry != NULL) {
      if (oc_string_len(my_entry->id)) {
        sprintf(line, "index : '%d' id = '%s' \n", index, oc_string(my_entry->id));
        strcat(text, line);
        sprintf(line, "  profile : %d (%s)\n", my_entry->profile,
          oc_at_profile_to_string(my_entry->profile));
        strcat(text, line);
        if (my_entry->profile == OC_PROFILE_COAP_DTLS) {
          if (oc_string_len(my_entry->sub) > 0) {
            sprintf(line, "    sub           : %s\n", oc_string(my_entry->sub));
            strcat(text, line);
          }
          if (oc_string_len(my_entry->kid) > 0) {
            sprintf(line, "  kid : %s\n", oc_string(my_entry->kid));
            strcat(text, line);
          }
        }
        if (my_entry->profile == OC_PROFILE_COAP_OSCORE) {
          if (oc_string_len(my_entry->osc_id) > 0) {
            sprintf(line, "  osc_id : %s\n",
              oc_string(my_entry->osc_id));
            strcat(text, line);
          }
          if (oc_string_len(my_entry->osc_ms) > 0) {
            sprintf(line, "  osc_ms : ");
            strcat(text, line);
            int length = (int)oc_string_len(my_entry->osc_ms);
            char* ms = oc_string(my_entry->osc_ms);
            for (int i = 0; i < length; i++) {
              sprintf(line, "%02x", (unsigned char)ms[i]);
              strcat(text, line);
            }
            sprintf(line, "\n");
            strcat(text, line);
          }
          if (oc_string_len(my_entry->osc_alg) > 0) {
            sprintf(line, "  osc_alg : %s\n",
              oc_string(my_entry->osc_alg));
            strcat(text, line);
          }
          if (oc_string_len(my_entry->osc_contextid) > 0) {
            sprintf(line, "  osc_contextid : %s\n",
              oc_string(my_entry->osc_contextid));
            strcat(text, line);
          }
          if (my_entry->ga_len > 0) {
            sprintf(line, "  osc_ga : [");
            strcat(text, line);
            for (int i = 0; i < my_entry->ga_len; i++) {
              this->int2text(my_entry->ga[i], text, ga_conversion);
            }
            sprintf(line, " ]\n");
            strcat(text, line);
          } else {
            sprintf(line, "  scope : ", my_entry->scope);
            this->int2scopetext(my_entry->scope, line);
            strcat(text, line);
            strcat(text, "\n");
          }
        }
      }
    }
  }

  strcpy(windowtext, "Auth AT Table ");
  strcat(windowtext, oc_string(device->serialnumber));
  CustomDialog(windowtext, text);
  SetStatusText("List security entries");
}

/**
 * @brief shows static info about the application
 * 
 * @param event command triggered by a menu button
 */
void MyFrame::OnAbout(wxCommandEvent& event)
{
  char text[500 + (200* 8)];
  strcpy(text, "KNX virtual Push Button\n");
  strcat(text, "\nDevice Serial Number: ");
  oc_device_info_t* device = oc_core_get_device_info(0);
  strcat(text, oc_string(device->serialnumber));
  strcat(text,"\n");
  strcat(text,"manufactorer     : cascoda\n");
  strcat(text,"model            : KNX virtual - PB\n");
  strcat(text,"hardware type    : Linux/windows\n");
  strcat(text,"hardware version : [0, 1, 3]\n");
  strcat(text,"firmware version : [0, 1, 5]\n\n");
  
  strcat(text, "data points:\n");
  strcat(text,"url:/p/o_1_1 rt:urn:knx:dpa.421.61 if:if.s inst:1 name:OnOff_1\n");
  strcat(text,"url:/p/o_2_2 rt:urn:knx:dpa.421.51 if:if.a inst:1 name:InfoOnOff_1\n");
  strcat(text,"url:/p/o_3_3 rt:urn:knx:dpa.421.61 if:if.s inst:2 name:OnOff_2\n");
  strcat(text,"url:/p/o_4_4 rt:urn:knx:dpa.421.51 if:if.a inst:2 name:InfoOnOff_2\n");
  strcat(text,"url:/p/o_5_5 rt:urn:knx:dpa.421.61 if:if.s inst:3 name:OnOff_3\n");
  strcat(text,"url:/p/o_6_6 rt:urn:knx:dpa.421.51 if:if.a inst:3 name:InfoOnOff_3\n");
  strcat(text,"url:/p/o_7_7 rt:urn:knx:dpa.421.61 if:if.s inst:4 name:OnOff_4\n");
  strcat(text,"url:/p/o_8_8 rt:urn:knx:dpa.421.51 if:if.a inst:4 name:InfoOnOff_4\n");
  strcat(text, "\n");
  
  strcat(text, "(c) Cascoda Ltd\n");
  strcat(text, "(c) KNX.org\n");
  strcat(text, "2023-04-17 14:49:14.324366");
  CustomDialog("About", text);
}

/**
 * @brief update the UI on the timer ticks
 * updates:
 * - check boxes
 * - info buttons
 * - text buttons
 * does a oc_main_poll to give a tick to the stack
 * takes into account if the device is sleepy
 * e.g. then it only does an poll each 20 seconds
 * @param event triggered by a timer
 */
void MyFrame::OnTimer(wxTimerEvent& event)
{
  bool do_poll = true;
  bool sleepy = m_menuOptions->IsChecked(CHECK_SLEEPY);
  // do whatever you want to do every millisecond here
  if (sleepy)
  {
    do_poll = false;
    m_sleep_counter++;
    if ((m_sleep_counter / 1000) > m_sleep_seconds) {
      // only do a poll each x (20) seconds
      do_poll = true;
      m_sleep_counter = 0;
    }
    if (oc_knx_device_in_programming_mode(0)) {
      // make sure that the device is reactive in programming mode
      // e.g. keep on polling
      do_poll = true;
    }
  }

  if (do_poll) {
    oc_clock_time_t next_event;
   next_event = oc_main_poll();
  }
  this->updateInfoCheckBoxes();
  this->updateInfoButtons(); 
  this->updateTextButtons();
}


/**
 * @brief update the UI e.g. check boxes in the UI
 * updates:
 * does a oc_main_poll to give a tick to the stack
 * 
 * @param event triggered by a timer
 */
void  MyFrame::updateInfoCheckBoxes()
{
  bool p;
  p = app_retrieve_bool_variable("/p/o_2_2"); // set toggle of InfoOnOff_1
  m_INFOONOFF_1->SetValue(p); 
  p = app_retrieve_bool_variable("/p/o_4_4"); // set toggle of InfoOnOff_2
  m_INFOONOFF_2->SetValue(p); 
  p = app_retrieve_bool_variable("/p/o_6_6"); // set toggle of InfoOnOff_3
  m_INFOONOFF_3->SetValue(p); 
  p = app_retrieve_bool_variable("/p/o_8_8"); // set toggle of InfoOnOff_4
  m_INFOONOFF_4->SetValue(p); 

}

/**
 * @brief convert the boolean to text for display
 * 
 * @param on_off the boolean
 * @param text the text to add the boolean as text
 */
void MyFrame::bool2text(bool on_off, char* text)
{
  if (on_off) {
    strcat(text, " On");
  }
  else {
    strcat(text, " Off");
  }
}

/**
 * @brief convert the integer to text for display
 * 
 * @param on_off the integer
 * @param text the text to add info too
 * @param as_ets the text as terminology as used in ets
 */
void MyFrame::int2text(int value, char* text, bool as_ets)
{
  char value_text[50];

  if (as_ets) {
    /*
    The so called Group Address structure correlates with its representation style in ETS,
    see also the relevant ETS Professional article.
    The information about the ETS Group Address representation style itself is NOT included in the Group Address.
    '3-level' = main/middle/sub
    main = D7+D6+D5+D4+D3 of the first octet (high address)
    middle = D2+D1+D0 of the first octet (high address)
    sub = the entire second octet (low address)
    ranges: main = 0..31, middle = 0..7, sub = 0..255
    */
    uint32_t ga = value;
    uint32_t ga_main = (ga >> 11);
    uint32_t ga_middle = (ga >> 8) & 0x7;
    uint32_t ga_sub = (ga & 0x000000FF);
    sprintf(value_text, " %d/%d/%d", ga_main, ga_middle, ga_sub);
    strcat(text, value_text);
  } else {
    sprintf(value_text, " %d", value);
    strcat(text, value_text);
  }
}

/**
 * @brief convert the scope to text for display
 * 
 * @param value the scope
 * @param text the text to add info too
 */
void MyFrame::int2scopetext(uint32_t value, char* text)
{
  char value_text[150];

  sprintf(value_text, " [%d]", value);
  strcat(text, value_text);
  // should be the same as 
  if (value & (1 << 1)) strcat(text, " if.i");
  if (value & (1 << 2)) strcat(text, " if.o");
  if (value & (1 << 3)) strcat(text, " if.g.s");
  if (value & (1 << 4)) strcat(text, " if.c");
  if (value & (1 << 5)) strcat(text, " if.p");
  if (value & (1 << 6)) strcat(text, " if.d");
  if (value & (1 << 7)) strcat(text, " if.a");
  if (value & (1 << 8)) strcat(text, " if.s");
  if (value & (1 << 9)) strcat(text, " if.ll");
  if (value & (1 << 10)) strcat(text, " if.b");
  if (value & (1 << 11)) strcat(text, " if.sec");
  if (value & (1 << 12)) strcat(text, " if.swu");
  if (value & (1 << 13)) strcat(text, " if.pm");
  if (value & (1 << 14)) strcat(text, " if.m");
}

/**
 * @brief convert the group id to text for display
 * 
 * @param value the group id
 * @param text the text to add info too
 * @param as_ets the text as terminology as used in ets
 */
void MyFrame::int2grpidtext(uint64_t value, char* text, bool as_ets)
{
  char value_text[50];

  if (as_ets) {
    /*
     create the multicast address from group and scope
     FF3_:FD__:____:____:(8-f)___:____
     FF35:30:<ULA-routing-prefix>::<group id>
        | 5 == scope
        | 3 == scope
     Multicast prefix: FF35:0030:  [4 bytes]
     ULA routing prefix: FD11:2222:3333::  [6 bytes + 2 empty bytes]
     Group Identifier: 8000 : 0068 [4 bytes ]
    */
    // group number to the various bytes
    uint8_t byte_1 = (uint8_t)value;
    uint8_t byte_2 = (uint8_t)(value >> 8);
    uint8_t byte_3 = (uint8_t)(value >> 16);
    uint8_t byte_4 = (uint8_t)(value >> 24);
    uint8_t byte_5 = (uint8_t)(value >> 32);

    if (byte_5 == 0) {
       sprintf(value_text, " %0x%0x:%0x%0x", byte_4, byte_3, byte_2, byte_1);
    }
    else {
      sprintf(value_text, " %0x:%0x%0x:%0x%0x", byte_5, byte_4, byte_3, byte_2, byte_1);
    }

    strcat(text, value_text);
  }
  else {
    //sprintf(text, "IID: %lld", device->iid);
    sprintf(value_text, " %lld", value);
    strcat(text, value_text);
  }
}

/**
 * @brief convert the double (e.g. float)  to text for display
 * 
 * @param value the vlue
 * @param text the text to add info too
 */
void MyFrame::double2text(double value, char* text)
{
  char new_text[200];
  sprintf(new_text," %f", value);
  strcat(text, new_text);
}

/**
 * @brief update the buttons
 * 
 */
void  MyFrame::updateInfoButtons()
{
  char text[200];
  bool p;
  int p_int;
  double d;
  p = app_retrieve_bool_variable("/p/o_2_2"); // set button text of InfoOnOff_1
  strcpy(text, "InfoOnOff_1 ('/p/o_2_2')");
  this->bool2text(p, text);
  m_INFOONOFF_1->SetLabel(text); 
  p = app_retrieve_bool_variable("/p/o_4_4"); // set button text of InfoOnOff_2
  strcpy(text, "InfoOnOff_2 ('/p/o_4_4')");
  this->bool2text(p, text);
  m_INFOONOFF_2->SetLabel(text); 
  p = app_retrieve_bool_variable("/p/o_6_6"); // set button text of InfoOnOff_3
  strcpy(text, "InfoOnOff_3 ('/p/o_6_6')");
  this->bool2text(p, text);
  m_INFOONOFF_3->SetLabel(text); 
  p = app_retrieve_bool_variable("/p/o_8_8"); // set button text of InfoOnOff_4
  strcpy(text, "InfoOnOff_4 ('/p/o_8_8')");
  this->bool2text(p, text);
  m_INFOONOFF_4->SetLabel(text); 

}
void MyFrame::OnPressed_OnOff_1(wxCommandEvent& event)
{
  char url[] = "/p/o_1_1";
  char my_text[100];
  bool p = app_retrieve_bool_variable(url);
  if (p == true) {
    p = false;
  }
  else {
    p = true;
  }
  app_set_bool_variable(url, p);
  oc_do_s_mode_with_scope(2, url, "w");
  oc_do_s_mode_with_scope(5, url, "w");
  sprintf(my_text, "OnOff_1 ('%s') pressed: %d", url, (int)p);
  SetStatusText(my_text);
} 
void MyFrame::OnPressed_OnOff_2(wxCommandEvent& event)
{
  char url[] = "/p/o_3_3";
  char my_text[100];
  bool p = app_retrieve_bool_variable(url);
  if (p == true) {
    p = false;
  }
  else {
    p = true;
  }
  app_set_bool_variable(url, p);
  oc_do_s_mode_with_scope(2, url, "w");
  oc_do_s_mode_with_scope(5, url, "w");
  sprintf(my_text, "OnOff_2 ('%s') pressed: %d", url, (int)p);
  SetStatusText(my_text);
} 
void MyFrame::OnPressed_OnOff_3(wxCommandEvent& event)
{
  char url[] = "/p/o_5_5";
  char my_text[100];
  bool p = app_retrieve_bool_variable(url);
  if (p == true) {
    p = false;
  }
  else {
    p = true;
  }
  app_set_bool_variable(url, p);
  oc_do_s_mode_with_scope(2, url, "w");
  oc_do_s_mode_with_scope(5, url, "w");
  sprintf(my_text, "OnOff_3 ('%s') pressed: %d", url, (int)p);
  SetStatusText(my_text);
} 
void MyFrame::OnPressed_OnOff_4(wxCommandEvent& event)
{
  char url[] = "/p/o_7_7";
  char my_text[100];
  bool p = app_retrieve_bool_variable(url);
  if (p == true) {
    p = false;
  }
  else {
    p = true;
  }
  app_set_bool_variable(url, p);
  oc_do_s_mode_with_scope(2, url, "w");
  oc_do_s_mode_with_scope(5, url, "w");
  sprintf(my_text, "OnOff_4 ('%s') pressed: %d", url, (int)p);
  SetStatusText(my_text);
} 



