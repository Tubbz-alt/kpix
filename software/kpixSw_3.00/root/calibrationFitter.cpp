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
#include <Data.h>
#include <DataRead.h>
#include <math.h>
#include <fstream>
using namespace std;

// Channel data
class ChannelData {
   public:

      // Baseline Data
      uint         baseData[8192];
      uint         baseMin;
      uint         baseMax;
      double       baseCount;
      double       baseMean;
      double       baseSum;
      double       baseRms;

      // Calib Data
      double       calibCount[256];
      double       calibMean[256];
      double       calibSum[256];
      double       calibRms[256];

      ChannelData() {
         uint x;

         for (x=0; x < 8192; x++) baseData[x] = 0;
         baseMin    = 8192;
         baseMax    = 0;
         baseCount  = 0;
         baseMean   = 0;
         baseSum    = 0;
         baseRms    = 0;

         for (x=0; x < 256; x++) {
            calibCount[x]  = 0;
            calibMean[x]   = 0;
            calibSum[x]    = 0;
            calibRms[x]    = 0;
         }
      }

      void addBasePoint(uint data) {
         baseData[data]++;
         if ( data < baseMin ) baseMin = data;
         if ( data > baseMax ) baseMax = data;
         baseCount++;

         double tmpM = baseMean;
         double value = data;

         baseMean += (value - tmpM) / baseCount;
         baseSum  += (value - tmpM) * (value - baseMean);
      }

      void addCalibPoint(uint x, uint y) {
         calibCount[x]++;

         double tmpM = calibMean[x];
         double value = y;

         calibMean[x] += (value - tmpM) / calibCount[x];
         calibSum[x]  += (value - tmpM) * (value - calibMean[x]);
      }

      void compute() {
         uint x;

         if ( baseCount > 0 ) baseRms = sqrt(baseSum / baseCount);
         for (x=0; x < 256; x++) {
            if ( calibCount[x] > 0 ) calibRms[x] = sqrt(calibSum[x] / calibCount[x]);
         }
      }
};

// Function to compute calibration charge
double calibCharge ( uint dac, bool positive, bool highCalib ) {
   double volt;
   double charge;

   if ( dac >= 0xf6 ) volt = 2.5 - ((double)(0xff-dac))*50.0*0.0001;
   else volt =(double)dac * 100.0 * 0.0001;

   if ( positive ) charge = (2.5 - volt) * 200e-15;
   else charge = volt * 200e-15;

   if ( highCalib ) charge *= 22.0;

   return(charge);
}

// Process the data
int main ( int argc, char **argv ) {
   DataRead               dataRead;
   double                 fileSize;
   double                 filePos;
   KpixEvent              event;
   KpixSample             *sample;
   string                 calState;
   uint                   calChannel;
   uint                   calDac;
   uint                   lastPct;
   uint                   currPct;
   bool                   chanFound[1024];
   ChannelData            *chanData[32][1024][4][2];
   bool                   kpixFound[32];
   uint                   kpixMax;
   uint                   minChan;
   uint                   maxChan;
   uint                   x;
   uint                   range;
   uint                   value;
   uint                   kpix;
   uint                   channel;
   uint                   bucket;
   uint                   time;
   string                 serial;
   KpixSample::SampleType type;
   TH1F                   *hist;
   stringstream           tmp;
   ofstream               xml;
   double                 grX[256];
   double                 grY[256];
   double                 grYErr[256];
   double                 grXErr[256];
   uint                   grCount;
   TGraphErrors           *grCalib;
   bool                   positive;
   bool                   b0CalibHigh;
   uint                   injectTime[5];
   uint                   eventCount;
   string                 outRoot;
   string                 outXml;
   TFile                  *rFile;
   TCanvas                *c1;

   // Init structure
   for (kpix=0; kpix < 32; kpix++) {
      for (channel=0; channel < 1024; channel++) {
         for (bucket=0; bucket < 4; bucket++) {
            chanData[kpix][channel][bucket][0] = NULL;
            chanData[kpix][channel][bucket][1] = NULL;
         }
         chanFound[channel] = false;
      }
      kpixFound[kpix] = false;
   }

   // Data file is the first and only arg
   if ( argc != 2 ) {
      cout << "Usage: calibrationFitter data_file\n";
      return(1);
   }

   // Open data file
   if ( ! dataRead.open(argv[1]) ) {
      cout << "Error opening data file " << argv[1] << endl;
      return(1);
   }

   // Create output names
   tmp.str("");
   tmp << argv[1] << ".root";
   outRoot = tmp.str();
   tmp.str("");
   tmp << argv[1] << ".xml";
   outXml = tmp.str();

   //////////////////////////////////////////
   // Read Data
   //////////////////////////////////////////
   cout << "Opened data file: " << argv[1] << endl;
   fileSize = dataRead.size();
   filePos  = dataRead.pos();

   // Init
   currPct    = 0;
   lastPct    = 100;
   eventCount = 0;
   minChan    = 0;
   maxChan    = 0;

   cout << "\rReading File: 0 %" << flush;

   // Process each event
   while ( dataRead.next(&event) ) {

      // Get calibration state
      calState   = dataRead.getStatus("CalState");
      calChannel = dataRead.getStatusInt("CalChannel");
      calDac     = dataRead.getStatusInt("CalDac");

      // Get injection times
      if ( eventCount == 0 ) {
         minChan       = dataRead.getConfigInt("CalChanMin");
         maxChan       = dataRead.getConfigInt("CalChanMax");
         injectTime[0] = dataRead.getConfigInt("cntrlFpga:kpixAsic:Cal0Delay");
         injectTime[1] = dataRead.getConfigInt("cntrlFpga:kpixAsic:Cal1Delay") + injectTime[0] + 4;
         injectTime[2] = dataRead.getConfigInt("cntrlFpga:kpixAsic:Cal2Delay") + injectTime[1] + 4;
         injectTime[3] = dataRead.getConfigInt("cntrlFpga:kpixAsic:Cal3Delay") + injectTime[2] + 4;
         injectTime[4] = 8192;
      }

      // get each sample
      for (x=0; x < event.count(); x++) {

         // Get sample
         sample  = event.sample(x);
         kpix    = sample->getKpixAddress();
         channel = sample->getKpixChannel();
         bucket  = sample->getKpixBucket();
         value   = sample->getSampleValue();
         type    = sample->getSampleType();
         time    = sample->getSampleTime();
         range   = sample->getSampleRange();

         // Only process real samples in the expected range
         if ( type == KpixSample::Data ) {

            // Create entry if it does not exist
            if ( chanData[kpix][channel][bucket][range] == NULL ) chanData[kpix][channel][bucket][range] = new ChannelData;
            kpixFound[kpix]    = true;
            chanFound[channel] = true;

            // Filter for time
            if ( time > injectTime[bucket] && time < injectTime[bucket+1] ) {

               // Baseline
               if ( calState == "Baseline" ) chanData[kpix][channel][bucket][range]->addBasePoint(value);

               // Injection
               else if ( calState == "Inject" && channel == calChannel ) 
                  chanData[kpix][channel][bucket][range]->addCalibPoint(calDac, value);
            }
         }
      }

      // Show progress
      filePos  = dataRead.pos();
      currPct = (uint)((filePos / fileSize) * 100.0);
      if ( currPct != lastPct ) {
         cout << "\rReading File: " << currPct << " %      " << flush;
         lastPct = currPct;
      }
      eventCount++;
   }
   cout << "\rReading File: Done.               " << endl;

   //////////////////////////////////////////
   // Process Data
   //////////////////////////////////////////
   gStyle->SetOptFit(1111);

   // Default canvas
   c1 = new TCanvas("c1","c1");

   // Open root file
   rFile = new TFile(outRoot.c_str(),"recreate");

   // Open xml file
   xml.open(outXml.c_str(),ios::out | ios::trunc);
   xml << "<calibrationData>" << endl;

   // get calibration mode variables for charge computation
   positive    = (dataRead.getConfig("cntrlFpga:kpixAsic:CntrlPolarity") == "Positive");
   b0CalibHigh = (dataRead.getConfig("cntrlFpga:kpixAsic:CntrlCalibHigh") == "True");

   // Kpix count;
   for (kpix=0; kpix<32; kpix++) if ( kpixFound[kpix] ) kpixMax=kpix;

   // Process each kpix device
   for (kpix=0; kpix<32; kpix++) {
      if ( kpixFound[kpix] ) {

         // Get serial number
         tmp.str("");
         tmp << "cntrlFpga(0):kpixAsic(" << dec << kpix << "):SerialNumber";
         serial = dataRead.getConfig(tmp.str());
         xml << "   <kpixAsic id=\"" << serial << "\">" << endl;

         // Process each channel
         for (channel=minChan; channel <= maxChan; channel++) {

            // Show progress
            cout << "\rProcessing kpix " << dec << kpix << " / " << dec << kpixMax
                 << ", Channel " << channel << " / " << dec << maxChan 
                 << "                 " << flush;

            // Channel is valid
            if ( chanFound[channel] ) {

               // Start channel marker
               xml << "      <Channel id=\"" << channel << "\">" << endl;

               // Each bucket
               for (bucket = 0; bucket < 4; bucket++) {
               
                  // Bucket is valid
                  if ( chanData[kpix][channel][bucket][0] != NULL || chanData[kpix][channel][bucket][0] != NULL ) {
                     xml << "         <Bucket id=\"" << bucket << "\">" << endl;

                     // Each range
                     for (range = 0; range < 2; range++) {
 
                        // Range is valid
                        if ( chanData[kpix][channel][bucket][range] != NULL ) {
                           xml << "            <Range id=\"" << range << "\">" << endl;
                           chanData[kpix][channel][bucket][range]->compute();

                           // Create histogram
                           tmp.str("");
                           tmp << "hist_" << serial << "_c" << dec << setw(4) << setfill('0') << channel;
                           tmp << "_b" << dec << bucket;
                           tmp << "_r" << dec << range;
                           hist = new TH1F(tmp.str().c_str(),tmp.str().c_str(),8192,0,8192);

                           // Fill histogram
                           for (x=0; x < 8192; x++) hist->SetBinContent(x+1,chanData[kpix][channel][bucket][range]->baseData[x]);
                           hist->GetXaxis()->SetRangeUser(chanData[kpix][channel][bucket][range]->baseMin,
                                                          chanData[kpix][channel][bucket][range]->baseMax);
                           hist->Fit("gaus","q");
                           hist->Write();

                           // Add to xml
                           xml << "               <BaseMean>" << chanData[kpix][channel][bucket][range]->baseMean << "</BaseMean>" << endl;
                           xml << "               <BaseRms>" << chanData[kpix][channel][bucket][range]->baseRms << "</BaseRms>" << endl;
                           xml << "               <BaseFitMean>" << hist->GetFunction("gaus")->GetParameter(1) << "</BaseFitMean>" << endl;
                           xml << "               <BaseFitSigma>" << hist->GetFunction("gaus")->GetParameter(2) << "</BaseFitSigma>" << endl;
                           xml << "               <BaseFitMeanErr>" << hist->GetFunction("gaus")->GetParError(1) << "</BaseFitMeanErr>" << endl;
                           xml << "               <BaseFitSigmaErr>" << hist->GetFunction("gaus")->GetParError(2) << "</BaseFitSigmaErr>" << endl;

                           // Create calibration graph
                           grCount = 0;
                           for (x=0; x < 256; x++) {
                           
                              // Calibration point is valid
                              if ( chanData[kpix][channel][bucket][range]->calibCount[x] > 0 ) {
                                 grX[grCount]    = calibCharge ( x, positive, ((bucket==0)?b0CalibHigh:false));
                                 grY[grCount]    = chanData[kpix][channel][bucket][range]->calibMean[x];
                                 grYErr[grCount] = chanData[kpix][channel][bucket][range]->calibRms[x];
                                 grXErr[grCount] = 0;
                                 grCount++;
                              }
                           }

                           // Create graph
                           if ( grCount > 0 ) {
                              grCalib = new TGraphErrors(grCount,grX,grY,grXErr,grYErr);
                              grCalib->Draw("Ap");
                              grCalib->Fit("pol1","q");
                              grCalib->GetFunction("pol1")->SetLineWidth(1);

                              // Create name and write
                              tmp.str("");
                              tmp << "calib_" << serial << "_c" << dec << setw(4) << setfill('0') << channel;
                              tmp << "_b" << dec << bucket;
                              tmp << "_r" << dec << range;
                              grCalib->SetTitle(tmp.str().c_str());
                              grCalib->Write(tmp.str().c_str());

                              // Add to xml
                              xml << "               <CalibGain>" << grCalib->GetFunction("pol1")->GetParameter(1) << "</CalibGain>" << endl;
                              xml << "               <CalibIntercept>" << grCalib->GetFunction("pol1")->GetParameter(0) << "</CalibIntercept>" << endl;
                              xml << "               <CalibGainErr>" << grCalib->GetFunction("pol1")->GetParError(1) << "</CalibGainErr>" << endl;
                              xml << "               <CalibInterceptErr>" << grCalib->GetFunction("pol1")->GetParError(0) << "</CalibInterceptErr>" << endl;
                           }
                           xml << "            </Range>" << endl;
                        }
                     }
                     xml << "         </Bucket>" << endl;
                  }
               }
               xml << "      </Channel>" << endl;
            }
         }
         xml << "   </kpixAsic>" << endl;
      }
   }
   cout << endl;
   cout << "Wrote root plots to " << outRoot << endl;
   cout << "Wrote xml data to " << outXml << endl;

   xml << "</calibrationData>" << endl;
   xml.close();
   delete rFile;

   // Cleanup
   for (kpix=0; kpix < 32; kpix++) {
      for (channel=0; channel < 1024; channel++) {
         for (bucket=0; bucket < 4; bucket++) {
            if ( chanData[kpix][channel][bucket][0] != NULL ) delete chanData[kpix][channel][bucket][0];
            if ( chanData[kpix][channel][bucket][1] != NULL ) delete chanData[kpix][channel][bucket][1];
         }
      }
   }

   // Close file
   dataRead.close();
   return(0);
}

