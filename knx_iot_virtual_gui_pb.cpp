// wxWidgets "Hello World" Program
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
  TIMER_ID = BUTTON_4 + 1
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
    void OnPressed1(wxCommandEvent& event);
    void OnPressed2(wxCommandEvent& event);
    void OnPressed3(wxCommandEvent& event);
    void OnPressed4(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event);
    wxTimer m_timer;
};

wxIMPLEMENT_APP(MyApp);
bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame();
    frame->Show(true);
    return true;
}
MyFrame::MyFrame()
    : wxFrame(NULL, wxID_ANY, "KNX virtual Push Button")
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

    wxButton* btn_1 = new wxButton(this, BUTTON_1, _T("Button 1"), wxPoint(10, 10 ), wxSize(80, 25), 0);
    btn_1->Bind(wxEVT_BUTTON, &MyFrame::OnPressed1, this);

    wxButton* btn_2 = new wxButton(this, BUTTON_2, _T("Button 2"), wxPoint(10, 10 + 25), wxSize(80, 25), 0);
    btn_2->Bind(wxEVT_BUTTON, &MyFrame::OnPressed2, this);

    wxButton* btn_3 = new wxButton(this, BUTTON_3, _T("Button 3"), wxPoint(10, 10 + 50), wxSize(80, 25), 0);
    btn_3->Bind(wxEVT_BUTTON, &MyFrame::OnPressed3, this);

    wxButton* btn_4 = new wxButton(this, BUTTON_4, _T("Button 4"), wxPoint(10, 10 + 75), wxSize(80, 25), 0);
    btn_4->Bind(wxEVT_BUTTON, &MyFrame::OnPressed4, this);

    app_initialize_stack();

    //m_timer.Bind(wxEVT_TIMER, &MyFrame::OnTimer, this);
    //m_timer.Start(1);    // 1 mili interval
    // https://forums.wxwidgets.org/viewtopic.php?t=41836
    //m_timer.Connect(m_timer.GetId(), wxEVT_TIMER, wxTimerEventHandler(MyFrame::OnTimer), NULL, this);
    m_timer.Bind(wxEVT_TIMER, &MyFrame::OnTimer, this);
    m_timer.Start(1, wxTIMER_CONTINUOUS);

}
void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}
void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is tool is KNX-IOT virtual for push button\n (c) Cascoda Ltd\n (c) KNX.org",
                 "KNX-IOT virtual Push Button",
      wxOK | wxICON_INFORMATION);
}

void MyFrame::OnPressed1(wxCommandEvent& event)
{
  oc_do_s_mode_with_scope(2, "/p/1", "t");
  oc_do_s_mode_with_scope(5, "/p/1", "t");

  //wxLogMessage("on pressed 1!");
  SetStatusText("Button 1 pressed");
}

void MyFrame::OnPressed2(wxCommandEvent& event)
{

  oc_do_s_mode_with_scope(2, "/p/3", "t");
  oc_do_s_mode_with_scope(5, "/p/3", "t");

  //wxLogMessage("on pressed 2!");
  SetStatusText("Button 2 pressed");
}

void MyFrame::OnPressed3(wxCommandEvent& event)
{

  oc_do_s_mode_with_scope(2, "/p/5", "t");
  oc_do_s_mode_with_scope(5, "/p/5", "t");
  //wxLogMessage("on pressed 3!");
  SetStatusText("Button 3 pressed");
}

void MyFrame::OnPressed4(wxCommandEvent& event)
{

  oc_do_s_mode_with_scope(2, "/p/7", "t");
  oc_do_s_mode_with_scope(5, "/p/7", "t");
  //wxLogMessage("on pressed 4!");
  SetStatusText("Button 4 pressed");
}

void MyFrame::OnTimer(wxTimerEvent& event)
{
  // do whatever you want to do every mili second here
  oc_clock_time_t next_event;
  next_event = oc_main_poll();
  //SetStatusText(".");
}