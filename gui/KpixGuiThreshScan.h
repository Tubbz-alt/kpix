//-----------------------------------------------------------------------------
// File          : KpixGuiThreshScan.h
// Author        : Ryan Herbst  <rherbst@slac.stanford.edu>
// Created       : 09/25/2008
// Project       : SID Electronics API - GUI
//-----------------------------------------------------------------------------
// Description :
// Class for running KPIX ASIC threshold scans.
// This is a class which builds off of the class created in
// KpixGuiThreshScanForm.ui
//-----------------------------------------------------------------------------
// Copyright (c) 2006 by SLAC. All rights reserved.
// Proprietary and confidential to SLAC.
//-----------------------------------------------------------------------------
// Modification history :
// 07/02/2008: created
//-----------------------------------------------------------------------------
#ifndef __KPIX_GUI_THRESH_SCAN_H__
#define __KPIX_GUI_THRESH_SCAN_H__

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <qwidget.h>
#include "KpixGuiThreshScanForm.h"
#include "KpixGuiThreshView.h"
#include "KpixGuiError.h"
#include <KpixAsic.h>
#include <KpixFpga.h>
#include <KpixProgress.h>
#include <KpixCalDist.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlcdnumber.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qtable.h>
#include <qthread.h>
#include <qspinbox.h>
#include <TQtWidget.h>
#include <TCanvas.h>
#include <TH2F.h>

class KpixGuiTop;

class KpixGuiThreshScan : public KpixGuiThreshScanForm, public QThread, public KpixProgress {

      // ASIC & FPGA Containers
      unsigned int      asicCnt;
      KpixAsic          **asic;
      KpixFpga          *fpga;
      KpixGuiTop        *parent;
      KpixGuiError      *errorMsg;
      bool              running;
      bool              enRun;
      bool              isRunning;
      string            baseDir, desc, outDataDir, outDataFile;
      unsigned int      prgCurrent;
      unsigned int      prgTotal;
      KpixRunVar        **runVars;
      unsigned int      runVarCount;
      TH2F              *plot;
      KpixGuiThreshView *threshView;

   public:

      // Creation Class
      KpixGuiThreshScan (KpixGuiTop *parent );

      // Delete
      ~KpixGuiThreshScan ( );

      // Control Enable Of Buttons/Edits
      void setEnabled ( bool enable );

      // Set Configurations
      void setAsics ( KpixAsic **asic, unsigned int asicCnt, KpixFpga *fpga );

      // Update progress
      void updateProgress(unsigned int count, unsigned int total);
      void updateData(unsigned int id, unsigned int count, void **data);

      // Window was closed
      void closeEvent(QCloseEvent *e);

      // Close is called
      bool close();

   protected:

      void run();

   private slots:

      void runTest_pressed();
      void stopTest_pressed();
      void viewPlots_pressed();
      void customEvent ( QCustomEvent *event );

   public slots:


};

#endif
