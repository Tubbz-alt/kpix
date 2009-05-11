//-----------------------------------------------------------------------------
// File          : KpixHistogram.h
// Author        : Ryan Herbst  <rherbst@slac.stanford.edu>
// Created       : 10/15/2008
// Project       : SID Electronics API
//-----------------------------------------------------------------------------
// Description :
// Class to track histograms generated in KPIX runs.
//-----------------------------------------------------------------------------
// Copyright (c) 2008 by SLAC. All rights reserved.
// Proprietary and confidential to SLAC.
//-----------------------------------------------------------------------------
// Modification history :
// 10/15/2008: created
// 03/03/2009: changed container values to unsigned int
//-----------------------------------------------------------------------------
#ifndef __KPIX_HISTOGRAM_H__
#define __KPIX_HISTOGRAM_H__

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
using namespace std;

// KPIX Event Data Class
class KpixHistogram {

      // Histogram contents
      unsigned int *data;

      // Number of entries in histogram
      unsigned int entries;

      // Min Value
      unsigned int min;
      unsigned int max;

   public:

      // Constructor
      KpixHistogram ( );

      // Constructor
      ~KpixHistogram ( );

      // Add an entry
      void fill(unsigned int value);

      // Get Number Of Entries
      unsigned int binCount();

      // Get Min Value
      unsigned int minValue();

      // Get Max Value
      unsigned int maxValue();

      // Get Bin Value
      unsigned int value(unsigned int bin);

      // Get Bin Count
      unsigned int count(unsigned int bin);
};

#endif
