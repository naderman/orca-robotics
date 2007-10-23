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
//! @author Tobias Kaupp
//!
class LogWriter
{
public:

    //! Params are:
    //!   logWriterInfo:    struct with required info for all logWriters.
    LogWriter( const LogWriterInfo &logWriterInfo );
    ~LogWriter();

    const LogWriterInfo &logWriterInfo() const { return logWriterInfo_; }

protected:

    //! Handle to the output file.
    std::ofstream *file_;

    //! Appends a record to the master file. Supplies the LogWriter ID and the data
    //! counter. Also prints out heartbeat message periodically.
    void writeReferenceToMasterFile()
        { masterFileWriteHandler_.writeReferenceToMasterFile(); }

    int numItemsLogged() const { return masterFileWriteHandler_.numItemsLogged(); }

private:

    MasterFileWriteHandler masterFileWriteHandler_;
    LogWriterInfo logWriterInfo_;

    //! Optional utility. The handle to file is in file_.
    //! You can create the file yourself if you want.
    //! Raises FileException if something is wrong.
    void createAsciiFile( const std::string &filename );

    //! Optional utility. The handle to file is in file_.
    //! You can create the file yourself if you want.
    //! Raises FileException if something is wrong.
    void createBinaryFile( const std::string &filename );

    //! This version correctly handles two standard log file formats:
    //!     ice   : binary file
    //!     ascii : ascii file
    //! This function does not catch exceptions which may result when creating a file.
    //! Derived classes which want to modify this behavior or need to handle other
    //! formats need to re-implement this function.
    virtual void createLogFile( const std::string &filename, const std::string &format );
};

}

#endif
