//-----------------------------------------------------------------------------
// File          : KpixProgress.h
// Author        : Ryan Herbst  <rherbst@slac.stanford.edu>
// Created       : 09/26/2008
// Project       : SID Electronics API
//-----------------------------------------------------------------------------
// Description :
// Parent class to allow KpixApi classes to update progress in calling 
// functions.
//-----------------------------------------------------------------------------
// Copyright (c) 2009 by SLAC. All rights reserved.
// Proprietary and confidential to SLAC.
//-----------------------------------------------------------------------------
// Modification history :
// 09/26/2008: created
// 06/18/2009: Added namespace.
// 06/23/2009: Removed namespaces.
//-----------------------------------------------------------------------------
#ifndef __KPIX_PROGRESS_H__
#define __KPIX_PROGRESS_H__

// Constants
static const unsigned int KpixDataTH1F     = 0;
static const unsigned int KpixDataTGraph   = 1;
static const unsigned int KpixDataTGraph2D = 2;
static const unsigned int KpixDataTH2F     = 3;
static const unsigned int KpixDataString   = 4;
static const unsigned int KpixDataInt      = 5;
static const unsigned int KpixDataUInt     = 6;
static const unsigned int KpixDataDouble   = 7;

class KpixProgress {
   public:
      virtual void updateProgress(unsigned int count, unsigned int total) = 0;
      virtual void updateData(unsigned int type, unsigned int count, void **data) = 0;
      virtual ~KpixProgress() {};
};
#endif
