#pragma once

#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/event.h>
#include <wx/app.h>

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

