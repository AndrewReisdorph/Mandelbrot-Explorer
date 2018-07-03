#include <wx/wx.h>
#include <wx/slider.h>
#include <wx/sizer.h>
#include <wx/gauge.h>
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "MandelControlPanel.h"
#include "CustomResolutionDialog.h"
#include "ColorChannelGraph.h"
#include "images.h"
#include "main.h"

using namespace std;

MandelControlPanel::MandelControlPanel(MandelbrotExplorerWindow *parent, int id):wxPanel(parent, id)
{
	parent_ = parent;
	render_state_ = RENDER_IDLE;

	MandelControlPanel::InitializeUI();

	// Set Event Bindings
	Bind(wxEVT_CHOICE, &MandelControlPanel::OnColorModeSelect, this, ID_ColorModeChoice);
	Bind(wxEVT_SLIDER, &MandelControlPanel::OnColorSliderScroll, this, ID_ColorSliderRed);
	Bind(wxEVT_SLIDER, &MandelControlPanel::OnColorSliderScroll, this, ID_ColorSliderGreen);
	Bind(wxEVT_SLIDER, &MandelControlPanel::OnColorSliderScroll, this, ID_ColorSliderBlue);
}

void MandelControlPanel::SetMaxIterations(int Iterations)
{
	m_IterationsSpinCtrl->SetValue(Iterations);
}

void MandelControlPanel::InitializeUI()
{
	srand(time(NULL));
	wxInitAllImageHandlers();

	wxBoxSizer *main_vbox = new wxBoxSizer(wxVERTICAL);
	wxStaticBoxSizer *appearance_static_box = new wxStaticBoxSizer(wxVERTICAL, this, "Appearance");
	appearance_static_box->GetStaticBox()->InheritsBackgroundColour();
	appearance_static_box->GetStaticBox()->SetForegroundColour(wxColour(0,102,206));

	// Load toolbar image data
	m_playBitmap = wxBITMAP_PNG_FROM_DATA(play40);
	m_pauseBitmap = wxBITMAP_PNG_FROM_DATA(pause40);
	wxBitmap videoBitmap = wxBITMAP_PNG_FROM_DATA(video40);
	wxBitmap renderBitmap = wxBITMAP_PNG_FROM_DATA(render40);
	wxBitmap saveBitmap = wxBITMAP_PNG_FROM_DATA(save40);
	wxBitmap abortBitmap = wxBITMAP_PNG_FROM_DATA(abort40);

	m_videoButton = new wxBitmapButton(this, wxID_ANY, videoBitmap);
	m_videoButton->SetToolTip("Render Video");
	m_videoButton->Bind(wxEVT_BUTTON, &MandelControlPanel::OnVideoButtonClick, this);
	m_renderButton = new wxBitmapButton(this, wxID_ANY, renderBitmap);
	m_renderButton->SetToolTip("Render");
	m_renderButton->Bind(wxEVT_BUTTON, &MandelControlPanel::OnRenderButtonClick, this);
	m_playPauseButton = new wxBitmapButton(this, wxID_ANY, m_playBitmap);
	m_playPauseButton->Enable(false);
	m_playPauseButton->Bind(wxEVT_BUTTON, &MandelControlPanel::OnPlayPauseButtonClick, this);
	m_SaveButton = new wxBitmapButton(this, wxID_ANY, saveBitmap);
	m_SaveButton->SetToolTip("Save Image");
	m_SaveButton->Bind(wxEVT_BUTTON, &MandelControlPanel::OnSaveButtonClick, this);
	m_abortButton = new wxBitmapButton(this, wxID_ANY, abortBitmap);
	m_abortButton->Bind(wxEVT_BUTTON, &MandelControlPanel::OnAbortButtonClick, this);
	m_abortButton->SetToolTip("Abort");
	m_abortButton->Enable(false);

	wxBoxSizer *toolbarHBox = new wxBoxSizer(wxHORIZONTAL);
	toolbarHBox->Add(m_renderButton);
	toolbarHBox->Add(m_videoButton);
	toolbarHBox->Add(m_playPauseButton);
	toolbarHBox->Add(m_SaveButton);
	toolbarHBox->Add(m_abortButton);


	main_vbox->Add(toolbarHBox);

	wxFlexGridSizer *AppearanceFlexSizer = new wxFlexGridSizer(6,2,5,5);
	AppearanceFlexSizer->AddGrowableCol(1);

	wxStaticText *fractal_select_text = new wxStaticText(this, wxID_ANY, "Fractal:");
	fractal_select_text->InheritsBackgroundColour();
	wxChoice *fractal_select_dropdown = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, NUM_FRACTAL_OPTIONS, m_FractalChoices);
	fractal_select_dropdown->SetSelection(0);

	AppearanceFlexSizer->Add(fractal_select_text, 0, wxALIGN_CENTRE_VERTICAL);
	AppearanceFlexSizer->Add(fractal_select_dropdown, wxSizerFlags().Expand());

	wxStaticText *resolution_text = new wxStaticText(this, wxID_ANY, "Resolution:");
	wxChoice *ResolutionSelectDropdown = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, NUM_RESOLUTION_OPTIONS, m_ResolutionOptions);
	m_CurrentResolutionSelection = 0;
	ResolutionSelectDropdown->SetSelection(m_CurrentResolutionSelection);
	ResolutionSelectDropdown->Bind(wxEVT_CHOICE, &MandelControlPanel::OnResolutionSelect, this);
	AppearanceFlexSizer->Add(resolution_text);
	AppearanceFlexSizer->Add(ResolutionSelectDropdown, wxSizerFlags().Expand());

	wxStaticText *IterationsText = new wxStaticText(this, wxID_ANY, "Iterations:");
	m_IterationsSpinCtrl = new wxSpinCtrl(this, wxID_ANY);
	m_IterationsSpinCtrl->SetRange(1, 65535);
	m_IterationsSpinCtrl->SetValue(100);

	AppearanceFlexSizer->Add(IterationsText, 0, wxALIGN_CENTRE_VERTICAL);
	AppearanceFlexSizer->Add(m_IterationsSpinCtrl, wxSizerFlags().Expand());

	wxStaticText *sample_rate_text = new wxStaticText(this, wxID_ANY, "Sample Rate:");
	sample_rate_spinner_ = new wxSpinCtrl(this, wxID_ANY);
	sample_rate_spinner_->SetRange(1, 1000);
	sample_rate_spinner_->SetValue(2);

	AppearanceFlexSizer->Add(sample_rate_text, 0, wxALIGN_CENTRE_VERTICAL);
	AppearanceFlexSizer->Add(sample_rate_spinner_, wxSizerFlags().Expand());

	wxStaticText *hardwareSelectText = new wxStaticText(this, wxID_ANY, "Hardware:");
	wxChoice *HardwareSelectDropdown = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, NUM_HARDWARE_OPTIONS, m_HardwareOptions);
	m_CurrentHardwareSelection = 0;
	HardwareSelectDropdown->SetSelection(m_CurrentHardwareSelection);
	HardwareSelectDropdown->Bind(wxEVT_CHOICE, &MandelControlPanel::OnHardwareSelect, this);
	AppearanceFlexSizer->Add(hardwareSelectText, 0, wxALIGN_CENTRE_VERTICAL);
	AppearanceFlexSizer->Add(HardwareSelectDropdown, wxSizerFlags().Expand());

	wxStaticText *ThreadCountStaticText = new wxStaticText(this, wxID_ANY, "Threads:");
	m_ThreadCountSpinCtrl = new wxSpinCtrl(this, wxID_ANY);
	m_ThreadCountSpinCtrl->SetRange(1, 32);
	m_ThreadCountSpinCtrl->SetValue(2);

	AppearanceFlexSizer->Add(ThreadCountStaticText, 0, wxALIGN_CENTRE_VERTICAL);
	AppearanceFlexSizer->Add(m_ThreadCountSpinCtrl, wxSizerFlags().Expand());

#if 0
	wxBoxSizer *ColorMode_hbox = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *ColorModeText = new wxStaticText(this, wxID_ANY, "Color Mode:");
	ColorMode_hbox->Add(ColorModeText, wxSizerFlags().CenterVertical().Border(wxRIGHT));
	m_ColorModeDropdown = new wxChoice(this, ID_ColorModeChoice, wxDefaultPosition, wxDefaultSize, 2, m_ColorModeChoices);
	m_ColorModeDropdown->SetSelection(0);
	ColorMode_hbox->Add(m_ColorModeDropdown);

	GradationsHBoxSizer = new wxBoxSizer(wxHORIZONTAL);
	m_GradationsText = new wxStaticText(this, wxID_ANY, "Gradations:");
	GradationsHBoxSizer->Add(m_GradationsText, wxSizerFlags().CenterVertical().Border(wxRIGHT));
	m_GradationsSpinCtrl = new wxSpinCtrl(this, wxID_ANY);
	m_GradationsSpinCtrl->SetMin(1);
	m_GradationsSpinCtrl->SetMax(65535);
	m_GradationsSpinCtrl->SetValue(100);
	GradationsHBoxSizer->Add(m_GradationsSpinCtrl);
#endif
	//wxBoxSizer *ColorSliders_vbox = new wxBoxSizer(wxVERTICAL);
	//wxStaticText *RedText = new wxStaticText(this, wxID_ANY, "Red:");
	//ColorSliders_vbox->Add(RedText);
	//int RandRedValue = 172;//34,204,rand() % 100 + 1;
	//m_RedSlider = new wxSlider(this, ID_ColorSliderRed, RandRedValue, 0, 255);
	//ColorSliders_vbox->Add(m_RedSlider, wxSizerFlags().Expand());
	//int RandGreenValue = 217;//101,175, rand() % 100 + 1;
	//wxStaticText *GreenText = new wxStaticText(this, wxID_ANY, "Green:");
	//ColorSliders_vbox->Add(GreenText);
	//m_GreenSlider = new wxSlider(this, ID_ColorSliderGreen, RandGreenValue, 0, 255);
	//ColorSliders_vbox->Add(m_GreenSlider, wxSizerFlags().Expand());
	//wxStaticText *BlueText = new wxStaticText(this, wxID_ANY, "Blue:");
	//ColorSliders_vbox->Add(BlueText);
	//int RandBlueValue = 28;//161,161, rand() % 100 + 1;
	//m_BlueSlider = new wxSlider(this, ID_ColorSliderBlue, RandBlueValue, 0, 255);
	//ColorSliders_vbox->Add(m_BlueSlider, wxSizerFlags().Expand());

	//m_ColorPreviewPanel = new ColorPreviewPanel(this);

	appearance_static_box->Add(AppearanceFlexSizer, wxSizerFlags().Expand());
	//appearance_static_box->Add(ColorMode_hbox, wxSizerFlags().Border(wxUP));
	//appearance_static_box->Add(GradationsHBoxSizer, wxSizerFlags().Border(wxUP));
	//appearance_static_box->Add(ColorSliders_vbox, wxSizerFlags().Border(wxUP).Expand());
	//appearance_static_box->Add(m_ColorPreviewPanel, wxSizerFlags().Expand());

	// Geometry
	wxStaticBoxSizer *LocationBox = new wxStaticBoxSizer(wxVERTICAL, this, "Geometry");
	LocationBox->GetStaticBox()->InheritsBackgroundColour();
	LocationBox->GetStaticBox()->SetForegroundColour(wxColour(0, 102, 206));

	wxStaticText *MouseCoordinateText = new wxStaticText(this, wxID_ANY, "Mouse Coordinates:");
	LocationBox->Add(MouseCoordinateText);
	wxFlexGridSizer *MouseCoordinateFlexSizer = new wxFlexGridSizer(2, 2, 5, 5);
	MouseCoordinateFlexSizer->AddGrowableCol(1);
	wxStaticText *MouseRealText = new wxStaticText(this, wxID_ANY, "Real:");
	MouseCoordinateFlexSizer->Add(MouseRealText, 0, wxALIGN_CENTRE_VERTICAL);
	m_MouseRealTextBox = new wxTextCtrl(this, wxID_ANY);
	MouseCoordinateFlexSizer->Add(m_MouseRealTextBox,wxSizerFlags().Proportion(1).Expand());
	wxStaticText *MouseImaginaryText = new wxStaticText(this, wxID_ANY, "Imaginary:");
	MouseCoordinateFlexSizer->Add(MouseImaginaryText, 0, wxALIGN_CENTRE_VERTICAL);
	m_MouseImaginaryTextBox = new wxTextCtrl(this, wxID_ANY);
	MouseCoordinateFlexSizer->Add(m_MouseImaginaryTextBox, wxSizerFlags().Proportion(1).Expand());
	LocationBox->Add(MouseCoordinateFlexSizer,wxSizerFlags().Expand());

	wxBoxSizer *centerCoordinateLabelBox = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *CenterCoordinateText = new wxStaticText(this, wxID_ANY, "Center Coordinates:");
	wxButton *setCenterCoordinatesButton = new wxButton(this, wxID_ANY, "...", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	centerCoordinateLabelBox->Add(CenterCoordinateText, 0, wxALIGN_CENTRE_VERTICAL | wxRIGHT);
	centerCoordinateLabelBox->Add(setCenterCoordinatesButton);

	wxFlexGridSizer *CenterCoordinateFlexSizer = new wxFlexGridSizer(2, 2, 5, 5);
	CenterCoordinateFlexSizer->AddGrowableCol(1);
	LocationBox->Add(centerCoordinateLabelBox, wxSizerFlags().Border(wxUP));
	wxStaticText *CenterRealText = new wxStaticText(this, wxID_ANY, "Real:");
	CenterCoordinateFlexSizer->Add(CenterRealText, 0, wxALIGN_CENTRE_VERTICAL);
	m_CenterRealTextBox = new wxTextCtrl(this, wxID_ANY);
	CenterCoordinateFlexSizer->Add(m_CenterRealTextBox, wxSizerFlags().Proportion(1).Expand());
	wxStaticText *CenterImaginaryText = new wxStaticText(this, wxID_ANY, "Imaginary:");
	CenterCoordinateFlexSizer->Add(CenterImaginaryText, 0, wxALIGN_CENTRE_VERTICAL);
	m_CenterImaginaryTextBox = new wxTextCtrl(this, wxID_ANY);
	CenterCoordinateFlexSizer->Add(m_CenterImaginaryTextBox, wxSizerFlags().Proportion(1).Expand());
	LocationBox->Add(CenterCoordinateFlexSizer, wxSizerFlags().Expand().Border(wxDOWN));

	wxBoxSizer *plotDimensionsHBox = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *plotDimensionsText = new wxStaticText(this, wxID_ANY, "Plot Dimensions:");
	wxButton *setPlotDimensionsButton = new wxButton(this, wxID_ANY, "...", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	plotDimensionsHBox->Add(plotDimensionsText, 0, wxRIGHT | wxALIGN_CENTRE_VERTICAL);
	plotDimensionsHBox->Add(setPlotDimensionsButton);
	LocationBox->Add(plotDimensionsHBox);

	wxFlexGridSizer *PlotDimensionsFlexSizer = new wxFlexGridSizer(2, 2, 5, 5);
	PlotDimensionsFlexSizer->AddGrowableCol(1);
	wxStaticText *PlotWidthText = new wxStaticText(this, wxID_ANY, "Plot Width:");
	PlotDimensionsFlexSizer->Add(PlotWidthText, 0, wxALIGN_CENTRE_VERTICAL);
	m_PlotWidthTextBox = new wxTextCtrl(this, wxID_ANY);
	PlotDimensionsFlexSizer->Add(m_PlotWidthTextBox, wxSizerFlags().Proportion(1).Expand());
	wxStaticText *PlotHeightText = new wxStaticText(this, wxID_ANY, "Plot Height:");
	PlotDimensionsFlexSizer->Add(PlotHeightText,0,  wxALIGN_CENTRE_VERTICAL);
	m_PlotHeightTextBox = new wxTextCtrl(this, wxID_ANY);
	PlotDimensionsFlexSizer->Add(m_PlotHeightTextBox, wxSizerFlags().Proportion(1).Expand());
	LocationBox->Add(PlotDimensionsFlexSizer, wxSizerFlags().Expand());

	long progressBarStyle = wxGA_HORIZONTAL | wxGA_SMOOTH;
	// wxGA_PROGRESS is only supported on Windows
	#ifdef _WIN32
	progressBarStyle |= wxGA_PROGRESS;
	#endif
	m_RenderProgressBar = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, progressBarStyle);

	main_vbox->Add(appearance_static_box, wxSizerFlags().Border().Expand());
	main_vbox->Add(LocationBox, wxSizerFlags().Border().Expand());
	main_vbox->AddStretchSpacer();
	main_vbox->Add(m_RenderProgressBar, wxSizerFlags().Border().Expand());

	SetSizer(main_vbox);
}

void MandelControlPanel::SetMandelView(MandelViewPanel *MandelView)
{
	m_MandelView = MandelView;
}

void MandelControlPanel::SetRenderState(RenderState state)
{
	render_state_ = state;
	switch (render_state_)
	{
	case RENDER_IDLE:
		m_playPauseButton->SetBitmap(m_playBitmap);
		m_playPauseButton->Enable(false);
		m_abortButton->Enable(false);
		m_renderButton->Enable(true);
		m_videoButton->Enable(true);
		m_SaveButton->Enable(true);
		break;

	case RENDER_ACTIVE:
		m_playPauseButton->SetBitmap(m_pauseBitmap);
		m_playPauseButton->Enable(true);
		m_abortButton->Enable(true);
		m_SaveButton->Enable(false);
		m_videoButton->Enable(false);
		m_renderButton->Enable(false);
		break;

	case RENDER_PAUSE:
		m_playPauseButton->SetBitmap(m_playBitmap);
		m_playPauseButton->Enable(true);
		m_abortButton->Enable(true);
		m_SaveButton->Enable(true);
		break;

	default:
		break;
	}

}

void MandelControlPanel::OnResolutionSelect(wxCommandEvent& event)
{
	int ResolutionSelection = event.GetSelection();

	if (ResolutionSelection != m_CurrentResolutionSelection || (m_ResolutionOptions[ResolutionSelection].Cmp("Custom") == 0))
	{
		int NewWidth = 0;
		int NewHeight = 0;

		
		std::cout << "Resolution:" << m_ResolutionOptions[m_CurrentResolutionSelection] << std::endl;

		if (ResolutionSelection == NUM_RESOLUTION_OPTIONS - 1)
		{
			CustomResolutionDialog *ResolutionDialog = new CustomResolutionDialog();
			ResolutionDialog->Show(true);
			int ReturnCode = ResolutionDialog->GetReturnCode();
			if (ReturnCode == wxOK)
			{
				NewWidth = ResolutionDialog->GetWidth();
				NewHeight = ResolutionDialog->GetHeight();
				m_CurrentResolutionSelection = ResolutionSelection;
			}
			else
			{
				// Don't update resolution if the resolution dialog is canceled
				return;
			}
		}
		else
		{
			m_CurrentResolutionSelection = ResolutionSelection;
			NewWidth = m_ResolutionValues[m_CurrentResolutionSelection][0];
			NewHeight = m_ResolutionValues[m_CurrentResolutionSelection][1];
		}

		m_MandelView->SetResolution(NewWidth, NewHeight);
	}
	
	
}

void MandelControlPanel::OnPlayPauseButtonClick(wxCommandEvent& event)
{
	switch (render_state_)
	{
	case RENDER_ACTIVE:
		SetRenderState(RENDER_PAUSE);
		m_MandelView->PauseRender();
		break;
	case RENDER_PAUSE:
		SetRenderState(RENDER_ACTIVE);
		m_MandelView->ResumeRender();
		break;
	default:
		break;
	}

}

void MandelControlPanel::OnRenderButtonClick(wxCommandEvent& event)
{
	switch (render_state_)
	{
	case RENDER_IDLE:
		SetRenderState(RENDER_ACTIVE);
		m_MandelView->Render();
		break;
	case RENDER_ACTIVE:
		SetRenderState(RENDER_PAUSE);
		m_MandelView->PauseRender();
		break;
	case RENDER_PAUSE:
		SetRenderState(RENDER_ACTIVE);
		m_MandelView->ResumeRender();
		break;
	default:
		break;
	}
}

void MandelControlPanel::OnVideoButtonClick(wxCommandEvent& event)
{
	m_MandelView->RenderVideo();
	SetRenderState(RENDER_ACTIVE);
}

void MandelControlPanel::OnAbortButtonClick(wxCommandEvent & event)
{
	m_MandelView->AbortRender();
}

void MandelControlPanel::OnSaveButtonClick(wxCommandEvent & event)
{
	m_MandelView->SaveImage();
}

void MandelControlPanel::OnHardwareSelect(wxCommandEvent & event)
{
	int HardwareSelection = event.GetSelection();

	if (HardwareSelection != m_CurrentHardwareSelection)
	{
		
	}
	m_CurrentHardwareSelection = HardwareSelection;
}

void MandelControlPanel::UpdateRenderTimeInfo(chrono::duration<double> timeElapsed, chrono::duration<double> TimeRemaining)
{
	ostringstream stream;
	stream << "Time Elapsed: " << DurationToString(timeElapsed) << " | Time Remaining: " << DurationToString(TimeRemaining);

	parent_->SetStatusBarText(stream.str());
}

void MandelControlPanel::OnRenderFinish(std::chrono::duration<double> timeElapsed, bool aborted)
{
	std::ostringstream stream;
	if (aborted)
	{
		stream << "Render Aborted. Time Elapsed: " << DurationToString(timeElapsed);
	}
	else
	{
		stream << "Render Complete. Time Elapsed: " << DurationToString(timeElapsed);
	}

	parent_->SetStatusBarText(stream.str());
	m_RenderProgressBar->SetValue(0);
	SetRenderState(RENDER_IDLE);
}

void MandelControlPanel::UpdateProgressBar(int NewValue)
{
	m_RenderProgressBar->SetValue(NewValue);
}

RenderSettings MandelControlPanel::GetRenderSettings()
{
	RenderSettings settings = { 0 };
	settings.Iterations = m_IterationsSpinCtrl->GetValue();
	settings.SuperSamplingRate = sample_rate_spinner_->GetValue();
	settings.NumThreads = m_ThreadCountSpinCtrl->GetValue();
	settings.UseGPU = (m_CurrentHardwareSelection == 1);
#if 1
	settings.ColorMode = CONTINUOUS;
	settings.Red = 172;
	settings.Green = 217;
	settings.Blue = 28;
	settings.Gradations = 100;
#else
	settings.ColorMode = ColorModes(m_ColorModeDropdown->GetSelection());
	settings.Gradations = m_GradationsSpinCtrl->GetValue();
	settings.Red = m_RedSlider->GetValue();
	settings.Green = m_GreenSlider->GetValue();
	settings.Blue = m_BlueSlider->GetValue();
#endif

	return settings;
}

void MandelControlPanel::OnColorModeSelect(wxCommandEvent& event)
{
	ColorModes mode = ColorModes(m_ColorModeDropdown->GetSelection());

	if (mode == CONTINUOUS)
	{
		m_GradationsSpinCtrl->Show();
		m_GradationsText->Show();
		Layout();
	}
	else if(mode == PROPORTIONAL)
	{
		m_GradationsSpinCtrl->Hide();
		m_GradationsText->Hide();
		Layout();
	}

	m_ColorPreviewPanel->Refresh();
}

void MandelControlPanel::OnColorSliderScroll(wxCommandEvent& event)
{
	m_ColorPreviewPanel->Refresh();
}

void MandelControlPanel::RefreshPlot()
{
	m_parent->Refresh();
}

bool MandelControlPanel::RenderInProgress()
{
	return (render_state_ == RENDER_ACTIVE) || (render_state_ == RENDER_PAUSE);
}

void MandelControlPanel::GetHumanReadableMPFString(mpf_t &Num, char *buffer)
{
	mp_exp_t exponent;
	char leading_zeros[100] = { 0 };
	char whole_numbers[100] = { 0 };
	char temp_buffer[200];
	mpf_get_str(temp_buffer, &exponent, 10, 90, Num);
	bool isnegative = (temp_buffer[0] == '-');

	if (exponent < 0)
	{
		if (abs(exponent) > 99)
		{
			return;
		}
		for (int i = 0; i < abs(exponent); i++)
		{
			leading_zeros[i] = '0';
		}
		sprintf(buffer, "%s0.%s%s", isnegative ? "-" : "", leading_zeros, isnegative ? temp_buffer + 1 : temp_buffer);
	}
	else
	{
		int offset = isnegative ? 1 : 0;
		int i = 0;

		if (exponent == 0)
		{
			whole_numbers[0] = '0';
		}
		else
		{

			for (i = 0; i < abs(exponent); i++)
			{
				whole_numbers[i] = temp_buffer[offset + i];
			}
		}
		sprintf(buffer, "%s%s.%s", isnegative ? "-" : "", whole_numbers, (temp_buffer + i + offset == temp_buffer + strlen(temp_buffer)) ? "0": temp_buffer + i + offset);
	}
}

void MandelControlPanel::SetMouseCoordinateInfo(mpf_t &x, mpf_t &i)
{
	char buff[200];

	GetHumanReadableMPFString(x, buff);
	m_MouseRealTextBox->SetValue(wxString(buff));
	GetHumanReadableMPFString(i, buff);
	m_MouseImaginaryTextBox->SetValue(wxString(buff));
}

void MandelControlPanel::SetCenterInfo(mpf_t &x, mpf_t &i)
{
	char buff[200];

	GetHumanReadableMPFString(x, buff);
	m_CenterRealTextBox->SetValue(wxString(buff));
	GetHumanReadableMPFString(i, buff);
	m_CenterImaginaryTextBox->SetValue(wxString(buff));
}

void MandelControlPanel::SetPlotDimensionInfo(mpf_t &width, mpf_t &height)
{
	char buff[200];

	GetHumanReadableMPFString(width, buff);
	m_PlotWidthTextBox->SetValue(wxString(buff));
	GetHumanReadableMPFString(height, buff);
	m_PlotHeightTextBox->SetValue(wxString(buff));
}