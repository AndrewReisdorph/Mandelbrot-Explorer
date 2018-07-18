#include <wx/dcbuffer.h>

#include "fractal_render_panel.h"

using namespace std;

FractalRenderPanel::FractalRenderPanel(wxWindow * parent) :wxScrolledWindow(parent)
{
	SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void FractalRenderPanel::OnPaint(wxPaintEvent & event)
{
	wxAutoBufferedPaintDC dc(this);
	dc.SetBrush(wxBrush(wxColour(0, 0, 0, 0), wxBRUSHSTYLE_SOLID));
	dc.Clear();

	// Draw background color
	wxSize panelSize = GetSize();
	dc.SetBrush(wxBrush(wxColour(29, 30, 30, 255), wxBRUSHSTYLE_SOLID));
	dc.DrawRectangle(wxRect(0, 0, panelSize.GetWidth(), panelSize.GetHeight()));
}

void FractalRenderPanel::OnResize(wxSizeEvent & event)
{
	Refresh();
}

wxBEGIN_EVENT_TABLE(FractalRenderPanel, wxScrolledWindow)
EVT_PAINT(FractalRenderPanel::OnPaint)
EVT_SIZE(FractalRenderPanel::OnResize)
wxEND_EVENT_TABLE()