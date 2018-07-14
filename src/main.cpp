#include "main.h"
#include "MandelExploreFrame.h"

#include<iostream>

using namespace std;

wxIMPLEMENT_APP_CONSOLE(MandelExplorerApp);

bool MandelExplorerApp::OnInit()
{
	this->SetAppName("Mandelbrot Explorer");

	wxInitAllImageHandlers();

	MandelExploreFrame* frame = new MandelExploreFrame("Mandelbrot Explorer");
	frame->Show(true);

	return true;
}