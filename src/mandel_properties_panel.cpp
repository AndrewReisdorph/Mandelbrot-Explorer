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

#include <thread>

#include "../include/mandel_properties_panel.h"
#include "../include/custom_resolution_dialog.h"
#include "../include/regex_validator.h"

using std::cout;
using std::endl;
using std::thread;

MandelPropertiesPanel::MandelPropertiesPanel(wxWindow *parent)
    :wxPropertyGrid(parent) {
  wxArrayString fractal_types_array(kNumFractalTypes, fractal_type_names);
  wxArrayString color_modes_array(kNumColorModes, color_mode_names);
  wxArrayString hardware_types_array(kNumHardwareTypes, hardware_type_names);
  wxArrayString cpu_data_types_array(kNumCpuPrecions, cpu_precision_names);
  wxArrayString sample_kernel_types_array(kNumKernelTypes, kernel_type_names);

  wxArrayString resolutions_array;
  for (uint32_t resolution_iter = 0; resolution_iter < kNumResolutionOptions;
       resolution_iter++) {
    resolutions_array.Add(wxString::Format(wxT("%ix%i"),
                          resolution_options[resolution_iter][0],
                          resolution_options[resolution_iter][1]));
  }
  resolutions_array.Add("Custom");

  iterations_property_ = new wxUIntProperty("Iterations", "iterations", 100);
  iterations_property_->SetAttribute("Min", wxVariant(1));
  sample_rate_property_ = new wxUIntProperty("Sample Rate", "samplerate", 2);
  sample_rate_property_->SetAttribute("Min", wxVariant(1));
  // Set the default number of threads to the number of CPU cores minus one
  // Using all cores slows down the machine signifcantly during rendering
  unsigned concurrent_threads_supported = std::thread::hardware_concurrency();
  if (concurrent_threads_supported >= 2) {
    concurrent_threads_supported -= 1;
  }
  num_threads_property_ = new wxUIntProperty("Threads", "threads",
                                             concurrent_threads_supported);
  num_threads_property_->SetAttribute("Min", wxVariant(1));
  cell_size_property_ = new wxUIntProperty("Cell Size", "cellsize", 1);
  cell_size_property_->SetAttribute("Min", wxVariant(1));

  // TODO(andrew): Figure out how to make sure row heights are big enough to
  // a gtkspinner
#if !defined(__linux__)
  SetPropertyEditor(iterations_property_, wxPGEditor_SpinCtrl);
  SetPropertyEditor(sample_rate_property_, wxPGEditor_SpinCtrl);
  SetPropertyEditor(num_threads_property_, wxPGEditor_SpinCtrl);
  SetPropertyEditor(cell_size_property_, wxPGEditor_SpinCtrl);
#endif

  mouse_real_property_ = new wxStringProperty("Mouse Real", "mousereal", "0.0");
  mouse_imaginary_property_ = new wxStringProperty("Mouse Imaginary",
                                                   "mouseimaginary", "0.0");

  fractal_type_property_ = new wxEnumProperty("Fractal", "type",
                                              fractal_types_array);
  resolution_property_ = new  wxEditEnumProperty("Resolution", "resolution",
                                                 resolutions_array);
  resolution_property_->SetChoiceSelection(0);
  color_mode_property_ = new wxEnumProperty("Color Mode", "colormode",
                                            color_modes_array);
  hardware_type_property_ = new wxEnumProperty("Hardware", "hardware",
                                               hardware_types_array);
  cpu_data_type_property_ = new wxEnumProperty("Precision", "precision",
                                               cpu_data_types_array);
  sample_kernel_property_ = new wxEnumProperty("Sample Kernel", "samplekernel",
                                               sample_kernel_types_array);

  wxString geometry_property_string;
  wxString positigve_geometry_property_string;
  int validator_style = wxREV_FILTER_INCLUDE_CHAR_LIST | wxREV_FILTER_REGEX;
  wxRegexValidator positive_multi_float_validator(validator_style,
                      &positigve_geometry_property_string);
  wxRegexValidator any_multi_float_validator(validator_style,
                                             &geometry_property_string);
  wxString any_float_pattern = "^-?[[:digit:]]+(\\.?[[:digit:]]+)?$";
  any_multi_float_validator.SetRegexPattern(any_float_pattern);
  wxString positive_float_pattern = "^[[:digit:]]+(\\.?[[:digit:]]+)?$";
  positive_multi_float_validator.SetRegexPattern(positive_float_pattern);
  wxArrayString any_float_char_list;
  wxArrayString positive_float_char_list;
  wxString valid_chars(wxT("0123456789."));
  size_t num_chars = valid_chars.Length();
  for (size_t char_iter = 0; char_iter < num_chars; char_iter++) {
    char next_valid_char = valid_chars.GetChar(char_iter);
    any_float_char_list.Add(wxString(next_valid_char));
    positive_float_char_list.Add(wxString(next_valid_char));
  }
  any_float_char_list.Add("-");
  any_multi_float_validator.SetIncludes(any_float_char_list);
  positive_multi_float_validator.SetIncludes(positive_float_char_list);

  center_real_property_ = new wxStringProperty("Center Real", "centerreal",
                                               "0.0");
  center_real_property_->SetValidator(any_multi_float_validator);
  center_imaginary_property_ = new wxStringProperty("Center Imaginary",
                                                    "centerimaginary", "0.0");
  center_imaginary_property_->SetValidator(any_multi_float_validator);
  plot_width_property_ = new wxStringProperty("Plot Width", "plotwidth", "0.0");
  plot_width_property_->SetValidator(positive_multi_float_validator);
  plot_height_property_ = new wxStringProperty("Plot Height", "plotheight",
                                               "0.0");
  plot_height_property_->SetValidator(positive_multi_float_validator);

  Append(new wxPropertyCategory("Appearance"));
  Append(fractal_type_property_);
  Append(resolution_property_);
  Append(iterations_property_);
  Append(cell_size_property_);
  Append(sample_rate_property_);
  Append(sample_kernel_property_);
  Append(color_mode_property_);

  Append(new wxPropertyCategory("Algorithm"));
  Append(hardware_type_property_);
  Append(num_threads_property_);
  Append(cpu_data_type_property_);

  Append(new wxPropertyCategory("Geometry"));
  Append(mouse_real_property_);
  Append(mouse_imaginary_property_);
  Append(center_real_property_);
  Append(center_imaginary_property_);
  Append(plot_width_property_);
  Append(plot_height_property_);
}

void MandelPropertiesPanel::OnPropertyChanged(wxPropertyGridEvent& event) {
  wxPGProperty * selected_property = GetSelection();
}

void MandelPropertiesPanel::OnPropertyDoubleClick(wxPropertyGridEvent & event) {
  FitColumns();
}

void MandelPropertiesPanel::SetMouseCoordinates(wxString real,
    wxString imaginary) {
  mouse_real_property_->SetValueFromString(real);
  mouse_imaginary_property_->SetValueFromString(imaginary);
}

void MandelPropertiesPanel::SetCenterCoordinates(wxString real,
    wxString imaginary) {
  center_real_property_->SetValueFromString(real);
  center_imaginary_property_->SetValueFromString(imaginary);
}

void MandelPropertiesPanel::SetPlotDimensions(wxString height, wxString width) {
  plot_width_property_->SetValueFromString(width);
  plot_height_property_->SetValueFromString(height);
}

FractalSettings MandelPropertiesPanel::GetFractalSettings() {
  FractalSettings settings;

  int fractal_type_int = fractal_type_property_->GetValue().GetInteger();
  settings.fractal_type = static_cast<FractalType>(fractal_type_int);
  int kernel_type_int = sample_kernel_property_->GetValue().GetInteger();
  settings.kernel_type = static_cast<KernelType>(kernel_type_int);
  int color_mode_int = color_mode_property_->GetValue().GetInteger();
  settings.color_mode = static_cast<ColorMode>(color_mode_int);
  int hardware_int = hardware_type_property_->GetValue().GetInteger();
  settings.hardware = static_cast<HardwareType>(hardware_int);
  int cpu_precision_int = cpu_data_type_property_->GetValue().GetInteger();
  settings.cpu_precision = static_cast<CpuPrecision>(cpu_precision_int);
  int resolution_selection = cpu_data_type_property_->GetValue().GetInteger();
  if (resolution_selection == kNumResolutionOptions) {
    settings.image_width = custom_resolution_width_;
    settings.image_height = custom_resolution_height_;
  } else {
    settings.image_width = resolution_options[resolution_selection][0];
    settings.image_height = resolution_options[resolution_selection][1];
  }
  settings.max_iteration = iterations_property_->GetValue().GetInteger();
  settings.sample_rate = sample_rate_property_->GetValue().GetInteger();
  settings.thread_count = num_threads_property_->GetValue().GetInteger();
  settings.cell_size = cell_size_property_->GetValue().GetInteger();
  settings.center_real = center_real_property_->GetValueAsString();
  settings.center_imaginary = center_imaginary_property_->GetValueAsString();
  settings.plot_width = plot_width_property_->GetValueAsString();
  settings.plot_height = plot_height_property_->GetValueAsString();

  return settings;
}

void MandelPropertiesPanel::DisplayFractalSettings(FractalSettings settings) {
  fractal_type_property_->SetChoiceSelection(settings.fractal_type);
}

void MandelPropertiesPanel::OnPropertyChanging(wxPropertyGridEvent & event) {
  wxPGProperty * selected_property = GetSelection();

  event.Skip();
}

void MandelPropertiesPanel::OnDropDownSelected(wxCommandEvent & event) {
  wxPGProperty * selected_property = GetSelection();
  event.Skip();

  if (selected_property == resolution_property_) {
    wxString display_resolution;

    // if "custom" resolution is selected
    if (event.GetSelection() == kNumResolutionOptions) {
      CustomResolutionDialog *resolution_dialog = new CustomResolutionDialog();
      resolution_dialog->Show(true);
      int return_code = resolution_dialog->GetReturnCode();
      if (return_code == wxOK) {
        custom_resolution_height_ = resolution_dialog->GetHeight();
        custom_resolution_width_ = resolution_dialog->GetWidth();
        display_resolution = wxString::Format(wxT("%llux%llu"),
                                              custom_resolution_width_,
                                              custom_resolution_height_);
      } else {
        return;
      }
    } else {
      display_resolution = wxString::Format(wxT("%ix%i"),
      resolution_options[event.GetSelection()][0],
      resolution_options[event.GetSelection()][1]);
    }

    resolution_property_->SetValueFromString(display_resolution);
  }
}

void MandelPropertiesPanel::OnKey(wxKeyEvent & event) {
  wxPGProperty * selected_property = GetSelection();

  if ((selected_property == resolution_property_) ||
      (selected_property == mouse_real_property_) ||
      (selected_property == mouse_imaginary_property_)) {
    cout << "Ignoring Character Input" << endl;
  } else {
    event.Skip();
  }
}

MandelPropertiesPanel::~MandelPropertiesPanel() {
}


wxBEGIN_EVENT_TABLE(MandelPropertiesPanel, wxPropertyGrid)
EVT_PG_CHANGED(wxID_ANY, MandelPropertiesPanel::OnPropertyChanged)
EVT_PG_CHANGING(wxID_ANY, MandelPropertiesPanel::OnPropertyChanging)
EVT_COMBOBOX(wxID_ANY, MandelPropertiesPanel::OnDropDownSelected)
EVT_PG_DOUBLE_CLICK(wxID_ANY, MandelPropertiesPanel::OnPropertyDoubleClick)
EVT_CHAR_HOOK(MandelPropertiesPanel::OnKey)
wxEND_EVENT_TABLE()
