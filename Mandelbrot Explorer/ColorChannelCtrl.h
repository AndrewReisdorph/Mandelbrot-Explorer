#pragma once
#include <wx/wx.h>
#include "ColorControlPanel.h"
#include "main.h"

class ColorControlPanel;

class ColorChannelCtrl :public wxPanel
{
private:
	void InitializeUI();
	void UpdateSettings();

	wxString m_ColorName;
	wxSlider *m_amplitudeSlider;
	wxSlider *m_periodSlider;
	wxSlider *m_phaseSlider;
	wxTextCtrl *m_amplitudeValueCtrl;
	wxTextCtrl *m_periodValueCtrl;
	wxTextCtrl *m_phaseValueCtrl;
	ColorControlPanel *m_ColorControlPanel;
	double m_period;
	double m_amplitude;
	double m_phase;

public:
	ColorChannelCtrl(ColorControlPanel *parent, wxString ColorName);
	void OnSliderChange(wxCommandEvent &event);
	Wave GetWave();
};