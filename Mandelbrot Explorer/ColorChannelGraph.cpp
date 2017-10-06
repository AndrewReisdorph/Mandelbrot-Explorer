#include <wx/dcbuffer.h>
#include <wx/wx.h>
#include "ColorChannelGraph.h"
#include <math.h>

ColorChannelGraph::ColorChannelGraph(wxPanel *parent) :wxPanel(parent)
{
	m_Parent = parent;
	SetWindowStyle(wxBORDER_SUNKEN);
	SetMinSize(wxSize(-1, 75));
	this->Connect(wxEVT_PAINT, wxPaintEventHandler(ColorChannelGraph::OnPaint));
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_Green = { 1.0, 0.0, 2.0*M_PI };
	m_Blue = {1.0, 0.0, 2.0*M_PI};
	m_Red = { 1.0, 0.0, 2.0*M_PI };
}

void ColorChannelGraph::SetChannelSettings(Wave Red, Wave Green, Wave Blue)
{
	m_Red = Red;
	m_Green = Green;
	m_Blue = Blue;
	this->Refresh();
}

void ColorChannelGraph::OnPaint(wxPaintEvent& event)
{
	
	wxAutoBufferedPaintDC dc(this);
	wxBrush ClearBrush = wxBrush(wxColour(180, 180, 180, 255), wxBRUSHSTYLE_SOLID);
	dc.SetBrush(ClearBrush);
	dc.SetBackground(ClearBrush);
	dc.Clear();

	
	double x = 0.0;
	double x_delta = 0.01;
	int height = (this->GetSize().GetHeight()) * .90;
	int yoffset = 2;
	for (int i = 0; i < this->GetSize().GetWidth(); i++)
	{
		dc.SetPen(wxPen(wxColour(255, 0, 0),2));
		double RedAngle = m_Red.period * x + m_Red.phase;
		double RedValue = (height / 2.0) - (height / 2.0) * m_Red.amplitude * sin(RedAngle);
		double NextRedAngle = m_Red.period * (x + x_delta) + m_Red.phase;
		double NextRedValue = (height / 2.0) - (height / 2.0) * m_Red.amplitude * sin(NextRedAngle);
		dc.DrawLine(wxPoint(i, RedValue + yoffset), wxPoint(i + 1, NextRedValue+ yoffset));

		dc.SetPen(wxPen(wxColour(0, 255, 0),2));
		double GreenAngle = m_Green.period * x + m_Green.phase;
		double GreenValue = (height / 2.0) - (height/2.0) * m_Green.amplitude * sin(GreenAngle);
		double NextGreenAngle = m_Green.period * (x + x_delta) + m_Green.phase;
		double NextGreenValue = (height / 2.0) - (height / 2.0) * m_Green.amplitude * sin(NextGreenAngle);
		dc.DrawLine(wxPoint(i, GreenValue+ yoffset), wxPoint(i + 1, NextGreenValue+ yoffset));

		dc.SetPen(wxPen(wxColour(0, 0, 255),2));
		double BlueAngle = m_Blue.period * x + m_Blue.phase;
		double BlueValue = (height / 2.0) - (height / 2.0) * m_Blue.amplitude * sin(BlueAngle);
		double NextBlueAngle = m_Blue.period * (x + x_delta) + m_Blue.phase;
		double NextBlueValue = (height / 2.0) - (height / 2.0) * m_Blue.amplitude * sin(NextBlueAngle);
		dc.DrawLine(wxPoint(i, BlueValue+ yoffset), wxPoint(i + 1, NextBlueValue+ yoffset));

		x += x_delta;
	}

}