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

#include "wave_control.h"

wxDEFINE_EVENT(EVT_WAVE_CONTROL_CHANGE, wxCommandEvent);

WaveControl::WaveControl(wxWindow * parent,
                         wxString ColorName) : wxPanel(parent) {
  color_name_ = ColorName;

  InitializeUI();
  UpdateSettings();
}

void WaveControl::InitializeUI() {
  SetWindowStyle(wxBORDER_SUNKEN);

  wxBoxSizer *main_h_sizer = new wxBoxSizer(wxHORIZONTAL);
  
  wxFlexGridSizer *slider_panel_grid_sizer = new wxFlexGridSizer(3, 3, 5);
  slider_panel_grid_sizer->AddGrowableCol(2, 1);

  wxStaticText *amplitude_text = new wxStaticText(this, wxID_ANY,
                                                  "Amplitude:");
  amplitude_text_ctrl_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                        wxDefaultPosition, wxSize(60, -1));
  amplitude_slider_ = new wxSlider(this, wxID_ANY, 1000, 0, 1000);
  slider_panel_grid_sizer->Add(amplitude_text);
  slider_panel_grid_sizer->Add(amplitude_text_ctrl_);
  slider_panel_grid_sizer->Add(amplitude_slider_,
                               wxSizerFlags().Expand().Proportion(1));

  wxStaticText *period_text = new wxStaticText(this, wxID_ANY, "Period:");
  period_text_ctrl_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                     wxDefaultPosition, wxSize(60, -1));
  period_slider_ = new wxSlider(this, wxID_ANY, 628, 0, 1000);
  slider_panel_grid_sizer->Add(period_text);
  slider_panel_grid_sizer->Add(period_text_ctrl_);
  slider_panel_grid_sizer->Add(period_slider_,
                               wxSizerFlags().Expand().Proportion(1));

  wxStaticText *phase_text = new wxStaticText(this, wxID_ANY, "Phase:");
  phase_text_ctrl_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                    wxDefaultPosition, wxSize(60, -1));
  phase_slider_ = new wxSlider(this, wxID_ANY, rand() % 1000, 0, 1000);
  slider_panel_grid_sizer->Add(phase_text);
  slider_panel_grid_sizer->Add(phase_text_ctrl_);
  slider_panel_grid_sizer->Add(phase_slider_,
                               wxSizerFlags().Expand().Proportion(1));

  wxStaticText *color_name_text = new wxStaticText(this, wxID_ANY,
                                                   color_name_);
  wxFont color_name_font = color_name_text->GetFont();
  color_name_font.SetWeight(wxFONTWEIGHT_BOLD);
  color_name_text->SetFont(color_name_font);

  main_h_sizer->Add(color_name_text,
                    wxSizerFlags().CenterVertical().Border(wxLEFT | wxRIGHT));
  main_h_sizer->Add(slider_panel_grid_sizer,
    wxSizerFlags().Border(wxTOP|wxBOTTOM).Expand().Proportion(1));

  SetSizer(main_h_sizer);

  main_h_sizer->Fit(this);
}

void WaveControl::UpdateSettings()
{
  wave_.period = period_slider_->GetValue() / 100.0;
  wave_.amplitude = (amplitude_slider_->GetValue() / 500.0) - 1.0;
  wave_.phase = (phase_slider_->GetValue() / 500.0) * M_PI;

  period_text_ctrl_->SetValue(wxString::Format(wxT("%f"), wave_.period));
  amplitude_text_ctrl_->SetValue(wxString::Format(wxT("%f"), wave_.amplitude));
  phase_text_ctrl_->SetValue(wxString::Format(wxT("%f"), wave_.phase));

  wxCommandEvent event(EVT_WAVE_CONTROL_CHANGE);
  wxPostEvent(m_parent, event);
}

void WaveControl::OnSliderChange(wxCommandEvent & event)
{
  UpdateSettings();
  event.Skip();
}

Wave WaveControl::GetWave()
{
  return wave_;
}

BEGIN_EVENT_TABLE(WaveControl, wxPanel)
EVT_SLIDER(wxID_ANY, WaveControl::OnSliderChange)
wxEND_EVENT_TABLE()