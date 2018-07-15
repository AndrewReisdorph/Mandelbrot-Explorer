#include "mandel_properties_panel.h"

using namespace std;

enum
{
	ID_PROPS = 1
};

MandelPropertiesPanel::MandelPropertiesPanel(wxWindow *parent) :wxPropertyGrid(parent)
{
	
	InitUI();
}

void MandelPropertiesPanel::InitUI()
{
	wxArrayString fractal_types_array;
	fractal_types_array.Add("Mandelbrot");
	fractal_types_array.Add("Julia");

	wxArrayString resolutions_array;
	resolutions_array.Add("854x480");
	resolutions_array.Add("854x480");

	iterations_property_ = new wxIntProperty("Iterations", "iterations", 100);
	sample_rate_property_ = new wxIntProperty("Sample Rate", "samplerate", 100);
	num_threads_property_ = new wxIntProperty("Threads", "threads", 100);
	SetPropertyEditor(iterations_property_, wxPGEditor_SpinCtrl);
	SetPropertyEditor(sample_rate_property_, wxPGEditor_SpinCtrl);
	SetPropertyEditor(num_threads_property_, wxPGEditor_SpinCtrl);

	mouse_real_property_ = new wxStringProperty("Mouse Real", "mousereal", "0.0");
	mouse_imaginary_property_ = new wxStringProperty("Mouse Imaginary", "mouseimaginary", "0.0");
	SetPropertyEditor(mouse_real_property_, wxPGEditor_TextCtrlAndButton);
	SetPropertyEditor(mouse_imaginary_property_, wxPGEditor_TextCtrlAndButton);

	Append(new wxPropertyCategory("Appearance"));
	Append(new wxEnumProperty("Fractal", "type", fractal_types_array));
	Append(new wxEnumProperty("Resolution", "resolution", resolutions_array));
	Append(iterations_property_);
	Append(sample_rate_property_);
	Append(num_threads_property_);

	Append(new wxPropertyCategory("Geometry"));
	Append(mouse_real_property_);
	Append(mouse_imaginary_property_);
	Append(new wxStringProperty("Center Real", "centerreal", "0.0"));
	Append(new wxStringProperty("Center Imaginary", "centerimaginary", "0.0"));
	Append(new wxStringProperty("Plot Width", "plotwidth", "0.0"));
	Append(new wxStringProperty("Plot Height", "plotheight", "0.0"));
}

void MandelPropertiesPanel::OnPropertyChanged(wxPropertyGridEvent& event)
{

}

void MandelPropertiesPanel::OnPropertyButton(wxCommandEvent& event)
{
	wxPGProperty * selected_property = GetSelection();
	
	if (selected_property == mouse_real_property_)
	{
		cout << "Mouse Real" << endl;
	}
	else if (selected_property == mouse_imaginary_property_)
	{
		cout << "Mouse Imaginary" << endl;
	}

}

wxBEGIN_EVENT_TABLE(MandelPropertiesPanel, wxPropertyGrid)
EVT_PG_CHANGED(ID_PROPS, MandelPropertiesPanel::OnPropertyChanged)
EVT_BUTTON(wxID_ANY, MandelPropertiesPanel::OnPropertyButton)
wxEND_EVENT_TABLE()