//-----------------------------------------------------------------------------
// File          : KpixGuiTrig.h
// Author        : Ryan Herbst  <rherbst@slac.stanford.edu>
// Created       : 09/25/2008
// Project       : SID Electronics API - GUI
//-----------------------------------------------------------------------------
// Description :
// Class for graphical representation of KPIX Trigger Settings.
// This is a class which builds off of the class created in
// KpixGuiTrigForm.ui
//-----------------------------------------------------------------------------
// Copyright (c) 2006 by SLAC. All rights reserved.
// Proprietary and confidential to SLAC.
//-----------------------------------------------------------------------------
// Modification history :
// 07/02/2008: created
// 04/29/2009: Seperate methods for display update and data read.
//-----------------------------------------------------------------------------
#ifndef __KPIX_GUI_TRIG_H__
#define __KPIX_GUI_TRIG_H__

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <qwidget.h>
#include "KpixGuiTrigForm.h"
#include <KpixAsic.h>
#include <KpixFpga.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlcdnumber.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qtable.h>
#include <qspinbox.h>


class KpixGuiTrig : public KpixGuiTrigForm {

      // ASIC & FPGA Containers
      unsigned int asicCnt;
      KpixAsic     **asic;
      KpixFpga     *fpga;

      // Threshold Table Entries
      QComboBox **thold;

      // Channel Table Entries
      QComboBox **mode;

   public:

      // Creation Class
      KpixGuiTrig ( QWidget *parent = 0 );

      // Set Asics
      void setAsics (KpixAsic **asic, unsigned int asicCnt, KpixFpga *fpga);

      // Deconstructor
      ~KpixGuiTrig();

      // Control Enable Of Buttons/Edits
      void setEnabled ( bool enable );

   private slots:

      void setAllPressed();

   public slots:

      void updateDisplay();
      void readConfig();
      void writeConfig();
};

#endif
