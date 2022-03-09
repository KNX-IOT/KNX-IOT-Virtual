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
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnPressed1(wxCommandEvent& event);
    void OnPressed2(wxCommandEvent& event);
    void OnPressed3(wxCommandEvent& event);
    void OnPressed4(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event);
    wxTimer m_timer;
};
enum
{
    ID_Hello = 1
};
wxIMPLEMENT_APP(MyApp);
bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame();
    frame->Show(true);
    return true;
}
MyFrame::MyFrame()
    : wxFrame(NULL, wxID_ANY, "KNX virtual Push Button"),
    m_timer(this, TIMER_ID)
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    //menuBar->Append(menuHelp, "&Help");
    SetMenuBar( menuBar );
    CreateStatusBar();
    SetStatusText("Welcome to KNX Virtual!");
    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
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

    m_timer.Start(1);    // 1 mili interval

}
void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}
void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
                 "About Hello World", wxOK | wxICON_INFORMATION);
}
void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}
void MyFrame::OnPressed1(wxCommandEvent& event)
{
  oc_do_s_mode_with_scope(2, "/p/1", "t");
  oc_do_s_mode_with_scope(5, "/p/1", "t");

  wxLogMessage("on pressed 1!");
}

void MyFrame::OnPressed2(wxCommandEvent& event)
{

  oc_do_s_mode_with_scope(2, "/p/3", "t");
  oc_do_s_mode_with_scope(5, "/p/3", "t");

  wxLogMessage("on pressed 2!");
}

void MyFrame::OnPressed3(wxCommandEvent& event)
{

  wxLogMessage("on pressed 3!");
}

void MyFrame::OnPressed4(wxCommandEvent& event)
{
  wxLogMessage("on pressed 4!");
}

void MyFrame::OnTimer(wxTimerEvent& event)
{
  // do whatever you want to do every second here
  //print("sss");
  oc_clock_time_t next_event;
  next_event = oc_main_poll();
}