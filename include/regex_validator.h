#pragma once

#include <wx/regex.h> 
#include "wx/defs.h"

#if wxUSE_VALIDATORS && (wxUSE_TEXTCTRL || wxUSE_COMBOBOX)

class WXDLLIMPEXP_FWD_CORE wxTextEntry;

#include "wx/validate.h"

enum wxRegexValidatorStyle
{
	wxREV_FILTER_NONE = 0x0,
	wxREV_FILTER_EMPTY = 0x1,
	wxREV_FILTER_ASCII = 0x2,
	wxREV_FILTER_ALPHA = 0x4,
	wxREV_FILTER_ALPHANUMERIC = 0x8,
	wxREV_FILTER_DIGITS = 0x10,
	wxREV_FILTER_NUMERIC = 0x20,
	wxREV_FILTER_INCLUDE_LIST = 0x40,
	wxREV_FILTER_INCLUDE_CHAR_LIST = 0x80,
	wxREV_FILTER_EXCLUDE_LIST = 0x100,
	wxREV_FILTER_EXCLUDE_CHAR_LIST = 0x200,
	wxREV_FILTER_REGEX = 0x400,
};

class wxRegexValidator : public wxValidator
{
public:
	wxRegexValidator(long style = wxREV_FILTER_NONE, wxString *val = NULL);
	wxRegexValidator(wxString regex, wxString *val);
	wxRegexValidator(const wxRegexValidator& val);

	virtual ~wxRegexValidator() {}

	// Make a clone of this validator (or return NULL) - currently necessary
	// if you're passing a reference to a validator.
	// Another possibility is to always pass a pointer to a new validator
	// (so the calling code can use a copy constructor of the relevant class).
	virtual wxObject *Clone() const  { return new wxRegexValidator(*this); }
	bool Copy(const wxRegexValidator& val);

	// Called when the value in the window must be validated.
	// This function can pop up an error message.
	virtual bool Validate(wxWindow *parent);

	// Called to transfer data to the window
	virtual bool TransferToWindow();

	// Called to transfer data from the window
	virtual bool TransferFromWindow();

	// Filter keystrokes
	void OnChar(wxKeyEvent& event);

	// ACCESSORS
	inline long GetStyle() const { return m_validatorStyle; }
	void SetStyle(long style);

	wxTextEntry *GetTextEntry();

	void SetCharIncludes(const wxString& chars);
	void SetIncludes(const wxArrayString& includes) { m_includes = includes; }
	inline wxArrayString& GetIncludes() { return m_includes; }

	void SetCharExcludes(const wxString& chars);
	void SetExcludes(const wxArrayString& excludes) { m_excludes = excludes; }
	inline wxArrayString& GetExcludes() { return m_excludes; }

	bool HasFlag(wxRegexValidatorStyle style) const
	{
		return (m_validatorStyle & style) != 0;
	}

	void SetRegexPattern(wxString pattern) { m_regexPattern = pattern; };

protected:

	// returns true if all characters of the given string are present in m_includes
	bool ContainsOnlyIncludedCharacters(const wxString& val) const;

	// returns true if at least one character of the given string is present in m_excludes
	bool ContainsExcludedCharacters(const wxString& val) const;

	// returns the error message if the contents of 'val' are invalid
	virtual wxString IsValid(const wxString& val) const;

protected:
	
	long                 m_validatorStyle;
	wxString             m_regexPattern;
	wxString*            m_stringValue;
	wxArrayString        m_includes;
	wxArrayString        m_excludes;

private:
	
	wxDECLARE_NO_ASSIGN_CLASS(wxRegexValidator);
	wxDECLARE_DYNAMIC_CLASS(wxRegexValidator);
	wxDECLARE_EVENT_TABLE();
};

#endif
// wxUSE_VALIDATORS && (wxUSE_TEXTCTRL || wxUSE_COMBOBOX)

