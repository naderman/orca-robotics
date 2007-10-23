/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <fstream>
#include <assert.h>
#include <orcaice/orcaice.h>
#include "logwriter.h"
#include "exceptions.h"

using namespace std;
using namespace orcalog;

LogWriter::LogWriter( const LogWriterInfo &logWriterInfo )
    : file_(0),
      masterFileWriteHandler_(logWriterInfo),
      logWriterInfo_(logWriterInfo)
{
    //
    // create log file. Derived classes may re-implement this function to modify
    // the standard treatment of log formats.
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
LogWriter::createAsciiFile( const std::string &filename )
{
    if ( file_ ) {
        std::string s = "Log file already exists.";
        throw FileException( ERROR_INFO, s );
    }

    file_ = new ofstream( filename.c_str() );
    if ( !file_->is_open() )  {
        std::string s = "Could not create data file " + filename;
        throw FileException( ERROR_INFO, s );
    }
}

void
LogWriter::createBinaryFile( const std::string &filename )
{
    if ( file_ ) {
        std::string s = "Log file already exists.";
        throw FileException( ERROR_INFO, s );
    }

    file_ = new ofstream( filename.c_str(), ios::binary );
    if ( !file_->is_open() )  {
        std::string s = "Could not create data file " + filename;
        throw FileException( ERROR_INFO, s );
    }
}

void
LogWriter::createLogFile( const std::string &filename, const std::string &format )
{
    //
    // create log file, may throw and it will kill us
    //
    if ( format == "ice" ) {
        createBinaryFile(filename);
    }
    else if ( format == "ascii" ) {
        createAsciiFile(filename);
    }
    else {
        assert( false && "base class can only handle 'ice' and 'ascii' formats" );
    }
}
