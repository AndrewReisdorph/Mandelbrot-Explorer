#include "mandel_explore_frame.h"

MandelExploreFrame::MandelExploreFrame(const wxString& title) : wxFrame(0, -1, title)
{
	wxMenu *fileMenu = new wxMenu;
	fileMenu->Append(ID_MENU_QUIT, _T("E&xit\tAlt-X"), _T("Quit this program"));

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(fileMenu, _T("&File"));

	SetMenuBar(menuBar);

	CreateStatusBar(1);

	wxBoxSizer* mainVBoxSizer = new wxBoxSizer(wxVERTICAL);

	MandelPropertiesPanel* proppanl = new MandelPropertiesPanel(this);

	mainVBoxSizer->Add(proppanl, wxSizerFlags().Expand().Proportion(1));
	proppanl->SetSize(wxSize(200, -1));

	SetSizer(mainVBoxSizer);
	mainVBoxSizer->Fit(this);
	Centre();
}

void MandelExploreFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	wxExit();
}

wxBEGIN_EVENT_TABLE(MandelExploreFrame, wxFrame)
EVT_MENU(ID_MENU_QUIT, MandelExploreFrame::OnQuit)
wxEND_EVENT_TABLE()