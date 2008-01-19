/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <fstream>
#include <assert.h>
#include <orcaice/orcaice.h>
#include "logwriter.h"
#include "exceptions.h"

using namespace std;

namespace orcalog {

LogWriter::LogWriter( const LogWriterInfo &logWriterInfo )
    : file_(0),
      masterFileWriteHandler_(logWriterInfo),
      logWriterInfo_(logWriterInfo)
{
}

void
LogWriter::init()
{
    //
    // create log file. Derived classes may re-implement this function to modify
    // the standard treatment of log formats.
    //
    // Note: openLogFile() has to be called from here, because
    // calling it from the constructor would call the base-class's version.
    //
    createLogFile( logWriterInfo_.filename, 
                   logWriterInfo_.format );
}

LogWriter::~LogWriter()
{
    if ( file_ ) {
        // debug
//         cout<<"LogWriter::~LogWriter(): closing file"<<endl;
        file_->close();
        delete file_;
    }
}

void
LogWriter::createLogFile( const std::string &filename, const std::string &format )
{
    // Check that the file's not already open
    if ( file_ ) {
        std::string s = "Log file already exists.";
        throw FileException( ERROR_INFO, s );
    }

    //
    // create log file, may throw and it will kill us
    //
    if ( format == "ice" ) {
        file_ = openBinaryFileForWriting(filename);
    }
    else if ( format == "ascii" ) {
        file_ = openAsciiFileForWriting(filename);
    }
    else {
        assert( false && "base class can only handle 'ice' and 'ascii' formats" );
    }
}

//////////////////////////////////////////////////////////////////////

std::ofstream *
openAsciiFileForWriting( const std::string &filename )
{
    std::ofstream *file = new ofstream( filename.c_str() );
    if ( !file->is_open() )  {
        std::string s = "Could not create data file " + filename;
        throw FileException( ERROR_INFO, s );
    }
    return file;
}

std::ofstream *
openBinaryFileForWriting( const std::string &filename )
{
    std::ofstream *file = new ofstream( filename.c_str(), ios::binary );
    if ( !file->is_open() )  {
        std::string s = "Could not create data file " + filename;
        throw FileException( ERROR_INFO, s );
    }
    return file;
}

}
