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
// 2022-06-17 16:39:31.028988

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

enum
{
  RESET = wxID_HIGHEST + 1, // ID for reset button in the menu
  IA_TEXT = RESET + 1,       // ID for internal address text 
  IID_TEXT = IA_TEXT + 1,    // ID for installation id text 
  PM_TEXT = IID_TEXT + 1,    // ID for programming mode text 
  LS_TEXT = PM_TEXT + 1,     // ID for load status text 
  HOSTNAME_TEXT = LS_TEXT + 1, // ID for hostname text 
  GOT_TABLE_ID = HOSTNAME_TEXT + 1, // ID for the Group object window
  PUB_TABLE_ID = GOT_TABLE_ID + 1, // ID for the publisher table window
  REC_TABLE_ID = PUB_TABLE_ID + 1, // ID for the recipient table window
  PARAMETER_LIST_ID = REC_TABLE_ID + 1, // ID for the parameter window
  AT_TABLE_ID = PARAMETER_LIST_ID + 1, // ID for the auth/at window
  CHECK_PM = AT_TABLE_ID + 1 , // programming mode check in menu bar
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
  void OnReset(wxCommandEvent& event);

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

  wxMenu* m_menuFile;
  wxTimer m_timer;

  wxTextCtrl* m_ia_text;   // text control for internal address
  wxTextCtrl* m_iid_text; // text control for installation id
  wxTextCtrl* m_pm_text; // text control for programming mode
  wxTextCtrl* m_ls_text; // text control for load state
  wxTextCtrl* m_hostname_text; // text control for host name
  wxTextCtrl* m_secured_text; // text secure/not secure
  //DP_ID_ONOFF_1
  wxButton* m_ONOFF_1; // OnOff_1 if.s 
  //DP_ID_INFOONOFF_1
  wxCheckBox* m_INFOONOFF_1 ; // InfoOnOff_1 if.a 
  //DP_ID_ONOFF_2
  wxButton* m_ONOFF_2; // OnOff_2 if.s 
  //DP_ID_INFOONOFF_2
  wxCheckBox* m_INFOONOFF_2 ; // InfoOnOff_2 if.a 
  //DP_ID_ONOFF_3
  wxButton* m_ONOFF_3; // OnOff_3 if.s 
  //DP_ID_INFOONOFF_3
  wxCheckBox* m_INFOONOFF_3 ; // InfoOnOff_3 if.a 
  //DP_ID_ONOFF_4
  wxButton* m_ONOFF_4; // OnOff_4 if.s 
  //DP_ID_INFOONOFF_4
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
  SetStatusText("Welcome to KNX virtual Push Button!");
  Bind(wxEVT_MENU, &MyFrame::OnReset, this, RESET);
  Bind(wxEVT_MENU, &MyFrame::OnGroupObjectTable, this, GOT_TABLE_ID);
  Bind(wxEVT_MENU, &MyFrame::OnPublisherTable, this, PUB_TABLE_ID);
  Bind(wxEVT_MENU, &MyFrame::OnRecipientTable, this, REC_TABLE_ID);
  Bind(wxEVT_MENU, &MyFrame::OnParameterList, this, PARAMETER_LIST_ID);
  Bind(wxEVT_MENU, &MyFrame::OnAuthTable, this, AT_TABLE_ID);
  Bind(wxEVT_MENU, &MyFrame::OnProgrammingMode, this, CHECK_PM);
  Bind(wxEVT_MENU, &MyFrame::OnReset, this, RESET);
  Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);

  int x_width = 210;
  int x_height = 25;
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

  // serial number
  char text[500];
  strcpy(text, "Device Serial Number: -sn ");
  oc_device_info_t* device = oc_core_get_device_info(0);
  strcat(text, oc_string(device->serialnumber));
  wxTextCtrl* Statictext;
  Statictext = new wxTextCtrl(this, wxID_ANY, text, wxPoint(10, 10 + ((max_instances + 1) * x_height)), wxSize(150 + 130, x_height), 0);
  Statictext->SetEditable(false);

  // internal address
  sprintf(text, "IA: %d", device->ia);
  m_ia_text = new wxTextCtrl(this, IA_TEXT, text, wxPoint(10, 10 + ((max_instances + 2) * x_height)), wxSize(140, x_height), 0);
  m_ia_text->SetEditable(false);
  // installation id
  sprintf(text, "IID: %d", device->iid);
  m_iid_text = new wxTextCtrl(this, IID_TEXT, text, wxPoint(10 + 140, 10 + ((max_instances + 2) * x_height)), wxSize(140, x_height), 0);
  m_iid_text->SetEditable(false);
  // programming mode
  sprintf(text, "Programming Mode: %d", device->pm);
  m_pm_text = new wxTextCtrl(this, PM_TEXT, text, wxPoint(10, 10 + ((max_instances + 3) * x_height)), wxSize(140, x_height), 0);
  m_pm_text->SetEditable(false);
  // installation id
  sprintf(text, "LoadState: %s", oc_core_get_lsm_state_as_string(device->lsm_s));
  m_ls_text = new wxTextCtrl(this, LS_TEXT, text, wxPoint(10 + 140, 10 + ((max_instances + 3) * 25)), wxSize(140, 25), 0);
  m_ls_text->SetEditable(false);
  // host name
  sprintf(text, "host name: %s", oc_string(device->hostname));
  m_hostname_text = new wxTextCtrl(this, LS_TEXT, text, wxPoint(10, 10 + ((max_instances + 4) * 25)), wxSize(140, 25), 0);
  m_hostname_text->SetEditable(false);
  if (app_is_secure()) {
    strcpy(text, "secured");
  }
  else {
    strcpy(text, "unsecured");
  }
  m_secured_text = new wxTextCtrl(this, LS_TEXT, text, wxPoint(10 + 140,  10 + ((max_instances + 4) * 25)), wxSize(140, 25), wxTE_RICH);
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

  // get the device data structure
  oc_device_info_t* device = oc_core_get_device_info(device_index);
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
  char line2[200];
  char windowtext[200];

  strcpy(text, "");
  oc_device_info_t* device = oc_core_get_device_info(device_index);
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
      sprintf(line, "  cflags : '%d' ", (int)entry->cflags);
      oc_cflags_as_string(line, entry->cflags);
      strcat(text, line);
      strcpy(line,"  ga : [");
      for (int i = 0; i < entry->ga_len; i++) {
        sprintf(line2," %d", entry->ga[i]);
        strcat(line, line2);
      }
      strcat(line," ]\n");
      strcat(text, line);
    }
  }
  strcpy(windowtext, "Group Object Table  ");
  strcat(windowtext, oc_string(device->serialnumber));
  wxMessageBox(text, windowtext,
    wxOK | wxICON_NONE);
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
  char line2[200];
  char windowtext[200];

  strcpy(text, "");
  oc_device_info_t* device = oc_core_get_device_info(device_index);
  int total =  oc_core_get_publisher_table_size();
  for (int index = 0; index < total; index++) {
    oc_group_rp_table_t* entry = oc_core_get_publisher_table_entry(index);

    if (entry->id >= 0) {
      sprintf(line, "Index %d \n", index);
      strcat(text, line);
      sprintf(line, "  id: '%d'  ", entry->id);
      strcat(text, line);
       if ( entry->ia >= 0) {
        sprintf(line, "  ia: '%d' ", entry->ia);
        strcat(text, line);
      }
      if ( entry->iid >= 0) {
        sprintf(line, "  iid: '%d' ", entry->iid);
        strcat(text, line);
      }
      if ( entry->fid >= 0) {
        sprintf(line, "  fid: '%d' ", entry->fid);
        strcat(text, line);
      }
      if ( entry->grpid >= 0) {
        sprintf(line, "  grpid: '%d' ", entry->grpid);
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
          sprintf(line2," %d", entry->ga[i]);
          strcat(line, line2);
        }
        strcat(line," ]\n");
        strcat(text, line);
      }
    }
  }
  strcpy(windowtext, "Publisher Table  ");
  strcat(windowtext, oc_string(device->serialnumber));
  wxMessageBox(text, windowtext,
    wxOK | wxICON_NONE);
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
  char line2[200];
  char windowtext[200];

  strcpy(text, "");
  oc_device_info_t* device = oc_core_get_device_info(device_index);
  int total =  oc_core_get_recipient_table_size();
  for (int index = 0; index < total; index++) {
    oc_group_rp_table_t* entry = oc_core_get_recipient_table_entry(index);

    if (entry->id >= 0) {
      sprintf(line, "Index %d \n", index);
      strcat(text, line);
      sprintf(line, "  id: '%d'  ", entry->id);
      strcat(text, line);
      if ( entry->ia >= 0) {
        sprintf(line, "  ia: '%d' ", entry->ia);
        strcat(text, line);
      }
      if ( entry->iid >= 0) {
        sprintf(line, "  iid: '%d' ", entry->iid);
        strcat(text, line);
      }
      if ( entry->fid >= 0) {
        sprintf(line, "  fid: '%d' ", entry->fid);
        strcat(text, line);
      }
      if ( entry->grpid >= 0) {
        sprintf(line, "  grpid: '%d' ", entry->grpid);
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
          sprintf(line2," %d", entry->ga[i]);
          strcat(line, line2);
        }
        strcat(line," ]\n");
        strcat(text, line);
      }
    }
  }
  strcpy(windowtext, "Recipient Table  ");
  strcat(windowtext, oc_string(device->serialnumber));
  wxMessageBox(text, windowtext,
    wxOK | wxICON_NONE);
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
  wxMessageBox(text, windowtext,
    wxOK | wxICON_NONE);
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
  char text[1024 * 5];
  char line[200];
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
              sprintf(line, " %d", my_entry->ga[i]);
              strcat(text, line);
            }
            sprintf(line, " ]\n");
            strcat(text, line);
          } else {
             sprintf(line, "  scope : %d ", my_entry->scope);
             strcat(text, line);
          }
        }
      }
    }
  }

  strcpy(windowtext, "Auth AT Table ");
  strcat(windowtext, oc_string(device->serialnumber));
  wxMessageBox(text, windowtext,
    wxOK | wxICON_NONE);
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
  strcat(text,"model            : KNX virtual - PB\n");
  strcat(text,"hardware type    : Linux/windows\n");
  strcat(text,"hardware version : [0, 1, 2]\n");
  strcat(text,"firmware version : [3, 4, 5]\n\n");
  
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
  strcat(text, "2022-06-17 16:39:31.028988");
  wxMessageBox(text, "KNX virtual Push Button",
    wxOK | wxICON_NONE);
}

/**
 * @brief update the UI on the timer ticks
 * updates:
 * - checkboxes
 * - info buttons
 * - text buttons
 * does a oc_main_poll to give a tick to the stack
 * 
 * @param event triggered by a timer
 */
void MyFrame::OnTimer(wxTimerEvent& event)
{
  // do whatever you want to do every millisecond here
  oc_clock_time_t next_event;
  next_event = oc_main_poll();
  this->updateInfoCheckBoxes();
  this->updateInfoButtons(); 
  this->updateTextButtons();
}

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

void MyFrame::bool2text(bool on_off, char* text)
{
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
  bool p;
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

  //bool p = app_retrieve_bool_variable("/p/1");
  //strcpy(text, "Actuator 1 ('/p/1')");
  //this->bool2text(p, text);
  //m_btn_1->SetLabel(text);
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


