#pragma once
#include <wx/wx.h>
#include <mpir.h>
#include <chrono>
#include <string>
#include <sstream>

#define DEFAULT_MANDEL_WIDTH  854
#define DEFAULT_MANDEL_HEIGHT 480
#define INITIAL_PLOT_HEIGHT     4

#define M_Sleep100ms() std::this_thread::sleep_for(std::chrono::milliseconds(100));

enum ColorModes
{
	LOGLOG,
	CONTINUOUS,
	PROPORTIONAL
};

typedef struct RenderSettings
{
	int Iterations;
	int Gradations;
	bool Smoothing;
	int SmoothingThreshold;
	int SuperSamplingRate;
	int NumThreads;
	int Red;
	int Green;
	int Blue;
	ColorModes ColorMode;
	mpf_t PlotHeight;
	mpf_t PlotWidth;

} RenderSettings;

typedef struct Wave
{
	double amplitude;
	double phase;
	double period;
} Wave;

class MandelbrotExplorerApp : public wxApp
{
public:
	bool OnInit();
};

enum
{
	BUTTON_Render = wxID_HIGHEST + 1, // declares an id which will be used to call our button
	ID_ColorModeChoice,
	ID_ColorSliderRed,
	ID_ColorSliderGreen,
	ID_ColorSliderBlue,
	ID_SaveImage,
	ID_Quit,
	ID_RenderVideo
};

std::string DurationToString(std::chrono::duration<double> Duration);