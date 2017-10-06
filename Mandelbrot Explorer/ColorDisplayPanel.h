#pragma once
#include <wx/wx.h>
#include "main.h"

class ColorControlPanel;

class ColorDisplayPanel :public wxPanel
{
private:
	Wave m_Red;
	Wave m_Green;
	Wave m_Blue;
	void OnPaint(wxPaintEvent& event);
	ColorControlPanel *m_ColorControlPanel;
	//void InitializeUI();

public:
	ColorDisplayPanel(ColorControlPanel *parent);
	void SetChannelSettings(Wave Red, Wave Green, Wave Blue);
};