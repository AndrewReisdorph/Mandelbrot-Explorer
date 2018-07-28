// Copyright 2018 Andrew Reisdorph
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#ifndef MANDEL_PROPERTIES_PANEL_H
#define MANDEL_PROPERTIES_PANEL_H

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

#endif