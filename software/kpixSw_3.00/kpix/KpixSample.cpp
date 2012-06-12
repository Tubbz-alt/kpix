//-----------------------------------------------------------------------------
// File          : KpixSample.cpp
// Author        : Ryan Herbst  <rherbst@slac.stanford.edu>
// Created       : 05/29/2012
// Project       : Kpix DAQ
//-----------------------------------------------------------------------------
// Description :
// Sample Container
//-----------------------------------------------------------------------------
// Copyright (c) 2012 by SLAC. All rights reserved.
// Proprietary and confidential to SLAC.
//-----------------------------------------------------------------------------
// Modification history :
// 05/29/2012: created
//-----------------------------------------------------------------------------
#include <string.h>
#include <iostream>
#include "KpixSample.h"
using namespace std;

// Constructor for static pointer
KpixSample::KpixSample () {
   data_        = ldata_;
   eventNumber_ = 0;
}

// Constructor with copy
KpixSample::KpixSample ( uint *data, uint eventNumber ) {
   data_        = ldata_;
   eventNumber_ = eventNumber;
   memcpy(ldata_,data,8);
}

//! DeConstructor
KpixSample::~KpixSample ( ) { }

// Set data pointer.
void KpixSample::setData ( uint *data, uint eventNumber ) {
   data_        = data;
   eventNumber_ = eventNumber;
}

// Get sample event number.
uint KpixSample::getEventNum() {
   return(eventNumber_);
}

// Get KPIX address from sample.
uint KpixSample::getKpixAddress() {
   uint ret;
   ret = (data_[0] >> 16) & 0xFFF;
   return(ret);
}

// Get KPIX channel.
uint KpixSample::getKpixChannel() {
   uint ret;
   ret = data_[0] & 0x3FF;
   return(ret);
}

// Get KPIX bucket.
uint KpixSample::getKpixBucket() {
   uint ret;
   ret = (data_[0] >> 10) & 0x3;
   return(ret);
}

// Get sample range.
uint KpixSample::getSampleRange() {
   uint ret;
   ret = (data_[0] >> 13) & 0x1;
   return(ret);
}

// Get sample time
uint KpixSample::getSampleTime() {
   uint ret;
   ret = (data_[1] >> 16) & 0x1FFF;
   return(ret);
}

// Get sample value.
uint KpixSample::getSampleValue() {
   uint ret;
   ret = data_[1] & 0x1FFF;
   return(ret);
}

// Get empty flag.
uint KpixSample::getEmpty() {
   uint ret;
   ret = (data_[0] >> 15) & 0x1;
   return(ret);
}

// Get badCount flag.
uint KpixSample::getBadCount() {
   uint ret;
   ret = (data_[0] >> 14) & 0x1;
   return(ret);
}

// Get trigger type flag.
uint KpixSample::getTrigType() {
   uint ret;
   ret = (data_[0] >> 12) & 0x1;
   return(ret);
}

// Get sample type
KpixSample::SampleType KpixSample::getSampleType() {
   SampleType ret;
   ret = (SampleType)((data_[0] >> 28) & 0xf);
   return(ret);
}

