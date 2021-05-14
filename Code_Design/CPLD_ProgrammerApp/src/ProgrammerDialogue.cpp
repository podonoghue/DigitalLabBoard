/*
 * BootloaderDialogue.cpp
 *
 *  Created on: 9 Dec 2019
 *      Author: podonoghue
 */

#include "ProgrammerDialogue.h"

#include "wx/file.h"
#include "wx/filedlg.h"
#include "wx/msgdlg.h"

#include "XsvfLoader.h"
#include "JedecFile.h"
#include "Xsvf.h"
#include "XsvfWriter.h"
#include "DeviceMapFile.h"

ProgrammerDialogue::ProgrammerDialogue() : ProgrammerDialogueSkeleton(nullptr) {
}

ProgrammerDialogue::~ProgrammerDialogue() {
}

bool isSuffix(const char *name, const char *suffix) {
   unsigned sName   = strlen(name);
   unsigned sSuffix = strlen(suffix);

   if (sSuffix > sName) {
      return false;
   }
   bool found = true;
   do {
      if (name[--sName] != suffix[--sSuffix]) {
         found = false;
         break;
      }
   } while (sSuffix>0);
   return found;
}

const char *createXsvfFromJedec(const char *jedec_filepath, XsvfWriter &xsvfWriter) {

   JedecFile jedecFile;

   const char *msg = jedecFile.loadFile(jedec_filepath);
   if (msg != nullptr) {
      fprintf(stderr, "Failed to load file '%s', reason = %s\n", jedec_filepath, msg);
      fflush(stderr);
      return msg;
   }

//   jedecFile.report();
//   jedecFile.jedecPrint();

//   fprintf(stderr, "Loaded file '%s'\n", jedec_filepath);
//   fflush(stderr);

   const DeviceInformation *deviceInformation = DeviceInformation::findDevice(jedecFile.getDeviceName());

//   jedecFile.report();

   if (deviceInformation == nullptr) {
      fprintf(stderr, "Failed to get device information for %s\n", jedecFile.getDeviceName());
      fflush(stderr);
      return "Unknown device type";
   }
   DeviceMapFile deviceMapFile(*deviceInformation);
   msg = deviceMapFile.loadFile(jedecFile.getFuseLimit());
   if (msg != nullptr) {
      fprintf(stderr, "Failed to load device map file '%s', reason = %s", jedec_filepath, msg);
      fflush(stderr);
      return "Failed to load device map file";
   }
//   deviceMapFile.printMap();

   jedecFile.rearrangeProgrammingData(deviceMapFile);
//   jedecFile.printProgrammingData();

   xsvfWriter.clear();
   xsvfWriter.doPreamble();
   xsvfWriter.confirmIdcode(*deviceInformation);
   xsvfWriter.doErase();
   xsvfWriter.program(jedecFile, *deviceInformation);
   xsvfWriter.verify(jedecFile, *deviceInformation);
   xsvfWriter.doPostamble();
   return nullptr;
}

/**
 * Load Programming file - Either .jed or .xsvf
 *
 * @param filePath   Path to file (including file name)
 * @param fileName   File name portion only e.g. abc.jed
 *
 * @return True  => File successfully loaded
 * @return False => File load failed
 */
bool ProgrammerDialogue::loadProgrammingFile(wxString filePath, wxString fileName) {

   wxFile file;
   const char *msg = nullptr;

//   fprintf(stderr, "Loading %s\n", (const char*)filePath.c_str());
//   fprintf(stderr, "Loading %s\n", (const char*)fileName.c_str());
   fflush(stderr);

   xsvf.clear();

   do {
      if (isSuffix(fileName.c_str(), ".xsvf")) {
         msg = xsvf.readFile(filePath.c_str());
      }
      else if (isSuffix(fileName.c_str(), ".jed")) {
         XsvfWriter xsvfWriter;
         msg = createXsvfFromJedec(filePath.c_str(), xsvfWriter);
         if (msg == nullptr) {
            xsvf = xsvfWriter.getXsvf();
         }
      }
      else {
         msg = "File type not recognized";
      }
      if (msg != nullptr) {
         continue;
      }
   } while (false);

   if (msg != nullptr) {
      xsvf_file_modified_time = 0;
      xsvf_filePath = "";
      xsvf_fileName = "";

      loadedFile_static->SetLabel("-- No file loaded --");
      programDevice_button->Enable(false);

      wxMessageBox(msg, "Failed to load file");
      return false;
   }
   else {
      // Get file modification time
      xsvf_file_modified_time = wxFileModificationTime(filePath);
      xsvf_filePath = filePath;
      xsvf_fileName = fileName;
      wxDateTime time(xsvf_file_modified_time);
      time.FormatISOCombined(':');
      loadedFile_static->SetLabel(wxString::Format("%s (%s)", fileName, time.FormatISOCombined(' ')));
      programDevice_button->Enable(true);
      return true;
   }
}

bool ProgrammerDialogue::fileIsCurrent() {
   // Get file modification time
   long file_modified_time = wxFileModificationTime(xsvf_filePath);
   return (file_modified_time <= xsvf_file_modified_time);
}

void ProgrammerDialogue::onLoadFile(wxCommandEvent &event) {
   wxString caption  = _("Select Binary File to Load");
   wxString wildcard = _(
         "Programming Files(*.jed;*.xsvf)|*.jed;*.xsvf|"
         "Jedec Files(*.jed)|*.jed|"
         "Compressed SVF (*.xsvf)|*.xsvf|"
         "All Files|*");
   wxFileDialog openFileDialog(this, caption, currentDirectory, defaultFilename, wildcard, wxFD_OPEN);
   int getCancelOK = openFileDialog.ShowModal();
   if (getCancelOK != wxID_OK) {
      // Ignore
      return;
   }
   // Save for next time
   currentDirectory = openFileDialog.GetDirectory();
   defaultFilename  = openFileDialog.GetFilename();

   wxString filePath = openFileDialog.GetPath();
   wxString fileName = openFileDialog.GetFilename();

   programAction_static->SetLabel("");
   idcode_static->SetLabel("");

   loadProgrammingFile(filePath, fileName);
}

/**
 * Get device name from IDCODE
 *
 * @param idcode
 * @return
 */
const char *ProgrammerDialogue::getDeviceName(uint32_t idcode) {
   struct DeviceId {
      uint32_t mask;
      uint32_t idcode;
      const char *name;
   };

   static const DeviceId deviceIds[] = {
         { 0x0FFF8FFF, 0xF6E5F093, "XC2C64A"},
   };

   static const char *name = "Unknown";

   for(unsigned index=0; index<(sizeof(deviceIds)/sizeof(deviceIds[0])); index++) {
      fprintf(stderr, "m=0x%08X, i=0x%08X\n",
            deviceIds[index].idcode&deviceIds[index].mask, idcode&deviceIds[index].mask);
      fflush(stderr);
      if ((idcode&deviceIds[index].mask) == (deviceIds[index].idcode&deviceIds[index].mask)) {
         name = deviceIds[index].name;
         break;
      }
   }
   return name;
}

const char *ProgrammerDialogue::confirmDevice() {
   XsvfLoader loader;
   uint32_t idcode;

   const char *res   = nullptr;
   deviceInformation = nullptr;

   fprintf(stderr, "onConfirmId\n");
   fflush(stderr);
   do {
      res = loader.checkTargetVref();
      if (res != nullptr) {
         break;
      }
      res = loader.readIdcode(idcode);
      if (res != nullptr) {
         break;
      }
      deviceInformation = DeviceInformation::findDevice(idcode);
      if (deviceInformation == nullptr) {
         res = "Unknown device";
         loader.setFailed();
      }
   } while (false);

   if (res != nullptr) {
      fprintf(stderr, "confirmDevice - %s\n", res);
      fflush(stderr);
      idcode_static->SetLabel("Failed to identify device");
      return res;
   }
   else {
      fprintf(stderr, "confirmDevice - %s (IDCODE = 0x%08X)\n", deviceInformation->getName(), idcode);
      fflush(stderr);
      idcode_static->SetLabel(wxString::Format("%s (IDCODE = 0x%08X)", deviceInformation->getName(), idcode));
      return nullptr;
   }
}

void ProgrammerDialogue::onConfirmId( wxCommandEvent& event ) {

   const char *msg = confirmDevice();
   if (msg != nullptr) {
      wxMessageBox(msg, "Failed to identify device", wxOK | wxCENTRE | wxICON_ERROR);
   }
}

void ProgrammerDialogue::onProgramDevice(wxCommandEvent &event) {
   programAction_static->SetLabel("Busy");

   if (!fileIsCurrent() && !loadProgrammingFile(xsvf_filePath, xsvf_fileName)) {
      // File reload failed
      return;
   }

   XsvfLoader loader;
   const char *msg = nullptr;
   do {
      msg = loader.checkTargetVref();
      if (msg != nullptr) {
         break;
      }
      msg = confirmDevice();
      if (msg != nullptr) {
         break;
      }
      msg = loader.executeXsvf(xsvf);
      if (msg != nullptr) {
         break;
      }
   } while (false);

   if (msg != nullptr) {
      wxMessageBox(msg, "Failed to program device", wxOK | wxCENTRE | wxICON_ERROR);
      programAction_static->SetLabel("Programming Failed");
   }
   else {
      programAction_static->SetLabel("Programming Succeeded");
   }
}
