#include "ColorControlPanel.h"
#include "ColorDisplayPanel.h"
#include <wx/dcbuffer.h>
#include <wx/wx.h>

ColorDisplayPanel::ColorDisplayPanel(ColorControlPanel *parent) :wxPanel(parent)
{
	m_ColorControlPanel = parent;
	SetMinSize(wxSize(-1, 75));
	SetWindowStyle(wxBORDER_SUNKEN);
	this->Connect(wxEVT_PAINT, wxPaintEventHandler(ColorDisplayPanel::OnPaint));
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_Red = { 0 };
	m_Green = { 0 };
	m_Blue = { 0 };

	//InitializeUI();
}

void ColorDisplayPanel::OnPaint(wxPaintEvent& event)
{
	wxAutoBufferedPaintDC dc(this);
	wxBrush ClearBrush = wxBrush(wxColour(200, 200, 200, 255), wxBRUSHSTYLE_SOLID);
	dc.SetBrush(ClearBrush);
	dc.SetBackground(ClearBrush);
	dc.Clear();


	double x = 0.0;
	int height = (this->GetSize().GetHeight());

	for (int i = 0; i < this->GetSize().GetWidth(); i++)
	{
		double RedAngle = m_Red.period * x + m_Red.phase;
		double RedValue = 255-((255 / 2.0) - (255 / 2.0) * m_Red.amplitude * sin(RedAngle));

		double GreenAngle = m_Green.period * x + m_Green.phase;
		double GreenValue = 255 - ((255 / 2.0) - (255 / 2.0) * m_Green.amplitude * sin(GreenAngle));

		double BlueAngle = m_Blue.period * x + m_Blue.phase;
		double BlueValue = 255 - ((255 / 2.0) - (255 / 2.0) * m_Blue.amplitude * sin(BlueAngle));

		dc.SetPen(wxPen(wxColour(RedValue, GreenValue, BlueValue)));
		dc.DrawLine(wxPoint(i, 0), wxPoint(i,height));

		x += 0.01;
	}

}

void ColorDisplayPanel::SetChannelSettings(Wave Red, Wave Green, Wave Blue)
{
	m_Red = Red;
	m_Green = Green;
	m_Blue = Blue;
	Refresh();
}