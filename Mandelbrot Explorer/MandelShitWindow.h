#pragma once
#include <wx/wx.h>
#include "MandelControlPanel.h"
#include "ColorControlPanel.h"
#include "MandelView.h"

class MandelViewPanel;
class ColorControlPanel;

class MandelShitWindow : public wxFrame
{
private:
	void Render(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	//void SaveImage(wxCommandEvent& event);
	void DoVideo(wxCommandEvent& event);

	MandelControlPanel *m_ControlPanel;
	MandelViewPanel *m_MandelView;
	ColorControlPanel *m_ColorCtrl;

	wxBoxSizer *MainVSizer;

	wxMenuBar *m_MenuBar;
	wxMenu *FileMenu;

public:
	void SetStatusBarText(wxString StatusText);
	MandelShitWindow(const wxString& title);
	
	DECLARE_EVENT_TABLE()
};

