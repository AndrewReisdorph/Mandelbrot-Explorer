#include "mandel_explore_frame.h"
#include "images.h"

#include <wx/gdicmn.h> 
#ifdef WIN32
#include <wx/sysopt.h>
#endif

using namespace std;

MandelExploreFrame::MandelExploreFrame(const wxString& title) : wxFrame(0, -1, title)
{
	wxMenu *file_menu = new wxMenu;
	file_menu->Append(ID_MENU_QUIT, _T("E&xit\tAlt-X"), _T("Quit this program"));

	wxMenuBar *menu_bar = new wxMenuBar;
	menu_bar->Append(file_menu, _T("&File"));

	SetMenuBar(menu_bar);
#ifdef WIN32
	// Note that under wxMSW toolbar paints tools to reflect system-wide
	// colours. If you use more than 16 colours in your tool bitmaps, you may
	// wish to suppress this behaviour, otherwise system colours in your
	// bitmaps will inadvertently be mapped to system colours. To do this, set
	// the msw.remap system option before creating the toolbar:
	wxSystemOptions::SetOption("msw.remap", 0);
#endif
	tool_bar_ = CreateToolBar();
	wxBitmap render_button_bitmap = wxBITMAP_PNG_FROM_DATA(fractal);
	wxBitmap camera_button_bitmap = wxBITMAP_PNG_FROM_DATA(camera);
	wxBitmap save_button_bitmap = wxBITMAP_PNG_FROM_DATA(save);
	wxBitmap video_button_bitmap = wxBITMAP_PNG_FROM_DATA(video);
	wxBitmap open_button_bitmap = wxBITMAP_PNG_FROM_DATA(folder);
	wxBitmap select_box_bitmap = wxBITMAP_PNG_FROM_DATA(box);
	wxBitmap zoom_bitmap = wxBITMAP_PNG_FROM_DATA(zoom);
	resume_bitmap_ = wxBITMAP_PNG_FROM_DATA(play);
	pause_bitmap_ = wxBITMAP_PNG_FROM_DATA(pause);
	cancel_bitmap_ = wxBITMAP_PNG_FROM_DATA(cancel);

	tool_bar_->AddTool(ID_TOOLBAR_OPEN, wxT("Open Configuration"), open_button_bitmap);
	tool_bar_->AddTool(ID_TOOLBAR_SAVE, wxT("Save Configuration"), save_button_bitmap);
	tool_bar_->AddSeparator();
	tool_bar_->AddTool(ID_TOOLBAR_RENDER, wxT("Render"), render_button_bitmap);
	tool_bar_->AddTool(ID_TOOLBAR_CAPTURE, wxT("Save Image"), camera_button_bitmap);
	tool_bar_->AddTool(ID_TOOLBAR_VIDEO, wxT("Render Video"), video_button_bitmap);
	tool_bar_->AddSeparator();
	tool_bar_->AddTool(ID_TOOLBAR_PAUSE_RESUME, wxT("Pause/Resume Render"), resume_bitmap_);
	tool_bar_->AddTool(ID_TOOLBAR_CANCEL, wxT("Cancel Render"), cancel_bitmap_);
	tool_bar_->AddStretchableSpace();
	tool_bar_->AddTool(ID_TOOLBAR_SELECT, wxT("Selection Tool"), select_box_bitmap);
	tool_bar_->AddTool(ID_TOOLBAR_ZOOM, wxT("Zoom"), zoom_bitmap);
	tool_bar_->Realize();

	CreateStatusBar(1);

	wxBoxSizer* main_v_box_sizer = new wxBoxSizer(wxVERTICAL);
	MandelPropertiesPanel* mandel_properties_panel = new MandelPropertiesPanel(this);

	main_v_box_sizer->Add(mandel_properties_panel, wxSizerFlags().Expand().Proportion(1));


	SetSizer(main_v_box_sizer);
	main_v_box_sizer->Fit(this);
	Centre();
}

void MandelExploreFrame::OnOpenConfigurationButton(wxCommandEvent & event)
{
	cout << "Open Config" << endl;
}

void MandelExploreFrame::OnSaveConfigurationButton(wxCommandEvent & event)
{
	cout << "Save Config" << endl;
}

void MandelExploreFrame::OnRenderButton(wxCommandEvent & event)
{
	cout << "Render" << endl;
}

void MandelExploreFrame::OnCaptureButton(wxCommandEvent & event)
{
	cout << "Capture" << endl;
}

void MandelExploreFrame::OnVideoButton(wxCommandEvent & event)
{
	cout << "Video" << endl;
}

void MandelExploreFrame::OnPauseResumeButton(wxCommandEvent & event)
{
	cout << "Pause/Resume" << endl;
}

void MandelExploreFrame::OnCancelButton(wxCommandEvent & event)
{
	cout << "Cancel" << endl;
}

void MandelExploreFrame::OnSelectButton(wxCommandEvent & event)
{
	cout << "Select" << endl;
}

void MandelExploreFrame::OnZoomButton(wxCommandEvent & event)
{
	cout << "Zoom" << endl;
}

void MandelExploreFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	wxExit();
}

void MandelExploreFrame::OnToolBarMouseOver(wxCommandEvent & event)
{
	switch (event.GetSelection())
	{
	case ID_TOOLBAR_ZOOM:
		this->SetStatusText("Zoom: Left click to zoom in, right click to zoom out, middle click to slide");
		break;
	case wxID_ANY:
		this->SetStatusText("");
		break;
	default:
		break;
	}
}

wxBEGIN_EVENT_TABLE(MandelExploreFrame, wxFrame)
EVT_MENU(ID_MENU_QUIT, MandelExploreFrame::OnQuit)
EVT_TOOL(ID_TOOLBAR_OPEN, MandelExploreFrame::OnOpenConfigurationButton)
EVT_TOOL(ID_TOOLBAR_SAVE, MandelExploreFrame::OnSaveConfigurationButton)
EVT_TOOL(ID_TOOLBAR_RENDER, MandelExploreFrame::OnRenderButton)
EVT_TOOL(ID_TOOLBAR_CAPTURE, MandelExploreFrame::OnCaptureButton)
EVT_TOOL(ID_TOOLBAR_VIDEO, MandelExploreFrame::OnVideoButton)
EVT_TOOL(ID_TOOLBAR_PAUSE_RESUME, MandelExploreFrame::OnPauseResumeButton)
EVT_TOOL(ID_TOOLBAR_CANCEL, MandelExploreFrame::OnCancelButton)
EVT_TOOL(ID_TOOLBAR_SELECT, MandelExploreFrame::OnSelectButton)
EVT_TOOL(ID_TOOLBAR_ZOOM, MandelExploreFrame::OnZoomButton)
EVT_TOOL_ENTER(wxID_ANY, MandelExploreFrame::OnToolBarMouseOver)
wxEND_EVENT_TABLE()