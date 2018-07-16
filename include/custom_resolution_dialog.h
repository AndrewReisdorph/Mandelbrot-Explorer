#include <wx/wx.h>
#include <wx/valnum.h>

class CustomResolutionDialog : public wxDialog
{
private:
	void OnCancelButton(wxCommandEvent& event);
	void OnOKButton(wxCommandEvent& event);
	void OnKeyUp(wxKeyEvent& event);
	void ValidateDimensions();

	wxTextCtrl *width_text_ctrl_;
	wxTextCtrl *height_text_ctrl_;

	long width_;
	long height_;

public:
	CustomResolutionDialog();
	long GetWidth();
	long GetHeight();

};