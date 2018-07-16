#include "custom_resolution_dialog.h"

CustomResolutionDialog::CustomResolutionDialog() : wxDialog(NULL, -1, "Custom Resolution")
{
	wxBoxSizer *main_v_sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *button_h_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxFlexGridSizer *grid_sizer = new wxFlexGridSizer(2, 2, 5, 5);

	wxTextValidator resolution_validator(wxFILTER_INCLUDE_CHAR_LIST);
	wxArrayString validator_char_list;
	wxString valid_chars(wxT("0123456789"));
	size_t num_chars = valid_chars.Length();
	for (size_t char_iter = 0; char_iter<num_chars; char_iter++)
	{
		validator_char_list.Add(wxString(valid_chars.GetChar(char_iter)));
	}
	resolution_validator.SetIncludes(validator_char_list);

	wxStaticText *width_static_text = new wxStaticText(this, wxID_ANY, "Width:");
	width_text_ctrl_ = new wxTextCtrl(this, wxID_ANY, "854", wxDefaultPosition, wxDefaultSize, 0, resolution_validator);
	width_text_ctrl_->Bind(wxEVT_KEY_UP, &CustomResolutionDialog::OnKeyUp, this);
	wxStaticText *height_static_text = new wxStaticText(this, wxID_ANY, "Height:");
	height_text_ctrl_ = new wxTextCtrl(this, wxID_ANY, "480", wxDefaultPosition, wxDefaultSize, 0, resolution_validator);
	height_text_ctrl_->Bind(wxEVT_KEY_UP, &CustomResolutionDialog::OnKeyUp, this);

	grid_sizer->Add(width_static_text, 0, wxALIGN_CENTRE_VERTICAL);
	grid_sizer->Add(width_text_ctrl_);
	grid_sizer->Add(height_static_text, 0, wxALIGN_CENTRE_VERTICAL);
	grid_sizer->Add(height_text_ctrl_);

	wxButton *ok_button = new wxButton(this, -1, wxT("OK"));
	ok_button->Bind(wxEVT_BUTTON, &CustomResolutionDialog::OnOKButton, this);
	wxButton *cancel_button = new wxButton(this, -1, wxT("Cancel"));
	cancel_button->Bind(wxEVT_BUTTON, &CustomResolutionDialog::OnCancelButton, this);
	button_h_sizer->Add(cancel_button, wxSizerFlags().Expand());
	button_h_sizer->Add(ok_button, wxSizerFlags().Expand());


	main_v_sizer->Add(grid_sizer, wxSizerFlags().Expand().Border(wxALL));
	main_v_sizer->Add(button_h_sizer, wxSizerFlags().Right().Border(wxALL));
	SetSizerAndFit(main_v_sizer);
	Centre();
	ShowModal();
	Destroy();
}

void CustomResolutionDialog::OnCancelButton(wxCommandEvent& event)
{
	EndModal(wxCANCEL);
}

void CustomResolutionDialog::OnOKButton(wxCommandEvent& event)
{
	ValidateDimensions();
}

void CustomResolutionDialog::OnKeyUp(wxKeyEvent & event)
{
	int key_code = event.GetKeyCode();

	if (key_code == WXK_RETURN || key_code == WXK_NUMPAD_ENTER)
	{
		ValidateDimensions();
	}
}

void CustomResolutionDialog::ValidateDimensions()
{
	width_text_ctrl_->GetValue().ToLong(&width_);
	height_text_ctrl_->GetValue().ToLong(&height_);

	if ((width_ * height_) < 1)
	{
		wxMessageDialog(this, "Invalid Image Size", "Warning").ShowModal();
	}
	else
	{
		EndModal(wxOK);
	}
}

long CustomResolutionDialog::GetWidth()
{
	return width_;
}

long CustomResolutionDialog::GetHeight()
{
	return height_;
}