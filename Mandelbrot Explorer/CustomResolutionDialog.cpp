#include "CustomResolutionDialog.h"

CustomResolutionDialog::CustomResolutionDialog(): wxDialog(NULL, -1, "Custom Resolution")//, wxDefaultPosition, wxSize(250, 230))
{
	wxBoxSizer *MainVSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *ButtonHSizer = new wxBoxSizer(wxHORIZONTAL);
	wxFlexGridSizer *gridSizer = new wxFlexGridSizer(2, 2, 5, 5);

	wxIntegerValidator<unsigned long> val;

	wxStaticText *WidthText = new wxStaticText(this, wxID_ANY, "Width:");
	m_WidthTextCtrl = new wxTextCtrl(this, wxID_ANY, "854", wxDefaultPosition, wxDefaultSize, 0, val);
	wxStaticText *HeightText = new wxStaticText(this, wxID_ANY, "Height:");
	m_HeightTextCtrl = new wxTextCtrl(this, wxID_ANY, "480", wxDefaultPosition, wxDefaultSize, 0, val);

	gridSizer->Add(WidthText,wxSizerFlags().CentreVertical());
	gridSizer->Add(m_WidthTextCtrl);
	gridSizer->Add(HeightText, wxSizerFlags().CentreVertical());
	gridSizer->Add(m_HeightTextCtrl);

	wxButton *OKButton = new wxButton(this, -1, wxT("OK"));//,wxDefaultPosition, wxSize(70, 30));
	OKButton->Bind(wxEVT_BUTTON, &CustomResolutionDialog::OnOKButton, this);
	wxButton *CancelButton = new wxButton(this, -1, wxT("Cancel"));//,wxDefaultPosition, wxSize(70, 30));
	CancelButton->Bind(wxEVT_BUTTON, &CustomResolutionDialog::OnCancelButton, this);
	ButtonHSizer->Add(CancelButton,wxSizerFlags().Expand());
	ButtonHSizer->Add(OKButton,wxSizerFlags().Expand());

	
	MainVSizer->Add(gridSizer, wxSizerFlags().Expand().Border(wxALL));
	MainVSizer->Add(ButtonHSizer, wxSizerFlags().Right().Border(wxALL));
	SetSizerAndFit(MainVSizer);
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
	m_WidthTextCtrl->GetValue().ToLong(&m_Width);
	m_HeightTextCtrl->GetValue().ToLong(&m_Height);

	if ((m_Width * m_Height) < 1)
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
	return m_Width;
}

long CustomResolutionDialog::GetHeight()
{
	return m_Height;
}