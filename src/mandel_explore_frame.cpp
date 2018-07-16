#include "mandel_explore_frame.h"

MandelExploreFrame::MandelExploreFrame(const wxString& title) : wxFrame(0, -1, title)
{
	wxMenu *file_menu = new wxMenu;
	file_menu->Append(ID_MENU_QUIT, _T("E&xit\tAlt-X"), _T("Quit this program"));

	wxMenuBar *menu_bar = new wxMenuBar;
	menu_bar->Append(file_menu, _T("&File"));

	SetMenuBar(menu_bar);

	CreateStatusBar(1);

	wxBoxSizer* main_v_box_sizer = new wxBoxSizer(wxVERTICAL);
	MandelPropertiesPanel* mandel_properties_panel = new MandelPropertiesPanel(this);

	main_v_box_sizer->Add(mandel_properties_panel, wxSizerFlags().Expand().Proportion(1));


	SetSizer(main_v_box_sizer);
	main_v_box_sizer->Fit(this);
	Centre();
}

void MandelExploreFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	wxExit();
}

wxBEGIN_EVENT_TABLE(MandelExploreFrame, wxFrame)
EVT_MENU(ID_MENU_QUIT, MandelExploreFrame::OnQuit)
wxEND_EVENT_TABLE()