#pragma once

#include <wx/wx.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/editors.h>
#include <wx/valgen.h>

#include "fractal_settings.h"

static const uint32_t kNumResolutionOptions = 4;
static const uint32_t resolution_options[kNumResolutionOptions][2] = { { 854,480 },{ 1280,720 },{ 1920,1080 },{ 3480,2160 } };


class MandelPropertiesPanel : public wxPropertyGrid
{
public:
	MandelPropertiesPanel(wxWindow *parent);
	void SetMouseCoordinates(wxString real, wxString imaginary);
	void SetCenterCoordinates(wxString real, wxString imaginary);
	void SetPlotDimensions(wxString height, wxString width);
	FractalSettings GetFractalSettings();
	void DisplayFractalSettings(FractalSettings settings);

private:
	uint64_t custom_resolution_height_;
	uint64_t custom_resolution_width_;

	wxPGProperty * iterations_property_;
	wxPGProperty * sample_rate_property_;
	wxPGProperty * num_threads_property_;
	wxPGProperty * mouse_real_property_;
	wxPGProperty * mouse_imaginary_property_;
	wxPGProperty * fractal_type_property_;
	wxPGProperty * resolution_property_;
	wxPGProperty * center_real_property_;
	wxPGProperty * center_imaginary_property_;
	wxPGProperty * plot_width_property_;
	wxPGProperty * plot_height_property_;
	wxPGProperty * cell_size_property_;
	wxPGProperty * color_mode_property_;
	wxPGProperty * hardware_type_property_;
	wxPGProperty * cpu_data_type_property_;
	wxPGProperty * sample_kernel_property_;

	~MandelPropertiesPanel();
	void OnPropertyChanging(wxPropertyGridEvent& event);
	void OnPropertyChanged(wxPropertyGridEvent& event);
	void OnPropertyDoubleClick(wxPropertyGridEvent& event);
	void OnDropDownSelected(wxCommandEvent& event);
	void OnKey(wxKeyEvent& event);
	wxDECLARE_EVENT_TABLE();
};

