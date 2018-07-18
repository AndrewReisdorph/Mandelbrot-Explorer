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
	tool_bar_->SetToolBitmapSize(wxSize(24,24));
	wxBitmap render_button_bitmap = wxBITMAP_PNG_FROM_DATA(fractal);
	wxBitmap camera_button_bitmap = wxBITMAP_PNG_FROM_DATA(camera);
	wxBitmap palette_button_bitmap = wxBITMAP_PNG_FROM_DATA(palette);
	wxBitmap save_button_bitmap = wxBITMAP_PNG_FROM_DATA(save);
	wxBitmap video_button_bitmap = wxBITMAP_PNG_FROM_DATA(video);
	wxBitmap open_button_bitmap = wxBITMAP_PNG_FROM_DATA(folder);
	wxBitmap select_box_bitmap = wxBITMAP_PNG_FROM_DATA(box);
	wxBitmap zoom_bitmap = wxBITMAP_PNG_FROM_DATA(zoom);
	wxBitmap home_bitmap = wxBITMAP_PNG_FROM_DATA(home);
	wxBitmap properties_bitmap = wxBITMAP_PNG_FROM_DATA(properties);
	resume_bitmap_ = wxBITMAP_PNG_FROM_DATA(play);
	pause_bitmap_ = wxBITMAP_PNG_FROM_DATA(pause);
	cancel_bitmap_ = wxBITMAP_PNG_FROM_DATA(cancel);

	tool_bar_->AddTool(ID_TOOLBAR_OPEN, wxT("Open Configuration"), open_button_bitmap);
	tool_bar_->AddTool(ID_TOOLBAR_SAVE, wxT("Save Configuration"), save_button_bitmap);
	tool_bar_->AddSeparator();
	tool_bar_->AddTool(ID_TOOLBAR_RENDER, wxT("Render"), render_button_bitmap);
	tool_bar_->AddTool(ID_TOOLBAR_CAPTURE, wxT("Save Image"), camera_button_bitmap);
	tool_bar_->AddTool(ID_TOOLBAR_PALETTE, wxT("Palette"), palette_button_bitmap);
	tool_bar_->AddTool(ID_TOOLBAR_PROPERTIES, wxT("Properties"), properties_bitmap);
	tool_bar_->AddTool(ID_TOOLBAR_VIDEO, wxT("Render Video"), video_button_bitmap);
	tool_bar_->AddSeparator();
	tool_bar_->AddTool(ID_TOOLBAR_PAUSE_RESUME, wxT("Pause/Resume Render"), resume_bitmap_);
	tool_bar_->AddTool(ID_TOOLBAR_CANCEL, wxT("Cancel Render"), cancel_bitmap_);
	tool_bar_->AddStretchableSpace();
	tool_bar_->AddTool(ID_TOOLBAR_HOME, wxT("Home"), home_bitmap);
	tool_bar_->AddTool(ID_TOOLBAR_SELECT, wxT("Selection Tool"), select_box_bitmap);
	tool_bar_->AddTool(ID_TOOLBAR_ZOOM, wxT("Zoom"), zoom_bitmap);
	tool_bar_->Realize();

	CreateStatusBar(1);

	
	main_h_splitter_ = new wxSplitterWindow(this);

	mandel_properties_panel_ = new MandelPropertiesPanel(main_h_splitter_);
	

	FractalRenderPanel* fractal_render_panel = new FractalRenderPanel(main_h_splitter_);
	int scroll_bar_width = wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
	int sash_initial_position = mandel_properties_panel_->GetBestSize().GetWidth() + (scroll_bar_width * 2);
	main_h_splitter_->SplitVertically(mandel_properties_panel_, fractal_render_panel, sash_initial_position);

	fractal_render_panel->SetMinSize(wxSize(854, 480));
	wxSize properties_panel_size = mandel_properties_panel_->GetBestSize();
	properties_panel_size.SetWidth(properties_panel_size.GetWidth() + scroll_bar_width * 2);
	mandel_properties_panel_->SetMinSize(properties_panel_size);

	palette_panel_ = new PalettePanel(this);
	palette_panel_->SetMinSize(wxSize(-1, 128));

	render_progress_gauge_ = new wxGauge(this, wxID_ANY, 1000, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL | wxGA_PROGRESS | wxGA_SMOOTH);


	wxBoxSizer* main_v_box_sizer = new wxBoxSizer(wxVERTICAL);
	main_v_box_sizer->Add(main_h_splitter_, wxSizerFlags().Expand().Proportion(1));
	main_v_box_sizer->Add(palette_panel_, wxSizerFlags().Expand());
	main_v_box_sizer->Add(render_progress_gauge_, wxSizerFlags().Expand());

	SetSizer(main_v_box_sizer);
	main_v_box_sizer->Fit(this);
	Centre();

	fractal_render_panel->SetMinSize(wxSize(-1, -1));
	mandel_properties_panel_->SetMinSize(wxSize(-1, -1));

	mandel_properties_panel_->FitColumns();
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

void MandelExploreFrame::OnPaletteButton(wxCommandEvent & event)
{
	if (palette_panel_->IsShown())
	{
		palette_panel_->Hide();
	}
	else
	{
		palette_panel_->Show();
	}
	
	Layout();
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

void MandelExploreFrame::OnHomeButton(wxCommandEvent & event)
{
	cout << "Home" << endl;
}

void MandelExploreFrame::OnPropertiesButton(wxCommandEvent & event)
{
	if (main_h_splitter_->IsSashInvisible())
	{
		int scroll_bar_width = wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
		int sash_initial_position = mandel_properties_panel_->GetBestSize().GetWidth() + (scroll_bar_width * 2);
		main_h_splitter_->SetSashPosition(sash_initial_position);
		mandel_properties_panel_->Show(true);
		main_h_splitter_->SetSashInvisible(false);
		mandel_properties_panel_->FitColumns();
	}
	else
	{
		main_h_splitter_->SetSashPosition(1);
		mandel_properties_panel_->Show(false);
		main_h_splitter_->SetSashInvisible();
	}

	Refresh();
	Layout();
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
EVT_TOOL(ID_TOOLBAR_PALETTE, MandelExploreFrame::OnPaletteButton)
EVT_TOOL(ID_TOOLBAR_PROPERTIES, MandelExploreFrame::OnPropertiesButton)
EVT_TOOL(ID_TOOLBAR_VIDEO, MandelExploreFrame::OnVideoButton)
EVT_TOOL(ID_TOOLBAR_PAUSE_RESUME, MandelExploreFrame::OnPauseResumeButton)
EVT_TOOL(ID_TOOLBAR_CANCEL, MandelExploreFrame::OnCancelButton)
EVT_TOOL(ID_TOOLBAR_SELECT, MandelExploreFrame::OnSelectButton)
EVT_TOOL(ID_TOOLBAR_ZOOM, MandelExploreFrame::OnZoomButton)
EVT_TOOL(ID_TOOLBAR_HOME, MandelExploreFrame::OnHomeButton)
EVT_TOOL_ENTER(wxID_ANY, MandelExploreFrame::OnToolBarMouseOver)
wxEND_EVENT_TABLE()