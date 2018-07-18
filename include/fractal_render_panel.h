#pragma once

#include <wx/wx.h>
#include <wx/dcbuffer.h>

class FractalRenderPanel : public wxScrolledWindow
{
private:
	void OnPaint(wxPaintEvent& event);
	void OnResize(wxSizeEvent& event);

public:
	FractalRenderPanel(wxWindow *parent);

	wxDECLARE_EVENT_TABLE();
};