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
#include <cstdlib>

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
  wxBrush background_brush = wxBrush(wxColour(0, 12, 22, 255), wxBRUSHSTYLE_SOLID);
  dc.SetBrush(background_brush);

  int border_height = wxSystemSettings::GetMetric(wxSYS_EDGE_Y);
  wxSize panel_size = GetSize();
  int height = panel_size.GetHeight();
  int draw_area_height = height - 2 * border_height;
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
  clock_t begin = clock();
  dc.SetPen(wxPen(wxColour(255, 0, 0), 1));
  double red_angle;
  double red_value;
  double red_distance;
  double green_angle;
  double green_value;
  double green_distance;
  double blue_angle;
  double blue_value;
  double blue_distance;
  double row_y_value;
  double y_delta = 2.0 / (double)(draw_area_height - 4);
  bool red_most_prominent;
  bool green_most_prominent;
  bool blue_most_prominent;

  for (int column_iter = 0; column_iter < width; column_iter++) {
    red_angle = red_wave.period * angle + red_wave.phase;
    red_value = red_wave.amplitude * sin(red_angle);
    green_angle = green_wave.period * angle + green_wave.phase;
    green_value = green_wave.amplitude * sin(green_angle);
    blue_angle = blue_wave.period * angle + blue_wave.phase;
    blue_value = blue_wave.amplitude * sin(blue_angle);
    row_y_value = 1 + y_delta;

    for (int row_iter = 0; row_iter < draw_area_height; row_iter++) {
      red_distance = fabs(red_value - row_y_value);
      green_distance = fabs(green_value - row_y_value);
      blue_distance = fabs(blue_value - row_y_value);
      row_y_value -= y_delta;

      if (red_distance < green_distance && red_distance < blue_distance) {
        red_most_prominent = true;
        green_most_prominent = false;
        blue_most_prominent = false;
      } else if (green_distance < red_distance && green_distance < blue_distance) {
        red_most_prominent = false;
        green_most_prominent = true;
        blue_most_prominent = false;
      } else {
        red_most_prominent = false;
        green_most_prominent = false;
        blue_most_prominent = true;
      }

      if (red_distance < 0.065) {
        if (red_distance < 0.015) {
          dc.SetPen(wxPen(wxColour(255, 0, 0), 1));
        } else if (red_distance < 0.025) {
          dc.SetPen(wxPen(wxColour(248, 0, 0), 1));
        } else if (red_distance < 0.045) {
          dc.SetPen(wxPen(wxColour(146, 4, 8), 1));
        } else {
          dc.SetPen(wxPen(wxColour(58, 9, 16), 1));
        }
        dc.DrawPoint(column_iter, row_iter);
        continue;
      }

     if (green_distance < 0.065) {
        if (green_distance < 0.015) {
          dc.SetPen(wxPen(wxColour(0, 255, 0), 1));
        } else if (green_distance < 0.025) {
          dc.SetPen(wxPen(wxColour(0, 247, 8), 1));
        } else if (green_distance < 0.045) {
          dc.SetPen(wxPen(wxColour(0, 158, 13), 1));
        } else {
          dc.SetPen(wxPen(wxColour(0, 48, 19), 1));
        }
        dc.DrawPoint(column_iter, row_iter);
        continue;
      }

     if (blue_distance < 0.065) {
       if (blue_distance < 0.015) {
         dc.SetPen(wxPen(wxColour(0, 0, 255), 1));
       }
       else if (blue_distance < 0.025) {
         dc.SetPen(wxPen(wxColour(44, 54, 247), 1));
       }
       else if (blue_distance < 0.045) {
         dc.SetPen(wxPen(wxColour(30, 40, 176), 1));
       }
       else {
         dc.SetPen(wxPen(wxColour(11, 23, 82), 1));
       }
       dc.DrawPoint(column_iter, row_iter);
       continue;
     }

      
    }

    angle += angle_delta;
  }
  clock_t end = clock();
  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  cout << "time: " << elapsed_secs << endl;
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