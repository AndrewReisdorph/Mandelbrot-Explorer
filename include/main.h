#pragma once

#include <wx/app.h>
#include <wx/image.h> 

class MandelExplorerApp : public wxApp
{
private:
	bool OnInit();
};

wxDECLARE_APP(MandelExplorerApp);