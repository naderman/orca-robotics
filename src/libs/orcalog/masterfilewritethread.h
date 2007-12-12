#ifndef ORCALOG_MASTERFILEWRITEHANDLER_H
#define ORCALOG_MASTERFILEWRITEHANDLER_H

#include <orcalog/masterfilewriter.h>
#include <orcalog/logwriterinfo.h>

namespace orcalog {

//!
//! Handles interaction with the MasterFileWriter.  Tells the MasterFileWriter
//! everything it needs to write references to an object that has just been written in a sub-file.
//!
//! @author Alex Brooks
//!
class MasterFileWriteThread
{

public: 

    MasterFileWriteThread( const LogWriterInfo &logWriterInfo );

    //! Appends a record to the master file. Supplies the LogWriter ID and the data
    //! counter. Also prints out heartbeat message periodically.
    void writeReferenceToMasterFile();

    int numItemsLogged() const { return numItemsLogged_; }

private: 

    LogWriterInfo logWriterInfo_;

    //! This is a unique ID which Master assignes to each logger.
    //! By convention it is a sequential number.
    //! It is used to refer to the log files. E.g.
    //!    id_=0 Laser0
    //!    id_=1 Odometry2d0
    int id_;

    //! Counts how many data objects this logger has logged so far.
    int numItemsLogged_;
    
};

}

#endif
