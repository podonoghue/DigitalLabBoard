///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.9.0 Sep 11 2020)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/button.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ProgrammerDialogueSkeleton
///////////////////////////////////////////////////////////////////////////////
class ProgrammerDialogueSkeleton : public wxDialog
{
	private:

	protected:
		wxButton* selectFile_button;
		wxStaticText* selectedFile_static;
		wxButton* confirmId_button;
		wxStaticText* idcode_static;
		wxButton* programDevice_button;
		wxStaticText* programAction_static;

		// Virtual event handlers, overide them in your derived class
		virtual void onSelectFile( wxCommandEvent& event ) { event.Skip(); }
		virtual void onConfirmId( wxCommandEvent& event ) { event.Skip(); }
		virtual void onProgramDevice( wxCommandEvent& event ) { event.Skip(); }


	public:

		ProgrammerDialogueSkeleton( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("CPLD Programmer"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 534,287 ), long style = wxDEFAULT_DIALOG_STYLE );
		~ProgrammerDialogueSkeleton();

};

