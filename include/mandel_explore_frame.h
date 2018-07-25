#// Copyright 2018 Andrew Reisdorph
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#ifndef MANDEL_EXPLORE_FRAME_H
#define MANDEL_EXPLORE_FRAME_H

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

#endif