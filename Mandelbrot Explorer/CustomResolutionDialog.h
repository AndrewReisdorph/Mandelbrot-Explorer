#include <wx/wx.h>
#include <wx/valnum.h>

class CustomResolutionDialog : public wxDialog
{
private:
	void OnCancelButton(wxCommandEvent& event);	
	void OnOKButton(wxCommandEvent& event);
	void OnKeyUp(wxKeyEvent& event);
	void ValidateDimensions();

	wxTextCtrl *m_WidthTextCtrl;
	wxTextCtrl *m_HeightTextCtrl;

	long m_Width;
	long m_Height;

public:
	CustomResolutionDialog();
	long GetWidth();
	long GetHeight();

};