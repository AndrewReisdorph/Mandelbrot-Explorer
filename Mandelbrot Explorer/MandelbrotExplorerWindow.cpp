#include "MandelbrotExplorerWindow.h"
#include "MandelControlPanel.h"
#include "MandelView.h"
#include "ColorControlPanel.h"
#include "main.h"

MandelbrotExplorerWindow::MandelbrotExplorerWindow(const wxString& title) : wxFrame(NULL, wxID_ANY, title)
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
	
	SetSizer(MainVSizer);
	MainVSizer->Fit(this);
	Centre();
	Layout();


	m_ColorCtrl->UpdateChannelSettings();

}

void MandelbrotExplorerWindow::OnExit(wxCommandEvent& event)
{
	Close(TRUE);
}

void MandelbrotExplorerWindow::Render(wxCommandEvent& event)
{
	m_MandelView->Render();
}

void MandelbrotExplorerWindow::SetStatusBarText(wxString StatusText)
{
	SetStatusText(StatusText, 0);
}

void MandelbrotExplorerWindow::DoVideo(wxCommandEvent& event)
{
	m_MandelView->RenderVideo();
}

BEGIN_EVENT_TABLE(MandelbrotExplorerWindow, wxFrame)
EVT_MENU(ID_Quit, MandelbrotExplorerWindow::OnExit)
EVT_MENU(ID_RenderVideo, MandelbrotExplorerWindow::DoVideo)
END_EVENT_TABLE()