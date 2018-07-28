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

#ifndef WAVES_PALETTE_PANEL_H
#define WAVES_PALETTE_PANEL_H

#include <wx/dcbuffer.h>
#include <wx/wx.h>

#include "wave_control.h"
#include "waves_color_mapper.h"

class ChannelGraphPanel : public wxPanel {
 public:
  WavesColorMapper * color_mapper_;
  ChannelGraphPanel(wxWindow * parent, WavesColorMapper *color_mapper);

 private:
  void OnPaint(wxPaintEvent& event);

  wxDECLARE_EVENT_TABLE();
};

class ColorDisplayPanel : public wxPanel {
public:
  WavesColorMapper * color_mapper_;
  ColorDisplayPanel(wxWindow * parent, WavesColorMapper *color_mapper);

private:
  
  void OnPaint(wxPaintEvent& event);

  wxDECLARE_EVENT_TABLE();
};

class WavesPalettePanel : public wxPanel {
 public:
  WavesPalettePanel(wxWindow * parent);
  ~WavesPalettePanel();

 private:
  WaveControl * red_channel_control_;
  WaveControl * green_channel_control_;
  WaveControl * blue_channel_control_;
  ChannelGraphPanel *channel_graph_panel_;
  ColorDisplayPanel *color_display_panel_;
  WavesColorMapper *color_mapper_;

  void InitializeUI();
  void PaintChannelGraph(wxPaintEvent& event);
  void PaintColorDisplay(wxPaintEvent& event);
  void OnWaveChange(wxCommandEvent &event);

  wxDECLARE_EVENT_TABLE();
};

#endif