#pragma once
#include <wx/wx.h>
#include "MandelShitWindow.h"
#include "ColorChannelGraph.h"
#include "ColorChannelCtrl.h"
#include "ColorDisplayPanel.h"

#define NUM_COLOR_MODES 2

class MandelShitWindow;
class ColorChannelCtrl;
class ColorDisplayPanel;

class ColorControlPanel : public wxPanel
{
private:
	wxString m_ColorModeOptions[NUM_COLOR_MODES] = { "Waves", "Dropper" };


	MandelShitWindow *m_parent;
	ColorChannelGraph *m_ChannelGraph;
	ColorDisplayPanel *m_ColorDisplay;
	ColorChannelCtrl *m_RedChannelCtrl;
	ColorChannelCtrl *m_GreenChannelCtrl;
	ColorChannelCtrl *m_BlueChannelCtrl;

	void InitializeUI();

public:
	ColorControlPanel(MandelShitWindow *parent);
	void UpdateChannelSettings();
};