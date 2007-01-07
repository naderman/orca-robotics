/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOG_LOGGER_H
#define ORCA2_ORCALOG_LOGGER_H

#include <orcaice/context.h>


namespace orcalog
{

class LogMaster;

//!
//! @brief Base class for all interface slaves
//!        Sets up a binary data file and registers with the master.
//!        The individual loggers (e.g. LaserLogger) inherit from this class.
//!
//! @author Tobias Kaupp
//!
class Logger
{
public:

    Logger( LogMaster *master, 
            const std::string &interfaceType, 
            const std::string &interfaceTag,
            const std::string &format,
            const std::string &filename,
            const orcaice::Context & context );

    virtual ~Logger();

    //! This function are called from the component level and must be implemented
    //! in each derived class.
    virtual void init()=0;

protected:
    //! Master file handler
    LogMaster *master_;

    //! Summary of component and communicator information
    orcaice::Context context_;
    
    //! In Ice terms, this is the interface ID.
    //! By convention, we strip the namespace.
    //! E.g. ::orca::LaserScanner2d interface translates to LaserScanner2d type.
    std::string interfaceType_;

    //! This is the tag used in the config files
    //! By convention, it is formed by adding a sequential number
    //! to the interface type.
    //! E.g. Laser0, Laser1, ..., Laser12, etc. 
    std::string interfaceTag_;

    //! How the data is stored to files
    //! Standard types are "ice" (binary) and "ascii".
    //! Derived slaves are free to invent their own encodings.
    std::string format_;

    //! This is a unique ID which Master assignes to each logger.
    //! By convention it is a sequential number.
    //! It is used to refer to the log files. E.g.
    //!    id_=0 Laser0
    //!    id_=1 Position2d0
    int id_;

    //! Log filename.
    std::string filename_;

    //! Handle to the output file.
    std::ofstream *file_;

    //! Counts how many data objects this logger has logged so far.
    int dataCounter_;

    //! Optional utility. The handle to file is in file_.
    //! You can create the file yourself if you want.
    //! Raises FileException if something is wrong.
    void createAsciiFile();

    //! Optional utility. The handle to file is in file_.
    //! You can create the file yourself if you want.
    //! Raises FileException if something is wrong.
    void createBinaryFile();

    //! This version correctly handles two standard log file formats:
    //!     ice   ; binary file
    //!     ascii : ascii file
    //! This function does not catch exceptions which may result when creating a file.
    //! Derived classes which want to modify this behavior or need to handle other
    //! formats need to re-implement this function.
    virtual void createLogFile();

    //! Calls Master::append(). Supplies the Logger ID and the updated object
    //! counter. Also prints out heartbeat message periodically.
    void appendMasterFile( int seconds, int useconds );

private:

};

}

#endif
