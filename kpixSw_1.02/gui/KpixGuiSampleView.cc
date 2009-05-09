//-----------------------------------------------------------------------------
// File          : KpixGuiSampleView.cc
// Author        : Ryan Herbst  <rherbst@slac.stanford.edu>
// Created       : 09/25/2008
// Project       : SID Electronics API - GUI
//-----------------------------------------------------------------------------
// Description :
// Class to view KPIX samples.
//-----------------------------------------------------------------------------
// Copyright (c) 2006 by SLAC. All rights reserved.
// Proprietary and confidential to SLAC.
//-----------------------------------------------------------------------------
// Modification history :
// 07/02/2008: created
// 04/29/2009: Added new sample fields.
//-----------------------------------------------------------------------------
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <qlineedit.h>
#include <qtabwidget.h>
#include "KpixGuiSampleView.h"
using namespace std;


// Constructor
KpixGuiSampleView::KpixGuiSampleView ( ) : KpixGuiSampleViewForm() { 
   kpixRunRead   = NULL;
   kpixCalibRead = NULL;
   eventVar      = NULL;
   kpixIdxLookup = NULL;
   eventCount    = 0;
}


// Desconstructor Class
KpixGuiSampleView::~KpixGuiSampleView ( ) {
   unsigned int x;

   // Free Old Event Variables
   for (x=0; x<eventCount; x++) delete eventVar[x];
   if ( eventVar != NULL ) free(eventVar);
   eventCount = 0;

   // Delete calib read class
   if ( kpixCalibRead != NULL ) delete kpixCalibRead;
}


// Update calib Data
void KpixGuiSampleView::setRunData(KpixRunRead *kpixRunRead) { 
   unsigned int x,y;
   string       temp;

   // Delete calib read class
   if ( kpixCalibRead != NULL ) delete kpixCalibRead;

   this->kpixRunRead   = kpixRunRead;
   this->kpixCalibRead = new KpixCalibRead(kpixRunRead);

   // Free Old Event Variables
   for (x=0; x<eventCount; x++) delete eventVar[x];
   if ( eventVar != NULL ) free(eventVar);
   eventVar = NULL;
   eventCount = 0;

   // Free Lookup Table
   if ( kpixIdxLookup != NULL ) free (kpixIdxLookup);
   kpixIdxLookup = NULL;

   // Zero out sample selection
   selSample->setValue(0);

   // Create new list
   if ( kpixRunRead != NULL ) {

      // For Each Event Variable
      eventCount = kpixRunRead->getEventVarCount();
      eventVar = (KpixEventVar **) malloc(sizeof(KpixEventVar *)*eventCount);
      if ( eventVar == NULL ) throw(string("KpixGuiSampleView::setRunData -> Malloc Error"));
      eventVarTable->setNumRows(eventCount);
      for (x=0; x < eventCount; x++ ) {
         eventVar[x] = new KpixEventVar(*kpixRunRead->getEventVar(x));
         temp = eventVar[x]->name(); eventVarTable->setText(x,0,temp);
         temp = eventVar[x]->description(); eventVarTable->setText(x,2,temp);
      }

      // Determine max address
      y = 0;
      for (x=0; x < (unsigned int)kpixRunRead->getAsicCount(); x++) {
         if ( kpixRunRead->getAsic(x)->getAddress() > y ) 
            y = kpixRunRead->getAsic(x)->getAddress();
      }
      y++;

      // Creat table
      kpixIdxLookup = (unsigned int *)malloc(y*sizeof(unsigned int));     
      if ( kpixIdxLookup == NULL ) throw(string("KpixSampleView::setRunData -> Malloc Error"));
      for (x=0; x < (unsigned int)kpixRunRead->getAsicCount(); x++) 
         kpixIdxLookup[kpixRunRead->getAsic(x)->getAddress()] = x;
   }
   updateDisplay();
}


// Update Display
void KpixGuiSampleView::updateDisplay() {

   stringstream temp;
   unsigned int x,sample, kpix, gain, idx;;
   double       fitGain, fitIcept, charge;
   KpixSample   *kpixSample;

   if ( kpixRunRead != NULL ) {

      // Total Samples
      temp.str("");
      temp << kpixRunRead->getSampleCount();
      sampleCount->setText(temp.str());

      // Make sure there are samples to view
      if ( kpixRunRead->getSampleCount() > 0 ) {

         selSample->setEnabled(true);
         selSample->setMaxValue(kpixRunRead->getSampleCount()-1);

         // Get Sample Number & Kpix address
         sample     = selSample->value();
         kpixSample = kpixRunRead->getSample(sample);
         kpix       = kpixSample->getKpixAddress();
         idx        = kpixIdxLookup[kpix];

         // Train Number
         temp.str("");
         temp << dec << kpixSample->getTrainNum();
         trainNum->setText(temp.str());   

         // Kpix Address
         temp.str("");
         temp << dec << kpix;
         kpixAddress->setText(temp.str());

         // Kpix serial Number
         temp.str("");
         temp << dec << kpixRunRead->getAsic(idx)->getSerial();
         kpixSerial->setText(temp.str());

         // Kpix Channel
         temp.str("");
         temp << dec << kpixSample->getKpixChannel();
         kpixChannel->setText(temp.str());

         // Kpix Bucket
         temp.str("");
         temp << dec << kpixSample->getKpixBucket();
         kpixBucket->setText(temp.str());

         // Sample Range
         temp.str("");
         temp << dec << kpixSample->getSampleRange();
         sampleRange->setText(temp.str());

         // Sample Time
         temp.str("");
         temp << dec << kpixSample->getSampleTime();
         sampleTime->setText(temp.str());

         // Sample Value
         temp.str("");
         temp << dec << kpixSample->getSampleValue();
         sampleValue->setText(temp.str());

         // Sample empty
         temp.str("");
         temp << dec << kpixSample->getEmpty();
         sampleEmpty->setText(temp.str());

         // Sample Bad Count
         temp.str("");
         temp << dec << kpixSample->getBadCount();
         sampleBadCnt->setText(temp.str());

         // Sample Trigger Type
         temp.str("");
         temp << dec << kpixSample->getTrigType();
         sampleTrigType->setText(temp.str());

         // Figure out the gain
         gain = 0;
         if ( kpixRunRead->getAsic(0)->getCntrlForceLowGain(false) ) gain = 2;
         if ( kpixRunRead->getAsic(0)->getCntrlDoubleGain(false) ) gain = 1;
         if ( gain == 0 && kpixSample->getSampleRange() == 1 ) gain = 2;

         // Get Calibration Constants
         kpixCalibRead->getCalibData ( &fitGain, &fitIcept,"Force_Trig", gain, 
                                       kpixRunRead->getAsic(idx)->getSerial(),
                                       kpixSample->getKpixChannel(),
                                       kpixSample->getKpixBucket());

         // Sample Charge
         if ( fitGain == 0 ) charge = kpixSample->getSampleValue();
         else charge = (kpixSample->getSampleValue() - fitIcept) / fitGain;
         temp.str("");
         temp << charge;
         sampleCharge->setText(temp.str());

         // Fill In event Variables
         for (x=0; x < eventCount; x++ ) {
            temp.str("");
            temp  << kpixSample->getVarValue(x);
            eventVarTable->setText(x,1,temp.str());
         }
      }
      else {
         selSample->setEnabled(true);
         selSample->setMaxValue(0);
         trainNum->setText("");
         kpixAddress->setText("");
         kpixSerial->setText("");
         kpixChannel->setText("");
         kpixBucket->setText("");
         sampleRange->setText("");
         sampleTime->setText("");
         sampleValue->setText("");
         sampleCharge->setText("");
         for (x=0; x < eventCount; x++ ) eventVarTable->setText(x,1,"");
      }
   }
   update();
}

