#pragma once
#include <wx/wx.h>
#include "main.h"

class ColorChannelGraph :public wxPanel
{
private:
	wxPanel *m_Parent;
	Wave m_Red;
	Wave m_Green;
	Wave m_Blue;

public:
	ColorChannelGraph(wxPanel *parent);
	void OnPaint(wxPaintEvent& event);
	void SetChannelSettings(Wave Red, Wave Green, Wave Blue);
};