//-----------------------------------------------------------------------------
// File          : calibrationFitter.cpp
// Author        : Ryan Herbst  <rherbst@slac.stanford.edu>
// Created       : 05/30/2012
// Project       : Kpix Software Package
//-----------------------------------------------------------------------------
// Description :
// Application to process and fit kpix calibration data
//-----------------------------------------------------------------------------
// Copyright (c) 2012 by SLAC. All rights reserved.
// Proprietary and confidential to SLAC.
//-----------------------------------------------------------------------------
// Modification history :
// 05/30/2012: created
//-----------------------------------------------------------------------------
#include <iostream>
#include <iomanip>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TMultiGraph.h>
#include <TApplication.h>
#include <TGraphErrors.h>
#include <TGraph.h>
#include <TStyle.h>
#include <stdarg.h>
#include <KpixEvent.h>
#include <KpixSample.h>
#include <KpixCalibRead.h>
#include <Data.h>
#include <DataRead.h>
#include <math.h>
#include <fstream>
#include <XmlVariables.h>
using namespace std;

//From http://www.richelbilderbeek.nl/CppRemovePath.htm
//Returns the filename without path
const std::string RemovePath(const std::string& filename)
{
  const int sz = static_cast<int>(filename.size());
  const int path_sz = filename.rfind("/",filename.size());
  if (path_sz == sz) return filename;
  return filename.substr(path_sz + 1,sz - 1 - path_sz);
}

const char getChannelMode ( uint kpix, uint channel, DataRead *dread ) {
   char buffer[100];

   uint base = (channel / 32) * 32;
   uint top  = base + 31;

   sprintf(buffer,"cntrlFpga(0):kpixAsic(%i):Chan_%04i_%04i",kpix,base,top);

   string conf = dread->getConfig(buffer); 

   uint group    = (channel % 32) / 8;
   uint offset   = (channel % 32) % 8;
   uint position = (channel * 8) + group + offset;

   return(conf.at(position));
}

const bool goodChannel ( uint kpix, uint channel ) {

   bool good = true;

   // Global bad channels
   if ( channel == 13  || channel == 14  || channel == 15   || channel == 41   || channel == 54  || channel == 55  ||
        channel == 256 || channel == 257 || channel == 258  || channel == 259  || channel == 265 || channel == 266 ||
        channel == 267 || channel == 268 || channel == 284  || channel == 285  || channel == 286 || channel == 287 ||
        channel == 736 || channel == 737 || channel == 738  || channel == 739  || channel == 745 || channel == 746 ||
        channel == 747 || channel == 748 || channel == 764  || channel == 765  || channel == 766 || channel == 767 ||
        channel == 983 || channel == 984 || channel == 1005 || channel == 1006 || channel == 1007 ) good = false;

   // Kpix 0 bad channels
   if ( kpix == 0 && ( channel == 3  || channel == 4  || channel == 8  || channel == 13 || channel == 14 || channel == 15 || channel == 21 ||
                       channel == 24 || channel == 26 || channel == 32 || channel == 38 || channel == 41 || channel == 45 || channel == 47 ||
                       channel == 49 || channel == 54 || channel == 55 || channel == 57 || channel == 58 || channel == 60 || channel == 61 ||
                       channel == 62 || channel == 63 ) ) good = false;

   // Kpix 2 bad channels
   if ( kpix == 2 && ( channel == 1005 || channel == 1006 || channel == 1007 || channel == 1009 )) good = false;

   // Kpix 7 bad channels
   if ( kpix == 7 && ( channel == 13 || channel == 14 || channel == 15 || channel == 19 || channel == 27 ||
                       channel == 41 || channel == 42 || channel == 49 || channel == 52 || channel == 54 || 
                       channel == 55 || channel == 57 )) good = false;

   return(good);
}

// Process the data
int main ( int argc, char **argv ) {
   DataRead               dataRead;
   KpixCalibRead          calibRead;
   KpixEvent              event;
   KpixSample             *sample;
   uint                   calChannel;
   uint                   currPct;
   uint                   lastPct;
   uint                   eventCount;
   uint                   x;
   uint                   kpix;
   uint                   channel;
   uint                   bucket;
   uint                   value;
   uint                   type;
   uint                   tstamp;
   uint                   range;
   size_t                 filePos;
   size_t                 fileSize;
   TH1F                 * histA[9];
   TH1F                 * histB[9];
   TH1F                 * histC[9];
   TH1F                 * histD[9];
   TCanvas              * c1;
   TCanvas              * c2;
   TCanvas              * c3;
   TCanvas              * c4;
   uint                   minVal[9];
   uint                   maxVal[9];
   double                 minValC[9];
   double                 maxValC[9];
   double                 minValD[9];
   double                 maxValD[9];
   stringstream           tmp;
   uint                   count;
   string                 serialList[9];
   string                 serial;
   int                    bad;
   int                    badTar;
   double                 gain;
   double                 mean;
   double                 gainTar;
   double                 meanTar;
   double                 charge;
   uint                   lowRangeCnt;
   bool                   filterBad;
   bool                   foundBad;
   char                   temp[200];

   TApplication theApp("App",NULL,NULL);
   gStyle->SetOptFit(1111);
   gStyle->SetOptStat(111111111);

   // Data file is the first and only arg
   if ( argc != 4 ) {
      cout << "Usage: injectTestRead data_file calib_file filter_bad\n";
      return(1);
   }
   filterBad = atoi(argv[3]);

   // Attempt to open  calibration file
   if ( calibRead.parse(argv[2]) ) 
      cout << "Read calibration data from " << argv[2] << endl << endl;

   // Open data file
   if ( ! dataRead.open(argv[1]) ) {
      cout << "Error opening data file " << argv[1] << endl;
      return(1);
   }

   // Init plot
   for( x=0; x < 9; x++ ) {

      tmp.str("");
      tmp << "all layer " << dec << x << " " << RemovePath(argv[1]);
      histA[x] = new TH1F(tmp.str().c_str(),tmp.str().c_str(),8192,0,8191);

      tmp.str("");
      tmp << "zoom layer " << dec << x << " " << RemovePath(argv[1]);
      histB[x] = new TH1F(tmp.str().c_str(),tmp.str().c_str(),8192,0,8191);
      minVal[x] = 8192;
      maxVal[x] = 0;

      tmp.str("");
      tmp << "in phase charge layer " << dec << x << " " << RemovePath(argv[1]);
      histC[x] = new TH1F(tmp.str().c_str(),tmp.str().c_str(),1000,-500e-15,500e-15);
      minValC[x] = 500e-15;
      maxValC[x] = -500e-15;

      tmp.str("");
      tmp << "out of phase charge layer " << dec << x << " " << RemovePath(argv[1]);
      histD[x] = new TH1F(tmp.str().c_str(),tmp.str().c_str(),1000,-500e-15,500e-15);
      minValD[x] = 500e-15;
      maxValD[x] = -500e-15;
   }

   //////////////////////////////////////////
   // Read Data
   //////////////////////////////////////////
   cout << "Opened data file: " << argv[1] << endl;
   fileSize = dataRead.size();
   filePos  = dataRead.pos();

   // Init
   currPct          = 0;
   lastPct          = 100;
   eventCount       = 0;
   lowRangeCnt      = 0;

   cout << "\rReading File: 0 %" << flush;

   // Process each event
   count = 0;
   while ( dataRead.next(&event) ) {

      // Get serial numbers after first record
      if ( count == 0 ) {
         for (x=0; x < 9; x++) {
            tmp.str("");
            tmp << "cntrlFpga(0):kpixAsic(" << dec << x << "):SerialNumber";
            serialList[x] = dataRead.getConfig(tmp.str());
         }
      }

      // Get calibration state
      calChannel = dataRead.getConfigInt("UserDataA");

      // get each sample
      for (x=0; x < event.count(); x++) {

         // Get sample
         sample  = event.sample(x);
         kpix    = sample->getKpixAddress();
         channel = sample->getKpixChannel();
         bucket  = sample->getKpixBucket();
         value   = sample->getSampleValue();
         type    = sample->getSampleType();
         tstamp  = sample->getSampleTime();
         range   = sample->getSampleRange();

         serial = serialList[kpix];

         // Only process real samples in the expected range
         if ( type == KpixSample::Data ) {

            if ( range == 1 ) lowRangeCnt++;

            // Get gain and mean for target channel
            meanTar = calibRead.baseMean(serial,calChannel,0,0);
            gainTar = calibRead.calibGain(serial,calChannel,0,0);
            badTar  = calibRead.badChannel(serial,calChannel);

            // Get gain and mean for channel/bucket
            mean = calibRead.baseMean(serial,channel,bucket,range);
            gain = calibRead.calibGain(serial,channel,bucket,range);
            bad  = calibRead.badChannel(serial,channel);

            foundBad = (gainTar < 3e15) || (gain < 3e15) || badTar || bad || !goodChannel(kpix,calChannel) || !goodChannel(kpix,channel);

            // Only process if target channel is good and and crosstalk channel is good
            if ( !filterBad || !foundBad ) {

               // Report crosstalk, expected is 753
               if ( channel != calChannel && bucket == 0 ) {
                  histA[kpix]->Fill(tstamp);
                  histB[kpix]->Fill(tstamp);
                  if ( minVal[kpix] > tstamp ) minVal[kpix] = tstamp;
                  if ( maxVal[kpix] < tstamp ) maxVal[kpix] = tstamp;

                  // Only show hits that have valid calibration
                  if ( gain != 0 && bad == 0 ) {
                     charge = ((double)value - mean) / gain;

                     // In time hits
                     if ( tstamp <= 755 ) {
                        histC[kpix]->Fill(charge);
                        if ( minValC[kpix] > charge ) minValC[kpix] = charge;
                        if ( maxValC[kpix] < charge ) maxValC[kpix] = charge;
                     }

                     // Out of time hits
                     else {
                        histD[kpix]->Fill(charge);
                        if ( minValD[kpix] > charge ) minValD[kpix] = charge;
                        if ( maxValD[kpix] < charge ) maxValD[kpix] = charge;
                     }
                  }
               }
            }
         }
      }

      // Show progress
      filePos  = dataRead.pos();
      currPct = (uint)(((double)filePos / (double)fileSize) * 100.0);
      if ( currPct != lastPct ) {
         cout << "\rReading File: " << currPct << " %      " << flush;
         lastPct = currPct;
      }
      eventCount++;
   }
   cout << "\rReading File: Done.               " << endl;
   cout << "Low Range Count = " << dec << lowRangeCnt << endl;

   // Close file
   dataRead.close();

   c1 = new TCanvas("c1","c1");
   c2 = new TCanvas("c2","c2");
   c3 = new TCanvas("c3","c3");
   c4 = new TCanvas("c4","c4");
   c1->Divide(3,3,0.01,0.01);
   c2->Divide(3,3,0.01,0.01);
   c3->Divide(3,3,0.01,0.01);
   c4->Divide(3,3,0.01,0.01);

   for( x=0; x < 9; x++ ) {

      c1->cd(x+1)->SetLogy();
      if ( minVal[x] < 10 ) minVal[x] = 0;
      else minVal[x] += 10;
      if ( maxVal[x] > 8181 ) maxVal[x] = 8191;
      else maxVal[x] += 10;
      histA[x]->GetXaxis()->SetRangeUser(minVal[x],maxVal[x]);
      histA[x]->Draw();

      c2->cd(x+1)->SetLogy();
      histB[x]->GetXaxis()->SetRangeUser(750,850);
      histB[x]->Draw();

      c3->cd(x+1)->SetLogy();
      histC[x]->GetXaxis()->SetRangeUser(minValC[x],maxValC[x]);
      histC[x]->Draw();

      c4->cd(x+1)->SetLogy();
      histD[x]->GetXaxis()->SetRangeUser(minValD[x],maxValD[x]);
      histD[x]->Draw();
   }

   sprintf(temp,"%s_filt%i_all.ps",RemovePath(argv[1]).c_str(),filterBad);
   c1->Print(temp);
   sprintf(temp,"ps2pdf %s_filt%i_all.ps",RemovePath(argv[1]).c_str(),filterBad);
   system(temp);

   sprintf(temp,"%s_filt%i_zoom.ps",RemovePath(argv[1]).c_str(),filterBad);
   c2->Print(temp);
   sprintf(temp,"ps2pdf %s_filt%i_zoom.ps",RemovePath(argv[1]).c_str(),filterBad);
   system(temp);

   sprintf(temp,"%s_filt%i_intime.ps",RemovePath(argv[1]).c_str(),filterBad);
   c3->Print(temp);
   sprintf(temp,"ps2pdf %s_filt%i_intime.ps",RemovePath(argv[1]).c_str(),filterBad);
   system(temp);

   sprintf(temp,"%s_filt%i_outtime.ps",RemovePath(argv[1]).c_str(),filterBad);
   c4->Print(temp);
   sprintf(temp,"ps2pdf %s_filt%i_outtime.ps",RemovePath(argv[1]).c_str(),filterBad);
   system(temp);

   theApp.Run();

   return(0);
}
