#pragma once
#include <atomic>
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "MandelFractal.h"
#include "MandelControlPanel.h"

class MandelFractal;

typedef struct SelectionBoxPoint
{
	int x;
	int y;
	bool Active;
} SelectionBoxPoint;

class MandelViewPanel : public wxScrolledWindow
{
private:
	void OnPaint(wxPaintEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseLeftDown(wxMouseEvent& event);
	void OnMouseLeftUp(wxMouseEvent& event);
	void DrawSelectionBox(wxAutoBufferedPaintDC& dc);
	void CropImageToSelectionAndZoom();
	void DoRender();
	void DoRenderSelection();
	void RenderVideoThread();
	void DarkenImage();

	wxFrame *m_parent;
	MandelFractal *m_fractal;
	MandelControlPanel *m_ControlPanel;
	wxImage *m_FractalImage;
	unsigned char *m_ImageData;
	SelectionBoxPoint m_BoxStart;
	SelectionBoxPoint m_BoxEnd;
	int m_MandelHeight;
	int m_MandelWidth;

public:
	MandelViewPanel(wxFrame *parent, MandelControlPanel *ControlPanel);
	~MandelViewPanel();
	void DoRefresh();
	void Render();
	void RenderSelection();
	void SaveImage();
	void PauseRender();
	void ResumeRender();
	void RenderVideo();
	void AbortRender();
	void SetResolution(int Width, int Height);

};