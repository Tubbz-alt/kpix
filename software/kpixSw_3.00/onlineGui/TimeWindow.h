//-----------------------------------------------------------------------------
// File          : TimeWindow.h
// Author        : Ryan Herbst  <rherbst@slac.stanford.edu>
// Created       : 10/04/2011
// Project       : General purpose
//-----------------------------------------------------------------------------
// Description :
// Timestamp window
//-----------------------------------------------------------------------------
// Copyright (c) 2011 by SLAC. All rights reserved.
// Proprietary and confidential to SLAC.
//-----------------------------------------------------------------------------
// Modification history :
// 10/04/2011: created
//-----------------------------------------------------------------------------
#ifndef __TIME_WINDOW_H__
#define __TIME_WINDOW_H__

#include <QWidget>
#include <QMap>
#include <QDomDocument>
#include <QLineEdit>
#include <QSpinBox>
#include <QTimer>
#include <qwt_plot.h>
#include <qwt_plot_item.h>
#include <qwt_plot_histogram.h>
#include <KpixEvent.h>
#include "KpixHistogram.h"
using namespace std;

class TimeWindow : public QWidget {
   Q_OBJECT

      QwtPlotHistogram *time_[4];
      QwtPlot          *plot_[4];

      void setHistData(uint x, KpixHistogram *time);

      KpixHistogram data_[32][1024][4];

   public:

      // Window
      TimeWindow ( QWidget *parent = NULL );

      // Delete
      ~TimeWindow ( );

      void rxData (KpixEvent *event);
      void rePlot(uint kpix, uint chan);
      void resetPlot();

   public slots:

      void showItem( QwtPlotItem *item, bool on );

};

#endif
