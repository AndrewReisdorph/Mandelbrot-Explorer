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

#include <math.h>

#include "../include/waves_color_mapper.h"

WavesColorMapper::WavesColorMapper() {
}

void WavesColorMapper::Configure(Wave red_wave, Wave green_wave,
                                 Wave blue_wave, double angle_step) {
  red_wave_ = red_wave;
  green_wave_ = green_wave;
  blue_wave_ = blue_wave;
  angle_step_ = angle_step;
}

Color WavesColorMapper::GetColor(uint64_t iteration, double magnitude = 0.0,
                                 bool loglog = false) {
  return { 0, 0, 0 };
}

Color WavesColorMapper::GetColor(double angle) {
  double half_channel_max = (255.0 / 2.0);
  double red_angle = red_wave_.period * angle + red_wave_.phase;
  double red_value = 255 - (half_channel_max -
    half_channel_max * red_wave_.amplitude * sin(red_angle));

  double green_angle = green_wave_.period * angle + green_wave_.phase;
  double green_value = 255 - (half_channel_max -
    half_channel_max * green_wave_.amplitude * sin(green_angle));

  double blue_angle = blue_wave_.period * angle + blue_wave_.phase;
  double blue_value = 255 - (half_channel_max -
    half_channel_max * blue_wave_.amplitude * sin(blue_angle));

  return { (uint8_t)red_value, (uint8_t)green_value, (uint8_t)blue_value };
}

void WavesColorMapper::GetWaves(Wave * red, Wave * green, Wave * blue) {
  *red = red_wave_;
  *green = green_wave_;
  *blue = blue_wave_;
}
