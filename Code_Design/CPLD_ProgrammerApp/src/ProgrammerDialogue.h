/*
 * BootloaderDialogue.h
 *
 *  Created on: 9 Dec 2019
 *      Author: podonoghue
 */

#ifndef PROGRAMMERDIALOGUE_H_
#define PROGRAMMERDIALOGUE_H_

#include "ProgrammerDialogueSkeleton.h"
#include "XsvfLoader.h"
#include "Xsvf.h"
#include "DeviceInformation.h"

#include "wx/config.h"

class ProgrammerDialogue : public ProgrammerDialogueSkeleton {

private:
   ProgrammerDialogue(const ProgrammerDialogue &other) = delete;
   ProgrammerDialogue(ProgrammerDialogue &&other) = delete;
   ProgrammerDialogue& operator=(const ProgrammerDialogue &other) = delete;
   ProgrammerDialogue& operator=(ProgrammerDialogue &&other) = delete;

   Xsvf        xsvf;
   time_t      xsvf_file_modified_time = 0;
   wxString    xsvf_fileName;
   wxString    xsvf_filePath;

   // Last file opened
   wxString defaultFilename;

   // Last directory used
   wxString currentDirectory;

   // Application persistent settings
   wxConfig *config = nullptr;

   const DeviceInformation *deviceInformation = nullptr;

   const char *confirmDevice();

public:
   ProgrammerDialogue();
   virtual ~ProgrammerDialogue();

protected:
   /**
    * Get device name from IDCODE
    *
    * @param idcode
    * @return
    */
   const char *getDeviceName(uint32_t idcode);

   /**
    * Load Programming file - Either .jed or .xsvf
    *
    * @param filePath   Path to file (including file name)
    * @param fileName   File name portion only e.g. xxx.jed
    *
    * @return True  => File successfully loaded
    * @return False => File load failed
    */
   bool loadProgrammingFile(wxString filePath, wxString fileName);

   virtual void onSelectFile(wxCommandEvent &event) override;
   virtual void onConfirmId( wxCommandEvent& event ) override;
   virtual void onProgramDevice(wxCommandEvent &event) override;

   bool fileIsCurrent();

};

#endif /* PROGRAMMERDIALOGUE_H_ */
