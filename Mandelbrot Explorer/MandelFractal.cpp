#include <thread>
#include <cmath>
#include <math.h>
#include <algorithm>
#include "MandelFractal.h"
#include "main.h"
#ifdef __linux__
#elif __APPLE__
#else
#include "cudaWorker.cuh"
#include <cuda.h>
extern __global__ void CUDAFractalWorker(mpf_t leftX, mpf_t topI, mpf_t pixelCoordinateDelta, mpf_t SuperSampleCoordinateDelta, int frameWidth, int frameHeight, int MaxIterations, int SampleRate, int Precision, uint32_t *HostIterations, double *HostMagnitudes);
#endif

bool flag = false;

using namespace std;

MandelFractal::MandelFractal(MandelControlPanel *ControlPanel, MandelViewPanel *RenderArea, int PixelHeight, int PixelWidth)
{
	m_Paused = false;
	m_abort = false;
	m_ControlPanel = ControlPanel;
	m_RenderArea = RenderArea;
	m_LastRenderSettings = { 0 };
	mpf_init(m_CenterX);
	mpf_init(m_CenterI);
	mpf_init(m_PlotHeight);
	mpf_init(m_PlotWidth);

	//mpf_set_ui(m_CenterX, 0);
	//mpf_set_ui(m_CenterI, 0);
	//mpf_set_str(m_CenterX, "0.1974552436448450147288140327668129344782336", 10);
	//mpf_set_str(m_CenterI, "-1.287827964672264194185052832825214407540736", 10);
	mpf_set_str(m_CenterX, "-1.287827964672264194185052832825214407540736", 10);
	mpf_set_str(m_CenterI, "0.1974552436448450147288140327668129344782336", 10);

	mpf_set_ui(m_PlotHeight, INITIAL_PLOT_HEIGHT);
	SetResolution(PixelWidth, PixelHeight);

	/*
	r 0.393402777777777777777777777777777777777777777777777777777777777777777777777777777777777778
	i 0.237326388888888888888888888888888888888888888888888888888888888888888888888888888888888889
	w 0.0123553240740740740740740740740740740740740740740740740740740740740740740740740740740740741
	h 0.00694444444444444444444444444444444444444444444444444444444444444444444444444444444444444444
	205
	*/
	
	m_ColorMap = NULL;

	m_ControlPanel->SetCenterInfo(m_CenterX, m_CenterI);
}

void MandelFractal::SetResolution(int Width, int Height)
{
	m_PixelHeight = Height;
	m_PixelWidth = Width;

	mpf_t PlotHeightWidthRatio;
	mpf_init(PlotHeightWidthRatio);
	mpf_set_ui(PlotHeightWidthRatio, m_PixelWidth);
	mpf_div_ui(PlotHeightWidthRatio, PlotHeightWidthRatio, m_PixelHeight);
	mpf_mul(m_PlotWidth, PlotHeightWidthRatio, m_PlotHeight);

	m_ControlPanel->SetPlotDimensionInfo(m_PlotWidth, m_PlotHeight);
	m_ControlPanel->SetPlotDimensionInfo(m_PlotWidth, m_PlotHeight);

	mpf_clear(PlotHeightWidthRatio);

}

void MandelFractal::Zoom(double percent)
{
	mpf_t ZoomFactor;
	mpf_init(ZoomFactor);

	mpf_set_d(ZoomFactor, percent);
	mpf_mul(m_PlotHeight, m_PlotHeight, ZoomFactor);
	mpf_mul(m_PlotWidth, m_PlotWidth, ZoomFactor);

	mpf_clear(ZoomFactor);
}

void MandelFractal::PauseRender()
{
	m_Paused = true;
}

void MandelFractal::ResumeRender()
{
	m_Paused = false;
}

void MandelFractal::ProgressWorker()
{
	using namespace std::chrono;
	int CompletedRows;
	int NumCells = m_PixelHeight * m_PixelWidth;
	int PercentComplete = 0;
	bool PreviouslyPaused = false;
	time_point<high_resolution_clock> ReferenceTime = m_renderStartTime;
	duration<double> NonContinuousRenderTime = duration<double>(0);
	duration<double> TimeElapsed;
	duration<double> TimeRemaining;
	duration<double> TimePerRow;

	while (true)
	{
		TimeElapsed = (std::chrono::high_resolution_clock::now() - ReferenceTime) + NonContinuousRenderTime;
		TimeRemaining = std::chrono::duration<double>(0);

		if (m_FinishedCells > 100)
		{
			CompletedRows = (m_FinishedCells / m_PixelWidth);
			TimePerRow = TimeElapsed / CompletedRows;
			TimeRemaining = (m_PixelHeight - CompletedRows) * TimePerRow;
		}
		m_ControlPanel->UpdateRenderTimeInfo(TimeElapsed, TimeRemaining);

		PercentComplete = std::min(100, int(100.0 * double(m_FinishedCells) / NumCells));
		m_ControlPanel->UpdateProgressBar(PercentComplete);
		m_RenderArea->Refresh();

		if (m_FinishedCells == NumCells)
		{
			break;
		}

		M_Sleep100ms();

		while (m_Paused && !m_abort)
		{
			if (!PreviouslyPaused)
			{
				PreviouslyPaused = true;
				NonContinuousRenderTime = TimeElapsed;
			}
			M_Sleep100ms();
		}

		if (PreviouslyPaused)
		{
			PreviouslyPaused = false;
			ReferenceTime = std::chrono::high_resolution_clock::now();
		}

		if (m_abort)
		{
			break;
		}
	}


}

bool MandelFractal::RenderFractal(SelectionBoxPoint TopLeft, SelectionBoxPoint BottomRight, unsigned char * ImageData)
{
	mpf_t PixelCoordinateDelta;
	mpf_init(PixelCoordinateDelta);
	mpf_div_ui(PixelCoordinateDelta, m_PlotHeight, m_PixelHeight);
	mpf_t NewCenterX;
	mpf_init(NewCenterX);
	mpf_t NewCenterI;
	mpf_init(NewCenterI);
	mpf_t HalfPlotWidth;
	mpf_init(HalfPlotWidth);
	mpf_t HalfPlotHeight;
	mpf_init(HalfPlotHeight);
	mpf_t PlotHeightWidthRatio;
	mpf_init(PlotHeightWidthRatio);
	mpf_t NegativeOne;
	mpf_init(NegativeOne);

	mpf_set_ui(NegativeOne, 0);
	mpf_sub_ui(NegativeOne, NegativeOne, 1);
	

	int SelectionPixelWidth = abs(TopLeft.x - BottomRight.x);
	int SelectionPixelHeight = abs(TopLeft.y - BottomRight.y);
	int SelectionCenterX = ((TopLeft.x > BottomRight.x) ? TopLeft.x : BottomRight.x) - (SelectionPixelWidth / 2);
	int SelectionCenterI = ((TopLeft.y > BottomRight.y) ? TopLeft.y : BottomRight.y) - (SelectionPixelHeight / 2);
	mpf_mul_ui(HalfPlotWidth, PixelCoordinateDelta, (m_PixelWidth / 2));
	mpf_mul_ui(HalfPlotHeight, PixelCoordinateDelta, (m_PixelHeight / 2));

	// Set New Center Coordinates
	//real = (pixel_coordinate_value * mpmath.mpf(pixel[0])) - (pixel_coordinate_value*(self.mandel_width / mpmath.mpf(2))) + center_x
	mpf_mul_ui(NewCenterX, PixelCoordinateDelta, SelectionCenterX);
	mpf_sub(NewCenterX, NewCenterX, HalfPlotWidth);
	mpf_add(NewCenterX, NewCenterX, m_CenterX);

	//imaginary = (pixel_coordinate_value * mpmath.mpf(pixel[1]) * -1) + (pixel_coordinate_value*(self.mandel_height / mpmath.mpf(2))) + center_y
	mpf_mul_ui(NewCenterI, PixelCoordinateDelta, SelectionCenterI);
	mpf_mul(NewCenterI, NewCenterI, NegativeOne);
	mpf_add(NewCenterI, NewCenterI, HalfPlotHeight);
	mpf_add(NewCenterI, NewCenterI, m_CenterI);

	mpf_set(m_CenterX, NewCenterX);
	mpf_set(m_CenterI, NewCenterI);

	// Set New Plot Dimensions
	mpf_mul_ui(m_PlotHeight, PixelCoordinateDelta, SelectionPixelHeight);
	mpf_set_ui(PlotHeightWidthRatio, m_PixelWidth);
	mpf_div_ui(PlotHeightWidthRatio, PlotHeightWidthRatio, m_PixelHeight);
	mpf_mul(m_PlotWidth, PlotHeightWidthRatio, m_PlotHeight);

	mpf_clear(PixelCoordinateDelta);
	mpf_clear(NewCenterX);
	mpf_clear(NewCenterI);
	mpf_clear(HalfPlotHeight);
	mpf_clear(HalfPlotWidth);
	mpf_clear(PlotHeightWidthRatio);
	mpf_clear(NegativeOne);

	m_ControlPanel->SetCenterInfo(m_CenterX, m_CenterI);
	m_ControlPanel->SetPlotDimensionInfo(m_PlotWidth, m_PlotHeight);

	return this->RenderFractal(ImageData);
}

void MandelFractal::PanelCoordinateToComplex(int panel_x, int panel_y, mpf_t &x, mpf_t &i)
{
	mpf_t LeftMostX;
	mpf_t HalfWidth;
	mpf_t HalfHeight;
	mpf_t TopMostI;
	mpf_t PixelCoordinateDelta;
	mpf_t RealOffset;
	mpf_t ImaginaryOffset;

	mpf_init(ImaginaryOffset);
	mpf_init(RealOffset);
	mpf_init(HalfWidth);
	mpf_init(HalfHeight);
	mpf_init(LeftMostX);
	mpf_init(TopMostI);
	mpf_init(PixelCoordinateDelta);

	mpf_div_ui(HalfWidth, m_PlotWidth, 2);
	mpf_div_ui(HalfHeight, m_PlotHeight, 2);

	mpf_set(PixelCoordinateDelta, m_PlotHeight);
	mpf_div_ui(PixelCoordinateDelta, PixelCoordinateDelta, m_PixelHeight);

	mpf_set(TopMostI, m_CenterI);
	mpf_add(TopMostI, TopMostI, HalfHeight);

	mpf_set(LeftMostX, m_CenterX);
	mpf_sub(LeftMostX, LeftMostX, HalfWidth);

	mpf_mul_ui(RealOffset, PixelCoordinateDelta, panel_x);
	mpf_add(x, LeftMostX, RealOffset);

	mpf_mul_ui(ImaginaryOffset, PixelCoordinateDelta, panel_y);
	mpf_sub(i, TopMostI, ImaginaryOffset);

	// Clean Up
	mpf_clear( LeftMostX);
	mpf_clear(HalfWidth);
	mpf_clear(HalfHeight);
	mpf_clear(TopMostI);
	mpf_clear(PixelCoordinateDelta);
	mpf_clear(RealOffset);
	mpf_clear(ImaginaryOffset);
}



bool MandelFractal::RenderFractal(unsigned char * ImageData)
{
	RenderSettings settings = m_ControlPanel->GetRenderSettings();
	m_MaxIterations = settings.Iterations;
	m_TotalThreads = settings.NumThreads;
	m_SuperSampleRate = settings.SuperSamplingRate;
	vector<thread> Workers;

	bool aborted = false;
	int ImageDataIterator = 0;
	int CurrentIterations = 0;
	int MinIteration = 0;
	double MinIterationMagnitude;
	int CellsCompleted = 0;
	int VerticalSuperSamples = m_PixelHeight * m_SuperSampleRate;
	mpf_t CurrentX;
	mpf_init(CurrentX);
	mpf_t CurrentI;
	mpf_init(CurrentI);
	mpf_t SuperX;
	mpf_init(SuperX);
	mpf_t SuperI;
	mpf_init(SuperI);
	mpf_t TopMostI;
	mpf_init(TopMostI);
	mpf_t LeftMostX;
	mpf_init(LeftMostX);
	mpf_t HalfWidth;
	mpf_init(HalfWidth);
	mpf_t HalfHeight;
	mpf_init(HalfHeight);
	mpf_t cx;
	mpf_init(cx);
	mpf_t ci;
	mpf_init(ci);
	mpf_t zx;
	mpf_init(zx);
	mpf_t zi;
	mpf_init(zi);
	mpf_t Magnitude;
	mpf_init(Magnitude);
	mpf_t zx_squared;
	mpf_init(zx_squared);
	mpf_t zi_squared;
	mpf_init(zi_squared);
	mpf_t tempzx;
	mpf_init(tempzx);
	mpf_t tempzi;
	mpf_init(tempzi);
	mpf_t SuperSampleCoordinateDelta;
	mpf_init(SuperSampleCoordinateDelta);
	mpf_t PixelCoordinateDelta;
	mpf_init(PixelCoordinateDelta);


	mpf_set(SuperSampleCoordinateDelta, m_PlotHeight);
	mpf_div_ui(SuperSampleCoordinateDelta, SuperSampleCoordinateDelta, VerticalSuperSamples);

	mpf_set(PixelCoordinateDelta, m_PlotHeight);
	mpf_div_ui(PixelCoordinateDelta, PixelCoordinateDelta, m_PixelHeight);

	mpf_div_ui(HalfHeight, m_PlotHeight, 2);

	mpf_div_ui(HalfWidth, m_PlotWidth, 2);

	mpf_set(TopMostI, m_CenterI);
	mpf_add(TopMostI, TopMostI, HalfHeight);
	mpf_set(CurrentI, TopMostI);

	mpf_set(LeftMostX, m_CenterX);
	mpf_sub(LeftMostX, LeftMostX, HalfWidth);
	mpf_set(CurrentX, LeftMostX);

	InitColorPalette();

	cout << "RENDERING" << endl;

	m_renderStartTime = chrono::high_resolution_clock::now();
	m_MaxIterCells = 0;
	m_FinishedCells = 0;
	m_ThreadCount = 0;

	

	if (settings.UseGPU)
	{
#ifdef __linux__
#elif __APPLE__
#else
				uint32_t arrayCount = m_PixelWidth * m_PixelHeight;
		uint32_t *iterations = new uint32_t[arrayCount];
		double *magnitudes = new double[arrayCount];

		char buff[500];
		m_ControlPanel->GetHumanReadableMPFString(LeftMostX, buff);
		cout << "LeftX: " << buff << endl;
		m_ControlPanel->GetHumanReadableMPFString(TopMostI, buff);
		cout << "TopMostI: " << buff << endl;
		m_ControlPanel->GetHumanReadableMPFString(PixelCoordinateDelta, buff);
		cout << "PixelCoordinateDelta: " << buff << endl;
		m_ControlPanel->GetHumanReadableMPFString(SuperSampleCoordinateDelta, buff);
		cout << "SuperSampleCoordinateDelta: " << buff << endl;

		CUDAFractalWorker(LeftMostX, TopMostI, PixelCoordinateDelta, SuperSampleCoordinateDelta, m_PixelWidth, m_PixelHeight, m_MaxIterations, 1, 512, iterations, magnitudes);
		
		//for (uint32_t i = 0; i < arrayCount; i++)
		//{
		//	FillPixel(i*3, iterations[i], magnitudes[i], ImageData);
		//}

		delete magnitudes;
		delete iterations;
#endif
	}
	else
	{
		thread RenderUpdateThread(&MandelFractal::ProgressWorker, this);

		for (int ThreadIterator = 0; ThreadIterator < m_TotalThreads; ThreadIterator++)
		{
			Workers.emplace_back(thread([&](MandelFractal* f) {f->FractalWorker(ImageData, LeftMostX, TopMostI, PixelCoordinateDelta, SuperSampleCoordinateDelta); }, this));
		}

		for (int ThreadIterator = 0; ThreadIterator < m_TotalThreads; ThreadIterator++)
		{
			Workers[ThreadIterator].join();
		}

		RenderUpdateThread.join();
	}



	m_ControlPanel->OnRenderFinish(std::chrono::high_resolution_clock::now() - m_renderStartTime, m_abort);





	if (m_abort)
	{
		m_abort = false;
		aborted = true;
	}
	else
	{

		cout << fixed << endl << "Render Complete:" << endl << "\tTotal Time: " <<
			DurationToString(std::chrono::high_resolution_clock::now() - m_renderStartTime) << endl <<
			"\tSuper Sample Rate: " << m_SuperSampleRate << endl << "\tMax Iteration: " <<
			m_MaxIterations << endl << "\tMax Iteration Cells: " << m_MaxIterCells << endl << endl;

	}

	// Clean Up
	mpf_clear(CurrentX);
	mpf_clear(CurrentI);
	mpf_clear(SuperX);
	mpf_clear(SuperI);
	mpf_clear(TopMostI);
	mpf_clear(LeftMostX);
	mpf_clear(HalfWidth);
	mpf_clear(HalfHeight);
	mpf_clear(cx);
	mpf_clear(ci);
	mpf_clear(zx);
	mpf_clear(zi);
	mpf_clear(Magnitude);
	mpf_clear(zx_squared);
	mpf_clear(zi_squared);
	mpf_clear(tempzx);
	mpf_clear(tempzi);
	mpf_clear(SuperSampleCoordinateDelta);
	mpf_clear(PixelCoordinateDelta);

	return aborted;
}

void MandelFractal::InitColorPalette()
{
	RenderSettings settings = m_ControlPanel->GetRenderSettings();
	double TwoPi = M_PI * 2.0;
	double RGBFraction = (255.0 / 2.0);
	double GradationStep;
	double GradationsAngleStep;
	double RedOffset;
	double GreenOffset;
	double BlueOffset;
	int Gradation;
	int Red;
	int Green;
	int Blue;

	if (m_ColorMap != NULL)
	{
		delete m_ColorMap;
	}

	switch (settings.ColorMode)
	{
	case CONTINUOUS:
		m_ColorMap = new Color[settings.Gradations];
		GradationsAngleStep = TwoPi / double(settings.Gradations);
		Gradation = 0;
		RedOffset = (settings.Red / 255.0) * TwoPi;
		GreenOffset = (settings.Green / 255.0) * TwoPi;
		BlueOffset = (settings.Blue / 255.0) * TwoPi;

		for (int MapIter = 0; MapIter < settings.Gradations; MapIter++)
		{
			double BaseAngle = MapIter * GradationsAngleStep;
			Red = int((sin(RedOffset + BaseAngle) + 1)*RGBFraction);
			Green = int((sin(GreenOffset + BaseAngle) + 1)*RGBFraction);
			Blue = int((sin(BlueOffset + BaseAngle) + 1)*RGBFraction);
			m_ColorMap[MapIter] = {Red,Green,Blue};
		}

		break;
	}


}

void MandelFractal::FillPixel(int ImageBufferIndex, int Iteration, double Magnitude, unsigned char*ImageData)
{
	bool UsePartialIterationMethod = true;
	RenderSettings settings = m_ControlPanel->GetRenderSettings();

	if (ImageBufferIndex > m_PixelWidth * m_PixelHeight * 3)
	{
		std::cout << "SHITS FUCKED " << ImageBufferIndex << std::endl;
		return;
	}

	switch (settings.ColorMode)
	{
	case CONTINUOUS:
		Color IterationColor;
		if (UsePartialIterationMethod)
		{
			if (Iteration == m_MaxIterations)
			{
				IterationColor = { 0 };
			}
			else
			{
				double RGBFraction = 127.5;
				double a = Iteration + 2 - log(log(Magnitude)) / log(2);
				double GradationsAngleStep = 6.28 / double( 100 );
				double Gradation = 0;
				double RedOffset = ( settings.Red / 255.0 ) * 6.28;
				double GreenOffset = ( settings.Green / 255.0 ) * 6.28;
				double BlueOffset = ( settings.Blue / 255.0 ) * 6.28;
				double BaseAngle = a * GradationsAngleStep;
				IterationColor.Red = int( ( sin( RedOffset + BaseAngle ) + 1 ) * RGBFraction);
				IterationColor.Green = int( ( sin( GreenOffset + BaseAngle ) + 1 ) * RGBFraction);
				IterationColor.Blue = int( ( sin( BlueOffset + BaseAngle ) + 1 ) * RGBFraction);
			}
		}
		else
		{
			if (Iteration == m_MaxIterations)
			{
				IterationColor = { 0 };
			}
			else
			{
				IterationColor = m_ColorMap[Iteration%settings.Gradations];
			}

			
		}

		if (Iteration == -1)
		{
			IterationColor = { 0 };
			IterationColor.Red = 255;
		}

		ImageData[ImageBufferIndex++] = IterationColor.Red;
		ImageData[ImageBufferIndex++] = IterationColor.Green;
		ImageData[ImageBufferIndex] = IterationColor.Blue;
		break;
	}

}

Color MandelFractal::InterpolateColor(Color ColorA, Color ColorB, double distance)
{
	Color NewColor = { 0 };

	NewColor.Red = ((ColorA.Red > ColorB.Red) ? ColorB.Red : ColorA.Red ) + abs(ColorA.Red - ColorB.Red) * distance;
	NewColor.Green = ((ColorA.Green > ColorB.Green) ? ColorB.Green : ColorA.Green) + abs(ColorA.Green - ColorB.Green) * distance;
	NewColor.Blue = ((ColorA.Blue > ColorB.Blue) ? ColorB.Blue : ColorA.Blue) + abs(ColorA.Blue - ColorB.Blue) * distance;

	return NewColor;
}

void MandelFractal::GetCoordinateMagnitude(mpf_t &x, mpf_t &i, mpf_t &Magnitude)
{
	mpf_t x_squared;
	mpf_init(x_squared);
	mpf_t i_squared;
	mpf_init(i_squared);
	mpf_t tempx;
	mpf_init(tempx);
	mpf_t tempi;
	mpf_init(tempi);

	mpf_pow_ui(x_squared, x, 2);
	mpf_pow_ui(i_squared, i, 2);
	mpf_sub(tempx, x_squared, i_squared);
	mpf_mul(tempi, x, i);
	mpf_mul_ui(tempi, tempi, 2);

	mpf_pow_ui(x_squared, x, 2);
	mpf_pow_ui(i_squared, i, 2);
	mpf_add(Magnitude, x_squared, i_squared);
}

bool MandelFractal::IsTrivialCoordinate(mpf_t &x, mpf_t &i)
{
	bool isTrivial = false;

	// Check if coordinate is in disc at x=-1
	if (mpf_cmp_d(x, -1.25) >= 0 && mpf_cmp_d(x, -0.75) <= 0)
	{
		if (mpf_cmp_d(i, 0.25) <= 0 && mpf_cmp_d(i, -0.25) >= 0)
		{
			mpf_t xdist;
			mpf_t centerx;
			mpf_t ydist;
			mpf_t centery;
			mpf_init(xdist);
			mpf_init(centerx);
			mpf_set_d(centerx, -1);
			mpf_sub(xdist, centerx, x);
			mpf_mul(xdist, xdist, xdist);
			mpf_init(ydist);
			mpf_init(centery);
			mpf_set_ui(centery, 0);
			mpf_sub(ydist, centery, i);
			mpf_mul(ydist, ydist, ydist);
			mpf_add(ydist, xdist, ydist);
			// Get distance from center of disc (-1,0)
			if (mpf_cmp_d(ydist, 0.0625) <= 0)
			{
				isTrivial = true;
			}
			mpf_clear(xdist);
			mpf_clear(ydist);
			mpf_clear(centerx);
			mpf_clear(centery);
		}
	}
	else if (mpf_cmp_d(x, -0.75) >= 0 && mpf_cmp_d(x, 0.4) <= 0) // Check if point is in main cardioid
	{
		if (mpf_cmp_d(i, 0.75) <= 0 && mpf_cmp_d(i, -0.75) >= 0)
		{
			double d_x = mpf_get_d(x);
			double d_i = mpf_get_d(i);
		
			double q = (d_x - .25)*(d_x - .25) + (d_i * d_i);
			if ( (q*(q+(d_x - .25)))  < (.25 * d_i * d_i ) )
			{
				isTrivial = true;
			}

			/*double p = sqrt( (d_x - .25)*(d_x - .25) + d_i*d_i );
			if ( (d_x < (p - (2 * p * p) + .25)) && 
				 (((d_x + 1)*(d_x + 1) + d_i*d_i) < 0.0625) )
			{
				isTrivial = true;
			}*/

		}
	}

	return isTrivial;
}

#define MEAN 0
#define MODE 1
#define MEDIAN 2


IterationData MandelFractal::GetBestIteration(vector<IterationData> &iterations, int statistic)
{
	IterationData bestIteration;

	switch (statistic)
	{
	case MODE:
		int * iterationCounts = (int*)calloc(m_MaxIterations, sizeof(int));
		IterationData SmallestIteration = { m_MaxIterations, 0.0 };

		for (std::vector<IterationData>::iterator it = iterations.begin(); it != iterations.end(); ++it) {
			if ((*it).Iteration < SmallestIteration.Iteration)
			{
				SmallestIteration = *it;
			}
			iterationCounts[(*it).Iteration - 1]++;
		}

		int ModeIndex = 0;
		int HighestMode = 0;
		IterationData Mode;
		for (int i = 0; i < m_MaxIterations; i++)
		{
			if (iterationCounts[i] > HighestMode)
			{
				Mode.Iteration = i + 1;
				ModeIndex = i;
				HighestMode = iterationCounts[i];
			}
			
		}

		// Calculate average magnitude for the mode iterations
		double averageMag = 0;
		for (std::vector<IterationData>::iterator it = iterations.begin(); it != iterations.end(); ++it) {
			if (it->Iteration == Mode.Iteration)
			{
				averageMag += it->Magnitude;
			}
		}
		averageMag /= HighestMode;
		Mode.Magnitude = averageMag;


		if ((HighestMode == 1) || ((double)HighestMode /(double)iterations.size()) < .75)
		{
			bestIteration = SmallestIteration;
		}
		else
		{
			bestIteration = Mode;
		}



		free(iterationCounts);
		break;
	}

	return bestIteration;
}

void MandelFractal::FractalWorker(unsigned char*ImageData, mpf_t &LeftMostX, mpf_t &TopMostI, mpf_t &PixelCoordinateDelta, mpf_t &SuperSampleCoordinateDelta)
{
	mpf_t CurrentX;
	mpf_init(CurrentX);
	mpf_t CurrentI;
	mpf_init(CurrentI);
	mpf_t SuperI;
	mpf_init(SuperI);
	mpf_t SuperX;
	mpf_init(SuperX);
	mpf_t cx;
	mpf_init(cx);
	mpf_t ci;
	mpf_init(ci);
	mpf_t zx;
	mpf_init(zx);
	mpf_t zi;
	mpf_init(zi);
	mpf_t Magnitude;
	mpf_init(Magnitude);
	mpf_t zx_squared;
	mpf_init(zx_squared);
	mpf_t zi_squared;
	mpf_init(zi_squared);
	mpf_t tempzx;
	mpf_init(tempzx);
	mpf_t tempzi;
	mpf_init(tempzi);
	mpf_t RowPixelOffset;
	mpf_init(RowPixelOffset);

	double MinIterationMagnitude = 0.0;
	int ThreadNum                = m_ThreadCount.fetch_add(1);
	int ImageDataIterator        = ThreadNum * m_PixelWidth * 3;
	int CellsCompleted           = 0;
	int OffsetToNextAssignedRow  = 3 * m_PixelWidth * (m_TotalThreads - 1);
	int MinIteration             = 0;
	int CurrentIterations        = 0;
	bool PreviouslyPaused        = false;
	bool DoFullCalculation       = true;

	vector<IterationData> iterationsVector;

	mpf_mul_ui(CurrentI, PixelCoordinateDelta, ThreadNum);
	mpf_sub(CurrentI, TopMostI, CurrentI);

	mpf_mul_ui(RowPixelOffset, PixelCoordinateDelta, m_TotalThreads);

	for (int row = ThreadNum; row < m_PixelHeight; row+= m_TotalThreads)
	{
		mpf_set(CurrentX, LeftMostX);

		for (int col = 0; col < m_PixelWidth; col++)
		{
			// Get Iteration Count for Pixel
			MinIteration = m_MaxIterations;
			mpf_set(SuperI, CurrentI);
			for (int SuperRow = 0; SuperRow < m_SuperSampleRate; SuperRow++)
			{
				mpf_set(SuperX, CurrentX);
				for (int SuperCol = 0; SuperCol < m_SuperSampleRate; SuperCol++)
				{
					CurrentIterations = 0;
					mpf_set(cx, SuperX);
					mpf_set(ci, SuperI);
					mpf_set(zx, SuperX);
					mpf_set(zi, SuperI);
					mpf_set_ui(Magnitude, 0);

					if (IsTrivialCoordinate(SuperX, SuperI))
					{
						CurrentIterations = m_MaxIterations;
					}
					else
					{
						while (CurrentIterations < m_MaxIterations && (mpf_cmp_ui(Magnitude, 4) <= 0))
						{
							mpf_pow_ui(zx_squared, zx, 2);
							mpf_pow_ui(zi_squared, zi, 2);
							mpf_sub(tempzx, zx_squared, zi_squared);
							mpf_mul(tempzi, zx, zi);
							mpf_mul_ui(tempzi, tempzi, 2);

							mpf_add(zx, tempzx, cx);
							mpf_add(zi, tempzi, ci);

							mpf_pow_ui(zx_squared, zx, 2);
							mpf_pow_ui(zi_squared, zi, 2);
							mpf_add(Magnitude, zx_squared, zi_squared);

							CurrentIterations++;
						}
					}

					if (CurrentIterations < MinIteration)
					{
						MinIteration = CurrentIterations;
						MinIterationMagnitude = mpf_get_d(Magnitude);
					}

					iterationsVector.push_back(IterationData{ CurrentIterations,mpf_get_d(Magnitude) });

					mpf_add(SuperX, SuperX, SuperSampleCoordinateDelta);
				}
				mpf_sub(SuperI, SuperI, SuperSampleCoordinateDelta);
			}

			IterationData FinalIterationChoice = GetBestIteration(iterationsVector,MODE);
			iterationsVector.clear();
			if (FinalIterationChoice.Iteration == m_MaxIterations)
			{
				m_MaxIterCells++;
			}

			FillPixel(ImageDataIterator, FinalIterationChoice.Iteration, FinalIterationChoice.Magnitude, ImageData);


			//FillPixel(ImageDataIterator, MinIteration, MinIterationMagnitude, ImageData);
			ImageDataIterator += 3;
			m_FinishedCells++;

			CellsCompleted++;

			mpf_add(CurrentX, CurrentX, PixelCoordinateDelta);

			while (m_Paused)
			{
				if (!PreviouslyPaused)
				{
					cout << "Thread: " << ThreadNum << " PAUSED." << endl;
					PreviouslyPaused = true;
				}
				M_Sleep100ms();
				if (m_abort)
				{
					break;
				}

			}

			if (PreviouslyPaused)
			{
				PreviouslyPaused = false;
				cout << "Thread: " << ThreadNum << " RESUMED." << endl;
			}

			if (m_abort)
			{
				break;
			}

		}
		ImageDataIterator += OffsetToNextAssignedRow;
		mpf_sub(CurrentI, CurrentI, RowPixelOffset);

		if (m_abort)
		{
			break;
		}

	}

	mpf_clear(CurrentX);
	mpf_clear(SuperI);
	mpf_clear(SuperX);
	mpf_clear(CurrentI);
	mpf_clear(cx);
	mpf_clear(ci);
	mpf_clear(zx);
	mpf_clear(zi);
	mpf_clear(Magnitude);
	mpf_clear(zx_squared);
	mpf_clear(zi_squared);
	mpf_clear(tempzx);
}

void MandelFractal::GetPlotDimensions(mpf_t &height, mpf_t &width)
{
	mpf_set(height, m_PlotHeight);
	mpf_set(width, m_PlotWidth);
}

void MandelFractal::Abort()
{
	m_abort = true;
}

MandelFractal::~MandelFractal()
{
	mpf_clear(m_CenterX);
	mpf_clear(m_CenterI);
	mpf_clear(m_PlotHeight);
	mpf_clear(m_PlotWidth);

	//free(m_Cells);
}
