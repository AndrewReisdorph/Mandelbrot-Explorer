#include "MandelShitWindow.h"
#include "MandelControlPanel.h"
#include "MandelView.h"
#include "ColorControlPanel.h"
#include "main.h"

MandelShitWindow::MandelShitWindow(const wxString& title) : wxFrame(NULL, wxID_ANY, title)
{
	MainVSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *HSizer = new wxBoxSizer(wxHORIZONTAL);

	CreateStatusBar(1);
	m_ControlPanel = new MandelControlPanel(this, wxID_ANY);
	m_MandelView = new MandelViewPanel(this, m_ControlPanel);
	m_ColorCtrl = new ColorControlPanel(this);
	m_ControlPanel->SetMandelView(m_MandelView);
	//MainVSizer->Add(m_ControlPanel, wxSizerFlags().Expand());
	//MainVSizer->Add(m_ColorCtrl);
	//VSizer->Add(m_MandelView, wxSizerFlags().Expand().Proportion(1));
	//VSizer->Add(m_ColorCtrl, wxSizerFlags().Expand());
	//MainVSizer->Add(VSizer, wxSizerFlags().Expand().Proportion(1));
	HSizer->Add(m_ControlPanel, wxSizerFlags().Expand());
	HSizer->Add(m_MandelView, wxSizerFlags().Expand().Proportion(1));
	MainVSizer->Add(HSizer, wxSizerFlags().Expand().Proportion(1));
	MainVSizer->Add(m_ColorCtrl, wxSizerFlags().Expand());

	m_MenuBar = new wxMenuBar;
	FileMenu = new wxMenu;
	FileMenu->Append(ID_SaveImage, wxT("&Save Image"));
	FileMenu->Append(ID_RenderVideo, wxT("&Video Poopoo"));
	FileMenu->Append(ID_Quit, wxT("&Quit"));
	m_MenuBar->Append(FileMenu, wxT("&File"));
	SetMenuBar(m_MenuBar);
	
	wxInitAllImageHandlers();

	SetSizer(MainVSizer);
	MainVSizer->Fit(this);
	Centre();
	Layout();


	m_ColorCtrl->UpdateChannelSettings();

}

void MandelShitWindow::OnExit(wxCommandEvent& event)
{
	Close(TRUE);
}

void MandelShitWindow::Render(wxCommandEvent& event)
{
	m_MandelView->Render();
}

void MandelShitWindow::SetStatusBarText(wxString StatusText)
{
	SetStatusText(StatusText, 0);
}

void MandelShitWindow::DoVideo(wxCommandEvent& event)
{
	m_MandelView->RenderVideo();
}

BEGIN_EVENT_TABLE(MandelShitWindow, wxFrame)
//EVT_BUTTON(BUTTON_Render, MandelShitWindow::Render)
EVT_MENU(ID_Quit, MandelShitWindow::OnExit)
//EVT_MENU(ID_SaveImage, MandelShitWindow::SaveImage)
EVT_MENU(ID_RenderVideo, MandelShitWindow::DoVideo)
END_EVENT_TABLE()