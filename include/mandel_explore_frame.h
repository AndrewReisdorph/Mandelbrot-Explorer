#pragma once

#include "mandel_properties_panel.h"
#include "fractal_render_panel.h"
#include "palette_panel.h"

#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/gauge.h>

enum
{
	ID_MENU_QUIT = 1,

	ID_TOOLBAR_OPEN,
	ID_TOOLBAR_SAVE,
	ID_TOOLBAR_RENDER,
	ID_TOOLBAR_CAPTURE,
	ID_TOOLBAR_PALETTE,
	ID_TOOLBAR_PROPERTIES,
	ID_TOOLBAR_VIDEO,
	ID_TOOLBAR_PAUSE_RESUME,
	ID_TOOLBAR_CANCEL,
	ID_TOOLBAR_SELECT,
	ID_TOOLBAR_ZOOM,
	ID_TOOLBAR_HOME,
};

class MandelExploreFrame : public wxFrame
{

public:
	MandelExploreFrame(const wxString& title);

private:
	wxBitmap resume_bitmap_;
	wxBitmap pause_bitmap_;
	wxBitmap cancel_bitmap_;
	wxToolBar *tool_bar_;
	MandelPropertiesPanel* mandel_properties_panel_;
	PalettePanel* palette_panel_;
	wxSplitterWindow* main_h_splitter_;
	wxGauge* render_progress_gauge_;

	void OnOpenConfigurationButton(wxCommandEvent& event);
	void OnSaveConfigurationButton(wxCommandEvent& event);
	void OnRenderButton(wxCommandEvent& event);
	void OnCaptureButton(wxCommandEvent& event);
	void OnPaletteButton(wxCommandEvent& event);
	void OnVideoButton(wxCommandEvent& event);
	void OnPauseResumeButton(wxCommandEvent& event);
	void OnCancelButton(wxCommandEvent& event);
	void OnSelectButton(wxCommandEvent& event);
	void OnZoomButton(wxCommandEvent& event);
	void OnHomeButton(wxCommandEvent& event);
	void OnPropertiesButton(wxCommandEvent& event);
	void OnQuit(wxCommandEvent& event);

	void OnToolBarMouseOver(wxCommandEvent& event);
	
	wxDECLARE_EVENT_TABLE();
};

