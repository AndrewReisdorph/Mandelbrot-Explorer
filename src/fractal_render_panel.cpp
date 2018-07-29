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

#include <wx/dcbuffer.h>

#include "../include/fractal_render_panel.h"

using std::cout;
using std::endl;

FractalRenderPanel::FractalRenderPanel(wxWindow * parent)
    :wxScrolledWindow(parent) {
  SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void FractalRenderPanel::OnPaint(wxPaintEvent & event) {
  wxAutoBufferedPaintDC dc(this);
  dc.SetBrush(wxBrush(wxColour(0, 0, 0, 0), wxBRUSHSTYLE_SOLID));
  dc.Clear();

  // Draw background color
  wxSize panelSize = GetSize();
  dc.SetBrush(wxBrush(wxColour(29, 30, 30, 255), wxBRUSHSTYLE_SOLID));
  dc.DrawRectangle(wxRect(0, 0, panelSize.GetWidth(), panelSize.GetHeight()));
}

void FractalRenderPanel::OnResize(wxSizeEvent & event) {
  Refresh();
}

wxBEGIN_EVENT_TABLE(FractalRenderPanel, wxScrolledWindow)
EVT_PAINT(FractalRenderPanel::OnPaint)
EVT_SIZE(FractalRenderPanel::OnResize)
wxEND_EVENT_TABLE()
