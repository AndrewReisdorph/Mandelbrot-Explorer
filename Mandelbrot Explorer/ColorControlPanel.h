#pragma once
#include <wx/wx.h>
#include "MandelbrotExplorerWindow.h"
#include "ColorChannelGraph.h"
#include "ColorChannelCtrl.h"
#include "ColorDisplayPanel.h"

#define NUM_COLOR_MODES 2

class MandelbrotExplorerWindow;
class ColorChannelCtrl;
class ColorDisplayPanel;

class ColorControlPanel : public wxPanel
{
private:
	wxString m_ColorModeOptions[NUM_COLOR_MODES] = { "Waves", "Dropper" };


	MandelbrotExplorerWindow *m_parent;
	ColorChannelGraph *m_ChannelGraph;
	ColorDisplayPanel *m_ColorDisplay;
	ColorChannelCtrl *m_RedChannelCtrl;
	ColorChannelCtrl *m_GreenChannelCtrl;
	ColorChannelCtrl *m_BlueChannelCtrl;

	void InitializeUI();

public:
	ColorControlPanel(MandelbrotExplorerWindow *parent);
	void UpdateChannelSettings();
};