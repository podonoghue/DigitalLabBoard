/*! \file
    \brief Flash Programming App

    CpldProgrammer.cpp

    \verbatim
    USBDM
    Copyright (C) 20019 Peter O'Donoghue

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    \endverbatim

    \verbatim
   Change History
   -=========================================================================================
   | 10 Dec 2019 | Initial work                                              - pgo
   +=========================================================================================
   \endverbatim
 */
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/wx.h>
#include <wx/cmdline.h>
#include <wx/stdpaths.h>

#include "ProgrammerDialogue.h"

using namespace std;

#define CONFIG_FILE_NAME "Bootloader"

// Declare the application class
class CpldProgrammer : public wxApp {
   DECLARE_CLASS( CpldProgrammer )
   DECLARE_EVENT_TABLE()

private:
   ProgrammerDialogue *dialogue = nullptr;

public:
   CpldProgrammer();

   // Called on application startup
   virtual bool OnInit();
   virtual int  OnExit();
   virtual int  OnRun();
   virtual ~CpldProgrammer();
};

// Implements CpldProgrammer& GetApp()
DECLARE_APP(CpldProgrammer)
IMPLEMENT_APP(CpldProgrammer)

/*
 * CpldProgrammer type definition
 */
IMPLEMENT_CLASS(CpldProgrammer, wxApp)

/*
 * CpldProgrammer event table definition
 */
BEGIN_EVENT_TABLE( CpldProgrammer, wxApp )

END_EVENT_TABLE()

CpldProgrammer::CpldProgrammer() {
}

bool CpldProgrammer::OnInit() {

   SetAppName(_("bootloader")); // So application files are kept in the correct directory

   // Create the main application window
   dialogue = new ProgrammerDialogue();

   SetTopWindow((wxWindow*)dialogue);
   dialogue->ShowModal();
   dialogue->Destroy();
   return true;
}

int CpldProgrammer::OnRun(void) {
   wxApp::OnRun();
   return 0;
}

int CpldProgrammer::OnExit(void) {
   return wxApp::OnExit();
}

CpldProgrammer::~CpldProgrammer() {
}

