#include <math.h>
#include "ColorChannelCtrl.h"

using namespace std;

ColorChannelCtrl::ColorChannelCtrl(ColorControlPanel *parent, wxString ColorName) :wxPanel(parent)
{
	m_ColorControlPanel = parent;
	m_ColorName = ColorName;
	SetWindowStyle(wxBORDER_SUNKEN);
	InitializeUI();
}

void ColorChannelCtrl::InitializeUI()
{
	wxBoxSizer *mainHSizer = new wxBoxSizer(wxHORIZONTAL);

	wxFlexGridSizer *sliderPanelGridSizer = new wxFlexGridSizer(3, 3, 5);
	sliderPanelGridSizer->AddGrowableCol(2,1);
	
	wxStaticText *amplitudeText = new wxStaticText(this, wxID_ANY, "Amplitude:");
	m_amplitudeValueCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(50,-1));
	m_amplitudeSlider = new wxSlider(this, wxID_ANY, 1000, 0, 1000);
	m_amplitudeSlider->Bind(wxEVT_SLIDER, &ColorChannelCtrl::OnSliderChange, this);
	sliderPanelGridSizer->Add(amplitudeText);
	sliderPanelGridSizer->Add(m_amplitudeValueCtrl);
	sliderPanelGridSizer->Add(m_amplitudeSlider,wxSizerFlags().Expand().Proportion(1));

	wxStaticText *periodText = new wxStaticText(this, wxID_ANY, "Period:");
	m_periodValueCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(50, -1));
	m_periodSlider = new wxSlider(this, wxID_ANY, 628, 0, 1000);
	m_periodSlider->Bind(wxEVT_SLIDER, &ColorChannelCtrl::OnSliderChange, this);
	sliderPanelGridSizer->Add(periodText);
	sliderPanelGridSizer->Add(m_periodValueCtrl);
	sliderPanelGridSizer->Add(m_periodSlider, wxSizerFlags().Expand().Proportion(1));

	wxStaticText *phaseText = new wxStaticText(this, wxID_ANY, "Phase:");
	m_phaseValueCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(50, -1));
	m_phaseSlider = new wxSlider(this, wxID_ANY, rand() % 1000, 0, 1000);
	
	m_phaseSlider->Bind(wxEVT_SLIDER, &ColorChannelCtrl::OnSliderChange, this);
	sliderPanelGridSizer->Add(phaseText);
	sliderPanelGridSizer->Add(m_phaseValueCtrl);
	sliderPanelGridSizer->Add(m_phaseSlider, wxSizerFlags().Expand().Proportion(1));

	wxStaticText *colorNameText = new wxStaticText(this, wxID_ANY, m_ColorName);
	wxFont colorNameFont = colorNameText->GetFont();
	colorNameFont.SetWeight(wxFONTWEIGHT_BOLD);
	colorNameText->SetFont(colorNameFont);
	mainHSizer->Add(colorNameText, 0, wxALIGN_CENTRE_VERTICAL | wxALIGN_CENTRE_HORIZONTAL | wxLEFT | wxRIGHT, 5);
	mainHSizer->Add(sliderPanelGridSizer, wxSizerFlags().Border(wxUP).Expand().Proportion(1));

	SetSizer(mainHSizer);

	UpdateSettings();
}

Wave ColorChannelCtrl::GetWave()
{
	return Wave({ m_amplitude, m_phase, m_period });
}

void ColorChannelCtrl::UpdateSettings()
{
	m_period = (m_periodSlider->GetValue() / 1000.0) * 10;
	m_amplitude = -1 + (2.0 * m_amplitudeSlider->GetValue() / 1000.0);
	m_phase = (m_phaseSlider->GetValue() / 1000.0) * M_PI * 2.0;
	m_periodValueCtrl->SetValue(wxString::Format(wxT("%f"), m_period));
	m_amplitudeValueCtrl->SetValue(wxString::Format(wxT("%f"), m_amplitude));
	m_phaseValueCtrl->SetValue(wxString::Format(wxT("%f"), m_phase));

}

void ColorChannelCtrl::OnSliderChange(wxCommandEvent &event)
{
	UpdateSettings();

	m_ColorControlPanel->UpdateChannelSettings();
}