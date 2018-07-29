/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/valtext.cpp
// Purpose:     wxRegexValidator
// Author:      Julian Smart
// Modified by: Francesco Montorsi
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_VALIDATORS && (wxUSE_TEXTCTRL || wxUSE_COMBOBOX)

#include "../include/regex_validator.h"

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/textctrl.h"
#include "wx/combobox.h"
#include "wx/utils.h"
#include "wx/msgdlg.h"
#include "wx/intl.h"
#endif

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "wx/combo.h"


// ----------------------------------------------------------------------------
// global helpers
// ----------------------------------------------------------------------------

static bool wxIsNumeric(const wxString& val)
{
	for (wxString::const_iterator i = val.begin(); i != val.end(); ++i)
	{
		// Allow for "," (French) as well as "." -- in future we should
		// use wxSystemSettings or other to do better localisation
		if ((!wxIsdigit(*i)) &&
			(*i != wxS('.')) && (*i != wxS(',')) && (*i != wxS('e')) &&
			(*i != wxS('E')) && (*i != wxS('+')) && (*i != wxS('-')))
			return false;
	}
	return true;
}

// ----------------------------------------------------------------------------
// wxRegexValidator
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxRegexValidator, wxValidator);
wxBEGIN_EVENT_TABLE(wxRegexValidator, wxValidator)
EVT_CHAR(wxRegexValidator::OnChar)
wxEND_EVENT_TABLE()

wxRegexValidator::wxRegexValidator(long style, wxString *val)
{
	m_stringValue = val;
	SetStyle(style);
}

wxRegexValidator::wxRegexValidator(const wxRegexValidator& val)
	: wxValidator()
{
	Copy(val);
}

void wxRegexValidator::SetStyle(long style)
{
	m_validatorStyle = style;

#if wxDEBUG_LEVEL
	int check;
	check = (int)HasFlag(wxREV_FILTER_ALPHA) + (int)HasFlag(wxREV_FILTER_ALPHANUMERIC) +
		(int)HasFlag(wxREV_FILTER_DIGITS) + (int)HasFlag(wxREV_FILTER_NUMERIC);
	wxASSERT_MSG(check <= 1,
		"It makes sense to use only one of the wxREV_FILTER_ALPHA/wxREV_FILTER_ALPHANUMERIC/"
		"wxREV_FILTER_SIMPLE_NUMBER/wxREV_FILTER_NUMERIC styles");

	wxASSERT_MSG(((int)HasFlag(wxREV_FILTER_INCLUDE_LIST) + (int)HasFlag(wxREV_FILTER_INCLUDE_CHAR_LIST) <= 1) &&
		((int)HasFlag(wxREV_FILTER_EXCLUDE_LIST) + (int)HasFlag(wxREV_FILTER_EXCLUDE_CHAR_LIST) <= 1),
		"Using both wxREV_FILTER_[IN|EX]CLUDE_LIST _and_ wxREV_FILTER_[IN|EX]CLUDE_CHAR_LIST "
		"doesn't work since wxRegexValidator internally uses the same array for both");

	check = (int)HasFlag(wxREV_FILTER_INCLUDE_LIST) + (int)HasFlag(wxREV_FILTER_INCLUDE_CHAR_LIST) +
		(int)HasFlag(wxREV_FILTER_EXCLUDE_LIST) + (int)HasFlag(wxREV_FILTER_EXCLUDE_CHAR_LIST);
	wxASSERT_MSG(check <= 1,
		"Using both an include/exclude list may lead to unexpected results");
#endif // wxDEBUG_LEVEL
}

bool wxRegexValidator::Copy(const wxRegexValidator& val)
{
	wxValidator::Copy(val);

	m_validatorStyle = val.m_validatorStyle;
	m_stringValue = val.m_stringValue;
	m_regexPattern = val.m_regexPattern;
	m_includes = val.m_includes;
	m_excludes = val.m_excludes;

	return true;
}

wxTextEntry *wxRegexValidator::GetTextEntry()
{
#if wxUSE_TEXTCTRL
	if (wxDynamicCast(m_validatorWindow, wxTextCtrl))
	{
		return (wxTextCtrl*)m_validatorWindow;
	}
#endif

#if wxUSE_COMBOBOX
	if (wxDynamicCast(m_validatorWindow, wxComboBox))
	{
		return (wxComboBox*)m_validatorWindow;
	}
#endif

#if wxUSE_COMBOCTRL
	if (wxDynamicCast(m_validatorWindow, wxComboCtrl))
	{
		return (wxComboCtrl*)m_validatorWindow;
	}
#endif

	wxFAIL_MSG(
		"wxRegexValidator can only be used with wxTextCtrl, wxComboBox, "
		"or wxComboCtrl"
	);

	return NULL;
}

// Called when the value in the window must be validated.
// This function can pop up an error message.
bool wxRegexValidator::Validate(wxWindow *parent)
{
	// If window is disabled, simply return
	if (!m_validatorWindow->IsEnabled())
		return true;

	wxTextEntry * const text = GetTextEntry();
	if (!text)
		return false;

	wxString val(text->GetValue());

	wxString errormsg;

	wxRegEx regex(m_regexPattern);

	// We can only do some kinds of validation once the input is complete, so
	// check for them here:
	if (HasFlag(wxREV_FILTER_EMPTY) && val.empty())
		errormsg = _("Required information entry is empty.");
	else if (HasFlag(wxREV_FILTER_INCLUDE_LIST) && m_includes.Index(val) == wxNOT_FOUND)
		errormsg = wxString::Format(_("'%s' is not one of the valid strings"), val);
	else if (HasFlag(wxREV_FILTER_EXCLUDE_LIST) && m_excludes.Index(val) != wxNOT_FOUND)
		errormsg = wxString::Format(_("'%s' is one of the invalid strings"), val);
	else if (HasFlag(wxREV_FILTER_REGEX) && regex.IsValid() && !regex.Matches(val))
		errormsg = wxString::Format(_("'%s' is not valid"), val);
	else if (!(errormsg = IsValid(val)).empty())
	{
		// NB: this format string should always contain exactly one '%s'
		wxString buf;
		buf.Printf(errormsg, val.c_str());
		errormsg = buf;
	}

	if (!errormsg.empty())
	{
		return false;
	}

	return true;
}

// Called to transfer data to the window
bool wxRegexValidator::TransferToWindow()
{
	if (m_stringValue)
	{
		wxTextEntry * const text = GetTextEntry();
		if (!text)
			return false;

		text->SetValue(*m_stringValue);
	}

	return true;
}

// Called to transfer data to the window
bool wxRegexValidator::TransferFromWindow()
{
	if (m_stringValue)
	{
		wxTextEntry * const text = GetTextEntry();
		if (!text)
			return false;

		*m_stringValue = text->GetValue();
	}

	return true;
}

// IRIX mipsPro refuses to compile wxStringCheck<func>() if func is inline so
// let's work around this by using this non-template function instead of
// wxStringCheck(). And while this might be fractionally less efficient because
// the function call won't be inlined like this, we don't care enough about
// this to add extra #ifs for non-IRIX case.
namespace
{

	bool CheckString(bool(*func)(const wxUniChar&), const wxString& str)
	{
		for (wxString::const_iterator i = str.begin(); i != str.end(); ++i)
		{
			if (!func(*i))
				return false;
		}

		return true;
	}

} // anonymous namespace

wxString wxRegexValidator::IsValid(const wxString& val) const
{
	// wxREV_FILTER_EMPTY is checked for in wxRegexValidator::Validate

	if (HasFlag(wxREV_FILTER_ASCII) && !val.IsAscii())
		return _("'%s' should only contain ASCII characters.");
	if (HasFlag(wxREV_FILTER_ALPHA) && !CheckString(wxIsalpha, val))
		return _("'%s' should only contain alphabetic characters.");
	if (HasFlag(wxREV_FILTER_ALPHANUMERIC) && !CheckString(wxIsalnum, val))
		return _("'%s' should only contain alphabetic or numeric characters.");
	if (HasFlag(wxREV_FILTER_DIGITS) && !CheckString(wxIsdigit, val))
		return _("'%s' should only contain digits.");
	if (HasFlag(wxREV_FILTER_NUMERIC) && !wxIsNumeric(val))
		return _("'%s' should be numeric.");
	if (HasFlag(wxREV_FILTER_INCLUDE_CHAR_LIST) && !ContainsOnlyIncludedCharacters(val))
		return _("'%s' doesn't consist only of valid characters");
	if (HasFlag(wxREV_FILTER_EXCLUDE_CHAR_LIST) && ContainsExcludedCharacters(val))
		return _("'%s' contains illegal characters");

	return wxEmptyString;
}

bool wxRegexValidator::ContainsOnlyIncludedCharacters(const wxString& val) const
{
	for (wxString::const_iterator i = val.begin(); i != val.end(); ++i)
		if (m_includes.Index((wxString)*i) == wxNOT_FOUND)
			// one character of 'val' is NOT present in m_includes...
			return false;

	// all characters of 'val' are present in m_includes
	return true;
}

bool wxRegexValidator::ContainsExcludedCharacters(const wxString& val) const
{
	for (wxString::const_iterator i = val.begin(); i != val.end(); ++i)
		if (m_excludes.Index((wxString)*i) != wxNOT_FOUND)
			// one character of 'val' is present in m_excludes...
			return true;

	// all characters of 'val' are NOT present in m_excludes
	return false;
}

void wxRegexValidator::SetCharIncludes(const wxString& chars)
{
	wxArrayString arr;

	for (wxString::const_iterator i = chars.begin(); i != chars.end(); ++i)
		arr.Add(*i);

	SetIncludes(arr);
}

void wxRegexValidator::SetCharExcludes(const wxString& chars)
{
	wxArrayString arr;

	for (wxString::const_iterator i = chars.begin(); i != chars.end(); ++i)
		arr.Add(*i);

	SetExcludes(arr);
}

void wxRegexValidator::OnChar(wxKeyEvent& event)
{
	// Let the event propagate by default.
	event.Skip();

	if (!m_validatorWindow)
		return;

#if wxUSE_UNICODE
	// We only filter normal, printable characters.
	int keyCode = event.GetUnicodeKey();
#else // !wxUSE_UNICODE
	int keyCode = event.GetKeyCode();
	if (keyCode > WXK_START)
		return;
#endif // wxUSE_UNICODE/!wxUSE_UNICODE

	// we don't filter special keys and delete
	if (keyCode < WXK_SPACE || keyCode == WXK_DELETE)
		return;

	wxString str((wxUniChar)keyCode, 1);
	if (IsValid(str).empty())
		return;

	if (!wxValidator::IsSilent())
		wxBell();

	// eat message
	event.Skip(false);
}


#endif
// wxUSE_VALIDATORS && (wxUSE_TEXTCTRL || wxUSE_COMBOBOX)
