#pragma once
#include <wx/wx.h>
//#include "MandelControlPanel.h"

class MandelControlPanel;

class ColorPreviewPanel :public wxPanel
{
private:
	void OnPaint(wxPaintEvent& event);
	MandelControlPanel *m_MandelControlPanel;

public:
	ColorPreviewPanel(MandelControlPanel *parent);
	~ColorPreviewPanel();
};