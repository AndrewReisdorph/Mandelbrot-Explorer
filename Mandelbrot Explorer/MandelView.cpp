#include <thread>
#include <chrono>
#include <math.h>
#include <wx/dcbuffer.h>
#include "main.h"
#include "MandelView.h"
#include <wx/dir.h>
#include <regex>

MandelViewPanel::MandelViewPanel(wxFrame *parent, MandelControlPanel *ControlPanel):wxScrolledWindow(parent)
{
	m_parent = parent;
	m_ControlPanel = ControlPanel;
	m_MandelHeight = DEFAULT_MANDEL_HEIGHT;
	m_MandelWidth = DEFAULT_MANDEL_WIDTH;
	SetMinSize(wxSize(m_MandelWidth, m_MandelHeight));
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	SetWindowStyle(wxSUNKEN_BORDER);
	m_parent->SetStatusText(wxT("Allocating Cells"), 0);
	m_fractal = new MandelFractal(m_ControlPanel, this, m_MandelHeight, m_MandelWidth);
	m_parent->SetStatusText(wxT("Ready"), 0);

	m_ImageData = (unsigned char*) calloc(m_MandelWidth * m_MandelHeight * 3, 1);
	
	m_BoxStart.Active = false;
	m_BoxEnd.Active = false;

	m_FractalImage = new wxImage(m_MandelWidth, m_MandelHeight, m_ImageData);
	SetScrollbars(1, 1, m_MandelWidth, m_MandelHeight, 0, 0);

	this->Connect(wxEVT_PAINT, wxPaintEventHandler(MandelViewPanel::OnPaint));
	this->Connect(wxEVT_MOTION, wxMouseEventHandler(MandelViewPanel::OnMouseMove));
	this->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(MandelViewPanel::OnMouseLeftDown));
	this->Connect(wxEVT_LEFT_UP, wxMouseEventHandler(MandelViewPanel::OnMouseLeftUp));

}

void MandelViewPanel::SetResolution(int Width, int Height)
{
	m_MandelHeight = Height;
	m_MandelWidth = Width;

	std::cout << "New Resolution " << Width << " " << Height << std::endl;
	m_FractalImage->Resize(wxSize(m_MandelWidth, m_MandelHeight), wxPoint(0, 0));
	m_ImageData = (unsigned char*)malloc(m_MandelWidth * m_MandelHeight * 3);
	m_FractalImage->SetData(m_ImageData);
	SetScrollbars(1, 1, m_MandelWidth, m_MandelHeight, 0, 0);
	this->Refresh();
	m_fractal->SetResolution(Width, Height);

}

void MandelViewPanel::DoRefresh()
{
	Refresh();
}

void MandelViewPanel::SaveImage()
{
	wxFileDialog *SaveDialog = new wxFileDialog(this, _("Save Image To File"), wxEmptyString, wxEmptyString, wxEmptyString, wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
	if (SaveDialog->ShowModal() == wxID_OK)
	{
		m_FractalImage->SaveFile(SaveDialog->GetPath(), wxBITMAP_TYPE_PNG);
	}
	SaveDialog->Destroy();
}

void MandelViewPanel::OnMouseMove(wxMouseEvent& event)
{
	wxPoint MousePosition = event.GetPosition();

	if (MousePosition.x < m_MandelWidth && MousePosition.y < m_MandelHeight)
	{
		mpf_t x;
		mpf_init(x);
		mpf_t y;
		mpf_init(y);

		m_fractal->PanelCoordinateToComplex(MousePosition.x, MousePosition.y, x, y);
		m_ControlPanel->SetMouseCoordinateInfo(x,y);

		mpf_clear(x);
		mpf_clear(y);

		if (!m_ControlPanel->RenderInProgress() && m_BoxStart.Active && event.Dragging())
		{
			m_BoxEnd.x = MousePosition.x;
			m_BoxEnd.y = MousePosition.y;
			m_BoxEnd.Active = true;
			Refresh();
		}
	}

	event.Skip();
}

void MandelViewPanel::OnMouseLeftDown(wxMouseEvent& event)
{
	if (!m_ControlPanel->RenderInProgress())
	{
		wxPoint MousePosition = event.GetPosition();
		m_BoxStart.x = MousePosition.x;
		m_BoxStart.y = MousePosition.y;
		m_BoxStart.Active = true;
	}

	event.Skip();

}

void MandelViewPanel::OnMouseLeftUp(wxMouseEvent& event)
{
	if (!m_ControlPanel->RenderInProgress())
	{
		m_BoxStart.Active = false;
		m_BoxEnd.Active = false;

		Refresh();
		CropImageToSelectionAndZoom();
		Refresh();

		m_ControlPanel->SetRenderState(RENDER_ACTIVE);

		RenderSelection();
	}
	event.Skip();

}

void MandelViewPanel::CropImageToSelectionAndZoom()
{
	int width = abs(m_BoxStart.x - m_BoxEnd.x);
	int height = width *(m_MandelHeight / float(m_MandelWidth));
	int LeftMostX = (m_BoxStart.x < m_BoxEnd.x) ? m_BoxStart.x : m_BoxEnd.x;
	int TopMostY = (m_BoxStart.y < m_BoxEnd.y) ? m_BoxStart.y : m_BoxEnd.y;

	// Copy subimage to buffer
	unsigned char* subimage = (unsigned char*)malloc(width * height * 3);
	int SubImageStartIndex = TopMostY * m_MandelWidth + LeftMostX;
	int ImageDataIter = SubImageStartIndex;
	int SubImageDataIter = 0;
	for (int row = 0; row < height; row++)
	{
		ImageDataIter = SubImageStartIndex + row * m_MandelWidth;
		for (int col = 0; col < width; col++)
		{
			subimage[SubImageDataIter++] = m_ImageData[(ImageDataIter + col) * 3 + 0];
			subimage[SubImageDataIter++] = m_ImageData[(ImageDataIter + col) * 3 + 1];
			subimage[SubImageDataIter++] = m_ImageData[(ImageDataIter + col) * 3 + 2];
		}
	}

	// Fill image buffer with scaled subimage
	double xRatio = width / double(m_MandelWidth);
	double yRatio = height / double(m_MandelHeight);
	double px;
	double py;
	int bigImageIndex;
	int smallImageIndex;
	for (int row = 0; row < m_MandelHeight; row++)
	{
		for (int col = 0; col < m_MandelWidth; col++)
		{
			px = floor(col*xRatio);
			py = floor(row*yRatio);
			bigImageIndex = 3 * int(row * m_MandelWidth + col);
			smallImageIndex = 3 * int(py * width + px);
			m_ImageData[bigImageIndex] = subimage[smallImageIndex];
			m_ImageData[bigImageIndex+1] = subimage[smallImageIndex+1];
			m_ImageData[bigImageIndex+2] = subimage[smallImageIndex+2];
		}
	}

	free(subimage);
}

void MandelViewPanel::DarkenImage()
{
	for (int DataIter = 0; DataIter < m_MandelWidth * m_MandelHeight * 3; DataIter++)
	{
		m_ImageData[DataIter] /= 2;
	}
}

void MandelViewPanel::RenderVideoThread()
{
	std::cout << "RenderFrames" << std::endl;
	int FrameNumber;
	bool aborted = false;
	int initalIterationCount = 250;
	double ZoomRate = 0.975;
	wxString SaveDirectory;
	RenderSettings settings = m_ControlPanel->GetRenderSettings();
	int Iterations = initalIterationCount;
	mpf_t plotHeight;
	mpf_t plotWidth;
	mpf_init(plotHeight);
	mpf_init(plotWidth);


	wxString filename;
	wxDirDialog *SelectDirectoryDialog = new wxDirDialog(this, _("Select Output Directory"),"",wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
	if (SelectDirectoryDialog->ShowModal() == wxID_OK)
	{
		SaveDirectory = SelectDirectoryDialog->GetPath();
		std::regex pattern("frame_(\\d+).png");
		std::smatch matches;
		wxDir directory(SelectDirectoryDialog->GetPath());
		bool cont = directory.GetFirst(&filename);
		int highest_frame = 0;
		while (cont)
		{
			std::string s(filename.c_str());
			if (std::regex_search(s, matches, pattern))
			{
				int frame_num = stoi(matches[1]);
				if (frame_num > highest_frame)
				{
					highest_frame = frame_num;
				}
			}
			cont = directory.GetNext(&filename);
		}
		FrameNumber = highest_frame + 1;
		SelectDirectoryDialog->Destroy();
	}
	else
	{
		SelectDirectoryDialog->Destroy();
		m_ControlPanel->SetRenderState(RENDER_IDLE);

		return;
	}

	Iterations = initalIterationCount + 5*FrameNumber;
	for (int i = 1; i < FrameNumber; i++)
	{
		m_fractal->Zoom(ZoomRate);
	}
	m_ControlPanel->SetMaxIterations(Iterations);

	while (true)
	{
		m_ControlPanel->SetRenderState(RENDER_ACTIVE);
		m_ControlPanel->UpdateProgressBar(0);
		m_fractal->GetPlotDimensions(plotHeight, plotWidth);
		m_ControlPanel->SetPlotDimensionInfo(plotWidth, plotHeight);
		aborted = m_fractal->RenderFractal(m_ImageData);
		if (aborted)
		{
			break;
		}
		Refresh();
		std::ostringstream stream;
		stream << SaveDirectory << "\\frame_" << FrameNumber << ".png";
		m_FractalImage->SaveFile(stream.str(), wxBITMAP_TYPE_PNG);
		FrameNumber++;
		Iterations += 5;
		m_ControlPanel->SetMaxIterations(Iterations);
		DarkenImage();
		m_fractal->Zoom(ZoomRate);

	}

	m_ControlPanel->SetRenderState(RENDER_IDLE);

	mpf_clear(plotHeight);
	mpf_clear(plotWidth);
}

void MandelViewPanel::RenderVideo()
{
	std::thread video_thread(&MandelViewPanel::RenderVideoThread, this);
	video_thread.detach();
}

void MandelViewPanel::AbortRender()
{
	m_fractal->Abort();
}

void MandelViewPanel::DrawSelectionBox(wxAutoBufferedPaintDC& dc)
{
	int width = abs(m_BoxStart.x - m_BoxEnd.x);
	int height = width *(m_MandelHeight / float(m_MandelWidth));
	dc.SetPen(wxPen(wxColour(255, 0, 0)));
	dc.SetBrush(wxBrush(wxColour(0, 0, 0, 0), wxBRUSHSTYLE_TRANSPARENT));

	int BoxX = m_BoxStart.x;
	int BoxY = m_BoxStart.y;

	if (m_BoxStart.x < m_BoxEnd.x && m_BoxStart.y < m_BoxEnd.y)
	{
		// Do nothing
	}
	else if (m_BoxStart.x > m_BoxEnd.x && m_BoxStart.y > m_BoxEnd.y)
	{
		BoxX -= width;
		BoxY -= height;
	}
	else if (m_BoxStart.x < m_BoxEnd.x && m_BoxStart.y > m_BoxEnd.y)
	{
		BoxY -= height;
	}
	else if (m_BoxStart.x > m_BoxEnd.x && m_BoxStart.y < m_BoxEnd.y)
	{
		BoxX -= width;
	}

	dc.DrawRectangle(BoxX, BoxY, width, height);
}

void MandelViewPanel::OnPaint(wxPaintEvent& event)
{
	wxAutoBufferedPaintDC dc(this);
	dc.SetBrush(wxBrush(wxColour(0, 0, 0, 0), wxBRUSHSTYLE_SOLID));
	dc.Clear();
	wxBitmap fractalBitmap = wxBitmap(*m_FractalImage);
	if (fractalBitmap.IsOk())
	{
		dc.DrawBitmap(fractalBitmap, wxPoint(0, 0));
	}

	if (m_BoxStart.Active && m_BoxEnd.Active)
	{
		DrawSelectionBox(dc);
	}
}

void MandelViewPanel::DoRender()
{
	m_ControlPanel->UpdateProgressBar(0);

	m_fractal->RenderFractal(m_ImageData);

	Refresh();

}

void MandelViewPanel::DoRenderSelection()
{
	int width = abs(m_BoxStart.x - m_BoxEnd.x);
	int height = width * (m_MandelHeight / float(m_MandelWidth));
	SelectionBoxPoint AdjustedEnd;
	AdjustedEnd.x = m_BoxStart.x + width;
	AdjustedEnd.y = m_BoxStart.y + height;

	m_fractal->RenderFractal(m_BoxStart, AdjustedEnd, m_ImageData);

	Refresh();

	m_ControlPanel->UpdateProgressBar(0);

}

void MandelViewPanel::RenderSelection()
{
	std::thread render_thread(&MandelViewPanel::DoRenderSelection, this);
	render_thread.detach();

}

void MandelViewPanel::Render()
{
	std::thread render_thread(&MandelViewPanel::DoRender, this);
	render_thread.detach();
}

void MandelViewPanel::PauseRender()
{
	m_fractal->PauseRender();
}

void MandelViewPanel::ResumeRender()
{
	m_fractal->ResumeRender();
}

MandelViewPanel::~MandelViewPanel()
{
	delete m_fractal;
	delete m_FractalImage;
}