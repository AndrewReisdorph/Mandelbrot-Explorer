#pragma once

#include <wx/wx.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/editors.h>


class MandelPropertiesPanel : public wxPropertyGrid
{
public:
	MandelPropertiesPanel(wxWindow *parent);

private:
	wxPGProperty * iterations_property_;
	wxPGProperty * sample_rate_property_;
	wxPGProperty * num_threads_property_;
	wxPGProperty * mouse_real_property_;
	wxPGProperty * mouse_imaginary_property_;

	void InitUI();
	void OnPropertyChanged(wxPropertyGridEvent& event);
	void OnPropertyButton(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
};