/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOG_REPLAYER_H
#define ORCA2_ORCALOG_REPLAYER_H

#include <string>
#include <vector>
#include <orcaice/context.h>

namespace orcalog
{

//! All replayers inherit from this. Some file handling is done in the base class.
class Replayer
{
public:
    Replayer( const std::string      &interfaceType, 
              const std::string      &format,
              const std::string      &filename,
              const orcaice::Context &context );
    virtual ~Replayer();

    //! Opens data file and calls virtual functions initInterfaces() and
    //! initConfigs(). The first one must be implemented in derived classes the
    //! second one is optional.
    void init();

    //! Reads the specified data object from the log and sends it out.
    virtual void replayData( int index, bool isTest=false ) = 0;

//     void 

    //! Returns interface type.
    std::string interfaceType() const { return interfaceType_; };

    //! Returns interface name.
    std::string interfaceName() const { return interfaceName_; };

protected:
    //! Summary of component and communicator information
    orcaice::Context context_;

    //! In Ice terms, this is the interface ID.
    //! By convention, we strip the namespace.
    //! E.g. ::orca::LaserScanner2d interface translates to LaserScanner2d type.
    std::string interfaceType_;

    //! This is the name under which the replayer should register the interface
    //! which it will replay.
    std::string interfaceName_;

    //! How the data is stored to files
    //! Standard types are "ice" (binary) and "ascii".
    //! Derived slaves are free to invent their own encodings.
    std::string format_;

    //! Log filename.
    std::string   filename_;

    //! Handle to the input file.
    std::ifstream *file_;

    //! Counts how many data objects we have loaded from the log file.
    int dataCounter_;

    virtual void initInterfaces() = 0;

    //! not all interfaces have config information
    virtual void initConfigs() {};

    //! Optional utility. The handle to file is in file_.
    //! You can open the file yourself if you want.
    //! Raises FileException if something is wrong.
    void openAsciiFile();

    //! Optional utility. The handle to file is in file_.
    //! You can open the file yourself if you want.
    //! Raises FileException if something is wrong.
    void openBinaryFile();

    //! This version correctly handles two standard log file formats:
    //!     ice   ; binary file
    //!     ascii : ascii file
    //! This function does not catch exceptions which may result when opening a file.
    //! Derived classes which want to modify this behavior or need to handle other
    //! formats need to re-implement this function.
    virtual void openLogFile();

    //! Throws orcalog::FileException when something is wrong with the log file.
    void checkFile();

    //! Throws orcalog::Exception when attempting to read the log backwards.
    void checkIndex( int index );
};

} // namespace

#endif
