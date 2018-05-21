//-----------------------------------------------------------------------------
// File          : readExample.cpp
// Author        : Ryan Herbst  <rherbst@slac.stanford.edu>
// Created       : 12/02/2011
// Project       : Kpix DAQ
//-----------------------------------------------------------------------------
// Description :
// Read data example
//-----------------------------------------------------------------------------
// Copyright (c) 2011 by SLAC. All rights reserved.
// Proprietary and confidential to SLAC.
//-----------------------------------------------------------------------------
// Modification history :
// 12/02/2011: created
//----------------------------------------------------------------------------
#include <KpixEvent.h>
#include <KpixSample.h>
#include <KpixCalibRead.h>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <Data.h>
#include <DataRead.h>
using namespace std;

int main (int argc, char **argv) {
   DataRead      dataRead;
   KpixEvent     event;
   KpixSample    *sample;
   uint          x;
   uint          count;
   stringstream  tmp;
   string        serialList[32];
   string        serial;
   uint          sampleCnt[9];
   uint          minTime[9];
   uint          maxTime[9];
   uint          rangeCnt[9];
   uint          addr;
   uint          chan;
   uint          buck;
   uint          range;
   int           uid;
   time_t        curr, last;

   // Check args
   if ( argc < 1 ) {
      cout << "Usage: readShared [uid]" << endl;
      return(1);
   }

   if ( argc == 2 ) uid = atoi(argv[1]);
   else uid = -1;

   cout << "Id=" << dec << uid << endl;

   dataRead.openShared("kpix",1,uid);

   // Process each event
   time(&curr);
   last = curr;
   count = 0;

   for (x=0; x < 9; x++) {
      sampleCnt[x] = 0;
      minTime[x]   = 9999;
      maxTime[x]   = 0;
      rangeCnt[x]  = 0;
   }

   while (1) {

      time(&curr);
      if ( last != curr ) {
         cout << "Got " << dec << setw(4) << count << " events, ";

         for (x=0; x < 9; x++) cout << dec << setw(12) << setfill(' ') << sampleCnt[x];
         cout << endl;

         cout << "                 ";
         for (x=0; x < 9; x++) {
            if ( minTime[x] != 9999 ) cout << dec << setw(7) << setfill(' ') << minTime[x];
            else cout << dec << setw(7) << setfill(' ') << 0;
            cout << "-";
            if ( maxTime[x] != 0 ) cout << dec << setw(4) << setfill(' ') << maxTime[x];
            else cout << dec << setw(4) << setfill(' ') << 0;
         }
         cout << endl;

         cout << "                 ";
         for (x=0; x < 9; x++) cout << dec << setw(12) << setfill(' ') << rangeCnt[x];
         cout << endl << endl;

         last = curr;
         count = 0;
         for (x=0; x < 9; x++) {
            sampleCnt[x] = 0;
            minTime[x]   = 9999;
            maxTime[x]   = 0;
            rangeCnt[x]  = 0;
         }
      }

      if ( dataRead.next(&event) ) {

         //if ( dataRead.sawRunStop()  ) dataRead.dumpRunStop();
         //if ( dataRead.sawRunStart() ) dataRead.dumpRunStart();
         //if ( dataRead.sawRunTime()  ) dataRead.dumpRunTime();

         // Extract kpix serial numbers after reading first event
         if ( count == 0 ) {
            for (x=0; x < 32; x++) {
               tmp.str("");
               tmp << "cntrlFpga(0):kpixAsic(" << dec << x << "):SerialNumber";
               serialList[x] = dataRead.getConfig(tmp.str());
            }
         }

         for (x=0; x < event.count(); x++) {
            sample = event.sample(x);
            addr   = sample->getKpixAddress();
            chan   = sample->getKpixChannel();
            buck   = sample->getKpixBucket();
            range  = sample->getSampleRange();

            if ( sample->getSampleType() == KpixSample::Data ) {
               sampleCnt[addr]++;
               if ( minTime[addr] > sample->getSampleTime() ) minTime[addr] = sample->getSampleTime();
               if ( maxTime[addr] < sample->getSampleTime() ) maxTime[addr] = sample->getSampleTime();
               if ( range ) rangeCnt[addr]++;
            }
         }

         count++;
      }
      else usleep(100);
   }

   return(0);
}

