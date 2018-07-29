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

#ifndef INCLUDE_FRACTAL_SETTINGS_H_
#define INCLUDE_FRACTAL_SETTINGS_H_

#include <wx/string.h>

#include <cstdint>


enum FractalType {
  kMandelBrot = 0,
  kJulia,
  kNumFractalTypes
};

static const char* fractal_type_names[kNumFractalTypes] = {"Mandelbrot",
                                                           "Julia"};

enum KernelType {
  kMean = 0,
  kMedian,
  kMode,
  kBlur,
  kBottomSobel,
  kEmboss,
  kIdentity,
  kNumKernelTypes
};

static const char* kernel_type_names[kNumKernelTypes] = { "Mean" ,
                                                          "Median",
                                                          "Mode",
                                                          "Blur",
                                                          "Bottom Sobel",
                                                          "Emboss",
                                                          "Identity"};

enum ColorMode {
  kWaves = 0,
  kGradient,
  kNumColorModes
};

static const char* color_mode_names[kNumColorModes] = {"Waves",
                                                       "Gradient"};

enum HardwareType {
  kCPU = 0,
  kGPU,
  kNumHardwareTypes
};

static const char* hardware_type_names[kNumHardwareTypes] = {"CPU",
                                                             "GPU"};

enum CpuPrecision {
  kFloat = 0,
  kDouble,
  kLongDouble,
  kMultiple,
  kNumCpuPrecions
};

static const char* cpu_precision_names[kNumCpuPrecions] = {"float",
                                                           "double",
                                                           "long double",
                                                           "multiple"};

typedef struct FractalSettings {
  FractalType fractal_type;
  KernelType kernel_type;
  ColorMode color_mode;
  HardwareType hardware;
  CpuPrecision cpu_precision;
  uint64_t multiple_precision;
  uint64_t image_width;
  uint64_t image_height;
  uint64_t max_iteration;
  uint64_t sample_rate;
  uint64_t thread_count;
  uint64_t cell_size;
  wxString center_real;
  wxString center_imaginary;
  wxString plot_width;
  wxString plot_height;
} FractalSettings;

#endif  // INCLUDE_FRACTAL_SETTINGS_H_
