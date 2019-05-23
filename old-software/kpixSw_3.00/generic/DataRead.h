//-----------------------------------------------------------------------------
// File          : DataRead.h
// Author        : Ryan Herbst  <rherbst@slac.stanford.edu>
// Created       : 04/12/2011
// Project       : General Purpose
//-----------------------------------------------------------------------------
// Description :
// Read data & configuration from disk
//-----------------------------------------------------------------------------
// Copyright (c) 2011 by SLAC. All rights reserved.
// Proprietary and confidential to SLAC.
//-----------------------------------------------------------------------------
// Modification history :
// 04/12/2011: created
//-----------------------------------------------------------------------------
#ifndef __DATA_READ_H__
#define __DATA_READ_H__

#include <string>
#include <map>
#include <Data.h>
#include <sys/types.h>
#include <XmlVariables.h>
#include <DataSharedMem.h>
using namespace std;

#ifdef __CINT__
#define uint unsigned int
#endif

// Define variable holder
typedef map<string,string> VariableHolder;

//! Class to contain generic register data.
class DataRead {

      // Shared memory
      uint smemFd_;
      void *smem_;
      uint rdAddr_;
      uint rdCount_;

      // File descriptor
      int fd_;

      // File size
      off_t size_;

      // Process xml
      void xmlParse ( uint size, char *data );

      // Variables
      XmlVariables status_;
      XmlVariables config_;
      XmlVariables start_;
      XmlVariables stop_;
      XmlVariables time_;

      // Start/Stop flags
      bool sawRunStart_;
      bool sawRunStop_;
      bool sawRunTime_;

   public:

      //! Constructor
      DataRead ( );

      //! Deconstructor
      ~DataRead ( );

      //! Open File
      /*! 
       * \param file Filename
      */
      bool open ( string file );

      //! Open Shared Memory
      /*! 
       * \param system System name
       * \param id ID to identify your process
      */
      void openShared ( string system, uint id );

      //! Close File
      void close ( );

      //! Return file size in bytes
      off_t size ( );

      //! Return file position in bytes
      off_t pos ( );

      //! Get next data record
      /*! 
       * Returns true on success
       * \param data Data object to store data
      */
      bool next ( Data *data );

      //! Get next data record & create new data object
      /*! 
       * Returns NULL on failure
      */
      Data *next ( );

      //! Get a config value
      /*! 
       * \param var Config variable name
      */
      string getConfig ( string var );

      //! Get a config value as integer
      /*! 
       * \param var Config variable name
      */
      uint getConfigInt ( string var );

      //! Get a status value
      /*! 
       * \param var Status variable name
      */
      string getStatus ( string var );

      //! Get a status value as integer
      /*! 
       * \param var Status variable name
      */
      uint getStatusInt ( string var );

      //! Dump config
      void dumpConfig ( );

      //! Dump status
      void dumpStatus ( );

      //! Get config as XML
      string getConfigXml ( );

      //! Dump status
      string getStatusXml ( );

      //! Get a run start value
      /*! 
       * \param var Variable name
      */
      string getRunStart ( string var );

      //! Get a run stop value
      /*! 
       * \param var Variable name
      */
      string getRunStop ( string var );

      //! Get a run time value
      /*! 
       * \param var Variable name
      */
      string getRunTime ( string var );

      //! Dump start
      void dumpRunStart ( );

      //! Dump stop
      void dumpRunStop ( );

      //! Dump time
      void dumpRunTime ( );

      //! Return true if we saw start marker, self clearing
      bool  sawRunStart ( );

      //! Return true if we saw stop marker, self clearing
      bool  sawRunStop ( );

      //! Return true if we saw time marker, self clearing
      bool  sawRunTime ( );
};
#endif
