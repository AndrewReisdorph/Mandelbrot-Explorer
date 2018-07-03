#include "ColorControlPanel.h"
#include "ColorChannelGraph.h"
#include "main.h"

ColorControlPanel::ColorControlPanel(MandelbrotExplorerWindow *parent) :wxPanel(parent)
{
	m_parent = parent;

	InitializeUI();
}

void ColorControlPanel::InitializeUI()
{
	wxPanel *ColorModePanel = new wxPanel(this);
	ColorModePanel->SetWindowStyle(wxBORDER_SUNKEN);

	m_ColorDisplay = new ColorDisplayPanel(this);
	m_ChannelGraph = new ColorChannelGraph(this);
	wxBoxSizer *mainHSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *ColorEditorSizer = new wxBoxSizer(wxVERTICAL);
	wxFlexGridSizer *ColorModeGridSizer = new wxFlexGridSizer(1,2,5,5);
	ColorModeGridSizer->AddGrowableCol(1);

	wxStaticText *ColorModeText = new wxStaticText(ColorModePanel, wxID_ANY, "Color Mode:");
	wxChoice *ColorModeDropdown = new wxChoice(ColorModePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, NUM_COLOR_MODES, m_ColorModeOptions);
	ColorModeDropdown->SetSelection(0);
	ColorModeGridSizer->Add(ColorModeText, wxALIGN_CENTRE_VERTICAL | wxALL);
	ColorModeGridSizer->Add(ColorModeDropdown, wxALIGN_CENTRE_VERTICAL | wxALL);
	ColorModePanel->SetSizer(ColorModeGridSizer);

	wxBoxSizer *channelSettingsHSizer = new wxBoxSizer(wxHORIZONTAL);
	m_RedChannelCtrl = new ColorChannelCtrl(this,"Red");
	m_GreenChannelCtrl = new ColorChannelCtrl(this, "Green");
	m_BlueChannelCtrl = new ColorChannelCtrl(this, "Blue");

	channelSettingsHSizer->Add(m_RedChannelCtrl, wxSizerFlags().Expand().Proportion(1));
	channelSettingsHSizer->Add(m_GreenChannelCtrl, wxSizerFlags().Expand().Proportion(1));
	channelSettingsHSizer->Add(m_BlueChannelCtrl, wxSizerFlags().Expand().Proportion(1));

	ColorEditorSizer->Add(channelSettingsHSizer,wxSizerFlags().Expand());
	ColorEditorSizer->Add(m_ChannelGraph,wxSizerFlags().Expand());
	ColorEditorSizer->Add(m_ColorDisplay,wxSizerFlags().Expand());

	mainHSizer->Add(ColorModePanel,wxSizerFlags().Expand());
	mainHSizer->Add(ColorEditorSizer,wxSizerFlags().Expand().Proportion(1));

	SetSizer(mainHSizer);
}

void ColorControlPanel::UpdateChannelSettings()
{
	Wave Red = m_RedChannelCtrl->GetWave();
	Wave Green = m_GreenChannelCtrl->GetWave();
	Wave Blue = m_BlueChannelCtrl->GetWave();
	m_ChannelGraph->SetChannelSettings(Red, Green, Blue);
	m_ColorDisplay->SetChannelSettings(Red, Green, Blue);

}