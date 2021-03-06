#pragma once
#include <mpir.h>
#include <wx/wx.h>
#include <wx/panel.h>
#include "ColorPreviewPanel.h"
#include <stdlib.h>
#include <chrono>
#include <time.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include "main.h"
#include "MandelbrotExplorerWindow.h"
#include "MandelView.h"

class MandelbrotExplorerWindow;

#define NUM_RESOLUTION_OPTIONS 5
#define NUM_FRACTAL_OPTIONS 2

#ifdef _WIN64
#define NUM_HARDWARE_OPTIONS 2
#else
#define NUM_HARDWARE_OPTIONS 1
#endif

enum RenderState
{
	RENDER_IDLE,
	RENDER_ACTIVE,
	RENDER_PAUSE
};



class MandelControlPanel : public wxScrolledWindow
{
private:
	RenderState render_state_;
	wxString m_FractalChoices[NUM_FRACTAL_OPTIONS] = {"Mandelbrot", "Julia"};
	int m_ResolutionValues[NUM_RESOLUTION_OPTIONS - 1][2] = { {854,480}, {1280,720}, {1920,1080}, {3480,2160} };
	wxString m_ResolutionOptions[NUM_RESOLUTION_OPTIONS] = { "854x480", "1280x720", "1920x1080", "3480x2160", "Custom" };

	wxString m_HardwareOptions[NUM_HARDWARE_OPTIONS] = { "CPU"
#ifdef _WIN64
													     ,"GPU"
#endif
	                                                   };

	int m_CurrentResolutionSelection;
	int m_CurrentHardwareSelection;

	wxSpinCtrl        *sample_rate_spinner_;
	wxSpinCtrl        *m_ThreadCountSpinCtrl;
	wxSpinCtrl        *m_GradationsSpinCtrl;
	wxSpinCtrl        *m_IterationsSpinCtrl;
	wxSlider          *m_RedSlider;
	wxSlider          *m_GreenSlider;
	wxSlider          *m_BlueSlider;
	wxChoice          *m_ColorModeDropdown;
	wxBoxSizer        *GradationsHBoxSizer;
	wxStaticText      *m_GradationsText;
	wxGauge           *m_RenderProgressBar;
	wxTextCtrl        *m_CenterRealTextBox;
	wxTextCtrl        *m_CenterImaginaryTextBox;
	wxTextCtrl        *m_PlotWidthTextBox;
	wxTextCtrl        *m_PlotHeightTextBox;
	wxTextCtrl        *m_MouseRealTextBox;
	wxTextCtrl        *m_MouseImaginaryTextBox;
	wxButton          *m_RenderButton;
	wxBitmap           m_playBitmap;
	wxBitmap           m_pauseBitmap;
	wxBitmapButton    *m_playPauseButton;
	wxBitmapButton    *m_abortButton;
	wxBitmapButton    *m_renderButton;
	wxBitmapButton    *m_videoButton;
	wxBitmapButton    *m_SaveButton;

	MandelbrotExplorerWindow  *parent_;
	ColorPreviewPanel *m_ColorPreviewPanel;
	MandelViewPanel   *m_MandelView;

	void InitializeUI();
	void OnColorSliderScroll(wxCommandEvent& event);
	void OnRenderButtonClick(wxCommandEvent& event);
	void OnResolutionSelect(wxCommandEvent& event);
	void OnPlayPauseButtonClick(wxCommandEvent& event);
	void OnVideoButtonClick(wxCommandEvent& event);
	void OnAbortButtonClick(wxCommandEvent& event);
	void OnSaveButtonClick(wxCommandEvent& event);
	void OnHardwareSelect(wxCommandEvent& event);

public:
	bool RenderInProgress();
	void GetHumanReadableMPFString(mpf_t &Num, char *buffer);
	MandelControlPanel(MandelbrotExplorerWindow *parent, int id);
	RenderSettings GetRenderSettings();
	void SetMandelView(MandelViewPanel *MandelView);
	void OnColorModeSelect(wxCommandEvent& event);
	void UpdateProgressBar(int NewValue);
	void RefreshPlot();
	void SetCenterInfo(mpf_t &x, mpf_t &i);
	void SetPlotDimensionInfo(mpf_t &width, mpf_t &height);
	void SetMouseCoordinateInfo(mpf_t &x, mpf_t &i);
	void UpdateRenderTimeInfo(std::chrono::duration<double> timeElapsed, std::chrono::duration<double> TimeRemaining);
	void OnRenderFinish(std::chrono::duration<double> timeElapsed, bool aborted);
	void SetRenderState(RenderState state);
	void SetMaxIterations(int Iterations);
};