/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOG_LOGWRITER_H
#define ORCA2_ORCALOG_LOGWRITER_H

#include <orcalog/masterfilewriter.h>
#include <orcalog/logwriterinfo.h>
#include <orcalog/masterfilewritehandler.h>

namespace orcalog
{

//!
//! @brief Base class for all interface slaves
//!        Sets up a data file and registers with the MasterFileWriter.
//!        The individual log writers (e.g. LaserLogWriter) inherit from this class.
//!
//! @author Tobias Kaupp, Alex Brooks
//!
class LogWriter
{
public:

    //! Params are:
    //!   logWriterInfo:    struct with required info for all logWriters.
    //!
    LogWriter( const LogWriterInfo &logWriterInfo );
    virtual ~LogWriter();

    const LogWriterInfo &logWriterInfo() const { return logWriterInfo_; }

    //! Creates data file (using createLogFile())
    void init();

protected:

    int numItemsLogged() const { return masterFileWriteHandler_.numItemsLogged(); }

    //! This version correctly handles two standard log file formats:
    //!     ice   : binary file
    //!     ascii : ascii file
    //! This function does not catch exceptions which may result when creating a file.
    //! Derived classes which want to modify this behavior or need to handle other
    //! formats need to re-implement this function.
    virtual void createLogFile( const std::string &filename, const std::string &format );

    //! Appends a record to the master file. Supplies the LogWriter ID and the data
    //! counter. Also prints out heartbeat message periodically.
    void writeReferenceToMasterFile()
        { masterFileWriteHandler_.writeReferenceToMasterFile(); }

    //! Handle to the output file.
    std::ofstream *file_;

private:

    MasterFileWriteHandler masterFileWriteHandler_;
    LogWriterInfo logWriterInfo_;
};


//! Allocates memory for file and opens
//! Raises FileException if something is wrong.
std::ofstream* openAsciiFileForWriting( const std::string &filename );

//! Allocates memory for file and opens
//! Raises FileException if something is wrong.
std::ofstream* openBinaryFileForWriting( const std::string &filename );

}

#endif
