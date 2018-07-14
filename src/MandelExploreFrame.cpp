#include "MandelExploreFrame.h"

MandelExploreFrame::MandelExploreFrame(const wxString& title) : wxFrame(0, -1, title)
{
	wxMenu *fileMenu = new wxMenu;
	fileMenu->Append(ID_MENU_QUIT, _T("E&xit\tAlt-X"), _T("Quit this program"));

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(fileMenu, _T("&File"));

	SetMenuBar(menuBar);
}

void MandelExploreFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	wxExit();
}

wxBEGIN_EVENT_TABLE(MandelExploreFrame, wxFrame)
EVT_MENU(ID_MENU_QUIT, MandelExploreFrame::OnQuit)
wxEND_EVENT_TABLE()