#pragma once

#include <cstdint>

#include <wx/string.h>

enum FractalType {
	kMandelBrot = 0,
	kJulia,
	kNumFractalTypes
};

static const char* fractal_type_names[kNumFractalTypes] = {"Mandelbrot", "Julia"};

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

static const char* kernel_type_names[kNumKernelTypes] = { "Mean" , "Median", "Mode", "Blur", "Bottom Sobel", "Emboss", "Identity"};

enum ColorMode {
	kWaves = 0,
	kGradient,
	kNumColorModes
};

static const char* color_mode_names[kNumColorModes] = {"Waves", "Gradient"};

enum HardwareType {
	kCPU = 0,
	kGPU,
	kNumHardwareTypes
};

static const char* hardware_type_names[kNumHardwareTypes] = {"CPU", "GPU"};

enum CpuPrecision {
	kFloat = 0,
	kDouble,
	kLongDouble,
	kMultiple,
	kNumCpuPrecions
};

static const char* cpu_precision_names[kNumCpuPrecions] = {"float", "double", "long double", "multiple"};

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