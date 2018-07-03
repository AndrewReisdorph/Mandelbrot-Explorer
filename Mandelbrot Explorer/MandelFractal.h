#pragma once
#include <thread>
#include <mutex> 
#include <chrono>
#include <vector>

#include <atomic>
#include <mpir.h>
#include "MandelView.h"
#include "MandelControlPanel.h"

struct SelectionBoxPoint;
struct RenderSettings;
class MandelViewPanel;

typedef struct Color
{
	int Red;
	int Green;
	int Blue;
} Color;

typedef struct IterationData
{
	int Iteration;
	double Magnitude;
} IterationData;

class MandelFractal
{
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_renderStartTime;
	std::mutex m_ThreadMutex;
	std::atomic<std::uint32_t> m_FinishedCells;
	std::atomic<std::uint32_t> m_MaxIterCells;
	std::atomic<std::uint32_t> m_ThreadCount;
	MandelControlPanel *m_ControlPanel;
	MandelViewPanel *m_RenderArea;
	int m_TotalThreads;
	bool m_Paused;
	bool m_abort;
	mpf_t m_CenterX;
	mpf_t m_CenterI;
	mpf_t m_PlotHeight;
	mpf_t m_PlotWidth;
	int m_MaxIterations;
	int m_SuperSampleRate;
	int m_CurrentThreadCount;
	Color *m_ColorMap;
	RenderSettings m_LastRenderSettings;
	int m_PixelHeight;
	int m_PixelWidth;

	Color InterpolateColor(Color ColorA, Color ColorB, double distance);
	void FillPixel(int ImageBufferIndex, int Iteration, double Magnitude, unsigned char*ImageData);
	void InitColorPalette();
	void ProgressWorker();
	void GetCoordinateMagnitude(mpf_t &x, mpf_t &i, mpf_t &Magnitude);
	bool IsTrivialCoordinate(mpf_t &x, mpf_t &i);
	IterationData GetBestIteration(std::vector<IterationData> &iterations, int statistic);

public:
	MandelFractal(MandelControlPanel *ControlPanel, MandelViewPanel *RenderArea, int PixelHeight, int PixelWidth);
	bool RenderFractal(unsigned char * ImageData);
	bool RenderFractal(SelectionBoxPoint TopLeft, SelectionBoxPoint BottomRight, unsigned char * ImageData);
	void PanelCoordinateToComplex(int panel_x, int panel_y, mpf_t &x, mpf_t &i);
	void FractalWorker(unsigned char*ImageData, mpf_t &LeftMostX, mpf_t &StartI, mpf_t &PixelCoordinateDelta, mpf_t &SuperSampleCoordinateDelta);
	void PauseRender();
	void ResumeRender();
	void Zoom(double percent);
	void SetResolution(int Width, int Height);
	void GetPlotDimensions(mpf_t &height, mpf_t &width);
	void Abort();

	~MandelFractal();
};