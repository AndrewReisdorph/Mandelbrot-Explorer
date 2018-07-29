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

#ifndef INCLUDE_WAVES_COLOR_MAPPER_H_
#define INCLUDE_WAVES_COLOR_MAPPER_H_

#include "./color_mapper.h"

typedef struct Wave {
  double amplitude;
  double period;
  double phase;
} Wave;

class WavesColorMapper : public ColorMapper {
 public:
  WavesColorMapper();
  void Configure(Wave red_wave, Wave green_wave, Wave blue_wave,
                 double angle_step);
  Color GetColor(uint64_t iteration, double magnitude, bool loglog);
  Color GetColor(double angle);
  void GetWaves(Wave *red, Wave *green, Wave *blue);

 private:
  Wave red_wave_;
  Wave green_wave_;
  Wave blue_wave_;
  double angle_step_;
};

#endif  // INCLUDE_WAVES_COLOR_MAPPER_H_
