#// Copyright 2018 Andrew Reisdorph
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

#include <ctime>

#include "waves_palette_panel.h"

using namespace std;

WavesPalettePanel::WavesPalettePanel(wxWindow * parent) : wxPanel(parent) {
  color_mapper_ = new WavesColorMapper();
  InitializeUI();
}

void WavesPalettePanel::InitializeUI() {
  wxBoxSizer *main_v_sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *channel_settings_h_sizer = new wxBoxSizer(wxHORIZONTAL);
  red_channel_control_ = new WaveControl(this, "Red");
  green_channel_control_ = new WaveControl(this, "Green");
  blue_channel_control_ = new WaveControl(this, "Blue");

  channel_graph_panel_ = new ChannelGraphPanel(this, color_mapper_);
  channel_graph_panel_->SetBackgroundStyle(wxBG_STYLE_PAINT);
  channel_graph_panel_->SetMinSize(wxSize(-1, 75));
  channel_graph_panel_->SetWindowStyle(wxBORDER_SUNKEN);
  color_display_panel_ = new ColorDisplayPanel(this, color_mapper_);
  color_display_panel_->SetBackgroundStyle(wxBG_STYLE_PAINT);
  color_display_panel_->SetMinSize(wxSize(-1, 75));
  color_display_panel_->SetWindowStyle(wxBORDER_SUNKEN);

  channel_settings_h_sizer->Add(red_channel_control_,
    wxSizerFlags().Expand().Proportion(1));
  channel_settings_h_sizer->Add(green_channel_control_,
                                wxSizerFlags().Expand().Proportion(1));
  channel_settings_h_sizer->Add(blue_channel_control_,
                                wxSizerFlags().Expand().Proportion(1));

  main_v_sizer->Add(channel_settings_h_sizer,
    wxSizerFlags().Expand().Proportion(1));
  main_v_sizer->Add(channel_graph_panel_, wxSizerFlags().Expand());
  main_v_sizer->Add(color_display_panel_, wxSizerFlags().Expand());

  SetSizer(main_v_sizer);

  main_v_sizer->Fit(this);
}

void WavesPalettePanel::OnWaveChange(wxCommandEvent &event) {
  color_mapper_->Configure(
      red_channel_control_->GetWave(),
      green_channel_control_->GetWave(),
      blue_channel_control_->GetWave(),
      0.1);
  channel_graph_panel_->Refresh();
  channel_graph_panel_->Update();
  color_display_panel_->Refresh();
  color_display_panel_->Update();
}

WavesPalettePanel::~WavesPalettePanel() {
  delete color_mapper_;
}

ChannelGraphPanel::ChannelGraphPanel(
    wxWindow *parent,
    WavesColorMapper *color_mapper) : wxPanel(parent) {
  color_mapper_ = color_mapper;
}

void ChannelGraphPanel::OnPaint(wxPaintEvent& event) {

  wxAutoBufferedPaintDC dc(this);
  wxBrush background_brush = wxBrush(wxColour(0, 12, 22, 255),
                                     wxBRUSHSTYLE_SOLID);
  dc.SetBrush(background_brush);
  wxSize panel_size = GetSize();
  int height = panel_size.GetHeight();
  int width = panel_size.GetWidth();

  // Draw background color
  dc.DrawRectangle(wxRect(0,0, height, width));

  // Draw iteration colors
  double angle = 0.0;
  Color line_color;
  for (int column_iter = 0; column_iter < width; column_iter++)
  {
    line_color = color_mapper_->GetColor(angle);

    dc.SetPen(wxPen(wxColour(line_color.red, line_color.green,
                             line_color.blue)));
    dc.DrawLine(wxPoint(column_iter, 0), wxPoint(column_iter, height));

    angle += 0.01;
  }
}

ColorDisplayPanel::ColorDisplayPanel(
  wxWindow *parent,
  WavesColorMapper *color_mapper) : wxPanel(parent) {
  color_mapper_ = color_mapper;
}

void ColorDisplayPanel::OnPaint(wxPaintEvent& event) {

  wxAutoBufferedPaintDC dc(this);
  wxBrush background_brush = wxBrush(wxColour(29, 30, 30, 255),
                                     wxBRUSHSTYLE_SOLID);
  dc.SetBrush(background_brush);

  int border_height = wxSystemSettings::GetMetric(wxSYS_EDGE_Y);
  wxSize panel_size = GetSize();
  int height = panel_size.GetHeight();
  int draw_area_height = height - (2 * border_height);
  int width = panel_size.GetWidth();

  // Draw background color
  dc.DrawRectangle(wxRect(0, 0, width, height));

  // Draw channel waves
  Wave red_wave;
  Wave green_wave;
  Wave blue_wave;
  color_mapper_->GetWaves(&red_wave, &green_wave, &blue_wave);

  double angle = 0.0;
  double angle_delta = 0.01;

  double amplitude_scale = draw_area_height / 2.0;

  double next_red_angle;
  double next_red_value;
  double next_green_angle;
  double next_green_value;
  double next_blue_angle;
  double next_blue_value;

  double red_amplitude = (amplitude_scale - 1) * red_wave.amplitude;
  double green_amplitude = (amplitude_scale - 1) * green_wave.amplitude;
  double blue_amplitude = (amplitude_scale - 1) * blue_wave.amplitude;

  double red_angle_delta = red_wave.period * angle_delta;
  double green_angle_delta = green_wave.period * angle_delta;
  double blue_angle_delta = blue_wave.period * angle_delta;

  double last_red_angle = red_wave.period * angle + red_wave.phase;
  double last_red_value = amplitude_scale +
                          red_amplitude * sin(last_red_angle);
  double last_green_angle = green_wave.period * angle + green_wave.phase;
  double last_green_value = amplitude_scale +
                            green_amplitude * sin(last_green_angle);
  double last_blue_angle = blue_wave.period * angle + blue_wave.phase;
  double last_blue_value = amplitude_scale +
                           blue_amplitude * sin(last_blue_angle);

  // Because of the overhead of calling dc.SetPen, it is faster to draw each
  // wave individually. 

  angle = 0.0;
  dc.SetPen(wxPen(wxColour(242,19,19),2));
  for (int column_iter = 0; column_iter < width; column_iter++) {
    next_red_value = amplitude_scale +
                     red_amplitude * sin(angle + red_wave.phase);
    dc.DrawLine(column_iter, last_red_value,
                column_iter + 1, next_red_value);
    last_red_value = next_red_value;
    angle += red_angle_delta;
  }

  angle = 0.0;
  dc.SetPen(wxPen(wxColour(0, 219, 3), 2));
  for (int column_iter = 0; column_iter < width; column_iter++) {
    next_green_value = amplitude_scale +
                       green_amplitude * sin(angle + green_wave.phase);
    dc.DrawLine(column_iter, last_green_value,
                column_iter + 1, next_green_value);

    last_green_value = next_green_value;
    angle += green_angle_delta;
  }

  angle = 0.0;
  dc.SetPen(wxPen(wxColour(0, 84, 219), 2));
  for (int column_iter = 0; column_iter < width; column_iter++) {
    next_blue_value = amplitude_scale +
                      blue_amplitude * sin(angle + blue_wave.phase);
    dc.DrawLine(column_iter, last_blue_value,
                column_iter + 1, next_blue_value);

    last_blue_value = next_blue_value;
    angle += blue_angle_delta;
  }
}

wxBEGIN_EVENT_TABLE(ChannelGraphPanel, wxPanel)
EVT_PAINT(ChannelGraphPanel::OnPaint)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(ColorDisplayPanel, wxPanel)
EVT_PAINT(ColorDisplayPanel::OnPaint)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(WavesPalettePanel, wxPanel)
EVT_COMMAND(wxID_ANY, EVT_WAVE_CONTROL_CHANGE, WavesPalettePanel::OnWaveChange)
wxEND_EVENT_TABLE()