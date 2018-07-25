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

#ifndef WAVE_CONTROL_H
#define WAVE_CONTROL_H

#include <math.h>

#include <wx/wx.h>

#include "waves_color_mapper.h"

wxDECLARE_EVENT(EVT_WAVE_CONTROL_CHANGE, wxCommandEvent);

class WaveControl : public wxPanel {

 public:
  WaveControl(wxWindow *parent, wxString ColorName);
  Wave GetWave();

 private:
  struct Wave wave_;

  wxString color_name_;
  wxSlider *amplitude_slider_;
  wxSlider *period_slider_;
  wxSlider *phase_slider_;
  wxTextCtrl *amplitude_text_ctrl_;
  wxTextCtrl *period_text_ctrl_;
  wxTextCtrl *phase_text_ctrl_;

  void InitializeUI();
  void UpdateSettings();
  void OnSliderChange(wxCommandEvent &event);
  void OnWaveChange(wxCommandEvent &event);

  wxDECLARE_EVENT_TABLE();
};

#endif