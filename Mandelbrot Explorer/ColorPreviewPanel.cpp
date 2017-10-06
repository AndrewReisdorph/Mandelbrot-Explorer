#include <math.h>
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "ColorPreviewPanel.h"
//#include "main.h"
#include "MandelControlPanel.h"

ColorPreviewPanel::ColorPreviewPanel(MandelControlPanel *parent): wxPanel(parent)
{
	m_MandelControlPanel = parent;
	SetMinSize(wxSize(-1,50));
	SetWindowStyle(wxBORDER_SUNKEN);
	this->Connect(wxEVT_PAINT, wxPaintEventHandler(ColorPreviewPanel::OnPaint));
	SetBackgroundStyle(wxBG_STYLE_PAINT);

}

void ColorPreviewPanel::OnPaint(wxPaintEvent& event)
{
	int width = GetSize().GetWidth();
	int height = GetSize().GetHeight();
	int red;
	int green;
	int blue;
	float gradient = 0.0;
	double TwoPi = M_PI * 2.0;
	double RGBFraction = (255.0 / 2.0);
	double GradationStep;
	double GradationsAngleStep;
	double Gradation = 0;
	double RedOffset;
	double GreenOffset;
	double BlueOffset;

	RenderSettings settings = m_MandelControlPanel->GetRenderSettings();
	wxAutoBufferedPaintDC dc(this);
	//dc.Clear();
	wxBrush ClearBrush = wxBrush(wxColour(0, 0, 0, 0), wxBRUSHSTYLE_TRANSPARENT);
	//wxPen RedPen = wxPen(wxColour(255,0,0));
	dc.SetBrush(ClearBrush);

	switch (settings.ColorMode)
	{
	case CONTINUOUS:
		TwoPi = M_PI * 2.0;
		RGBFraction = (255.0 / 2.0);
		GradationStep = settings.Gradations / double(width);
		GradationsAngleStep = TwoPi / double(settings.Gradations);
		Gradation = 0;
	    RedOffset = (settings.Red / 255.0) * TwoPi;
		GreenOffset = (settings.Green / 255.0) * TwoPi;
		BlueOffset = (settings.Blue / 255.0) * TwoPi;
		for (int Column = 0; Column < width; Column++)
		{
			double BaseAngle = Gradation * GradationsAngleStep;
			Gradation += GradationStep;

			red = int((sin(RedOffset + BaseAngle) + 1)*RGBFraction);
			green = int((sin(GreenOffset + BaseAngle) + 1)*RGBFraction);
			blue = int((sin(BlueOffset + BaseAngle) + 1)*RGBFraction);

			dc.SetPen(wxPen(wxColour(red, green, blue)));
			dc.DrawLine(Column, 0, Column, height);
		}
		break;
		
	case PROPORTIONAL:
		for (int Column = 0; Column < width; Column++)
		{
			gradient = Column / float(width);
			red = settings.Red * gradient;
			green = settings.Green * gradient;
			blue = settings.Blue * gradient;
			dc.SetPen(wxPen(wxColour(red, green, blue)));
			dc.DrawLine(Column, 0, Column, height);
		}
		break;
	default:

		break;

	}
}

ColorPreviewPanel::~ColorPreviewPanel()
{
	std::cout << "Color Preview Panel DEAD" << std::endl;
}