#include "CustomResolutionDialog.h"

CustomResolutionDialog::CustomResolutionDialog(): wxDialog(NULL, -1, "Custom Resolution")//, wxDefaultPosition, wxSize(250, 230))
{
	wxBoxSizer *MainVSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *ButtonHSizer = new wxBoxSizer(wxHORIZONTAL);
	wxFlexGridSizer *gridSizer = new wxFlexGridSizer(2, 2, 5, 5);

	wxTextValidator resolutionValidator(wxFILTER_INCLUDE_CHAR_LIST);
	wxArrayString validatorCharList;
	wxString validChars(wxT("0123456789"));
	size_t numChars = validChars.Length();
	for (size_t charIter=0; charIter<numChars; charIter++)
	{
		validatorCharList.Add(wxString(validChars.GetChar(charIter)));
	}
	resolutionValidator.SetIncludes(validatorCharList);

	wxStaticText *WidthText = new wxStaticText(this, wxID_ANY, "Width:");
	m_WidthTextCtrl = new wxTextCtrl(this, wxID_ANY, "854", wxDefaultPosition, wxDefaultSize, 0, resolutionValidator);
	m_WidthTextCtrl->Bind(wxEVT_KEY_UP, &CustomResolutionDialog::OnKeyUp, this);
	wxStaticText *HeightText = new wxStaticText(this, wxID_ANY, "Height:");
	m_HeightTextCtrl = new wxTextCtrl(this, wxID_ANY, "480", wxDefaultPosition, wxDefaultSize, 0, resolutionValidator);
	m_HeightTextCtrl->Bind(wxEVT_KEY_UP, &CustomResolutionDialog::OnKeyUp, this);

	gridSizer->Add(WidthText,wxALIGN_CENTRE_VERTICAL);
	gridSizer->Add(m_WidthTextCtrl);
	gridSizer->Add(HeightText, wxALIGN_CENTRE_VERTICAL);
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
	ValidateDimensions();
}

void CustomResolutionDialog::OnKeyUp(wxKeyEvent & event)
{
	int keyCode = event.GetKeyCode();

	std::cout << "Got key: " << keyCode << std::endl;

	if (keyCode == WXK_RETURN || keyCode == WXK_NUMPAD_ENTER)
	{
		ValidateDimensions();
	}
}

void CustomResolutionDialog::ValidateDimensions()
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