//============================================================================
// Name        : Convert_XSVF_C.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include "JedecFile.h"
#include "DeviceMapFile.h"
#include "XsvfParser.h"
#include "XsvfWriter.h"

const char *createXsvfFromJedec(const char *jedec_filename, XsvfWriter &xsvfWriter) {

   JedecFile jedecFile;

   const char *msg = jedecFile.loadFile(jedec_filename);
   if (msg != nullptr) {
      fprintf(stderr, "Failed to load file '%s', reason = %s", jedec_filename, msg);
      return msg;
   }

//   jedecFile.report();
//   jedecFile.jedecPrint();

   const DeviceInformation *deviceInformation = DeviceInformation::findDevice(jedecFile.getDeviceName());

   if (deviceInformation == nullptr) {
      fprintf(stderr, "Failed to get device information for %s\n", jedecFile.getDeviceName());
      return "Unknown device type";
   }
   DeviceMapFile deviceMapFile(*deviceInformation);
   deviceMapFile.loadFile(jedecFile.getFuseLimit());
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
   return 0;
}

int main(int argc, const char *args[]) {

   JedecFile jedecFile;

   const char *jedec_filename = "cpld_tester.jed";
   const char *xsvf_filename  = "cpld_tester";

   XsvfWriter xsvfWriter;
   createXsvfFromJedec(jedec_filename, xsvfWriter);

   Xsvf xsvf(xsvfWriter.getXsvf());
   xsvf.writeFile("xxx.xsvf");

   XsvfParser_Array xsvfParser(xsvf_filename, xsvf.getSize(), xsvf.getData());
   xsvfParser.parseAll();

   return 0;
}
