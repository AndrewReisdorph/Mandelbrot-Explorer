#pragma once

#include "mandel_properties_panel.h"

#include <wx/wx.h>


enum
{
	ID_MENU_QUIT = 1
};

class MandelExploreFrame : public wxFrame
{

public:
	MandelExploreFrame(const wxString& title);

private:
	void OnQuit(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
};

