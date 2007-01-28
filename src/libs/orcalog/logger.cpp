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
#include <orcaice/orcaice.h>
#include "logger.h"
#include "logmaster.h"
#include "exceptions.h"

using namespace std;
using namespace orcalog;

Logger::Logger( LogMaster *master, 
            const std::string &interfaceType, 
            const std::string &interfaceTag,
            const std::string &format,
            const std::string &filename,
            const orcaice::Context & context )
    : master_(master),
      context_(context),
      interfaceType_(interfaceType),
      interfaceTag_(interfaceTag),
      format_(format),
      id_(-1),
      filename_(filename),
      file_(0),
      dataCounter_(0)
{
    // we supply our interface name to the master so it's stored as a comment in 
    // the header of the master file.
    // This may throw ConfigFileException, not catching it here, it will kill us.
    std::string proxyString = orcaice::getRequiredInterfaceAsString( context_, interfaceTag_ );

    // record our slave ID
    id_ = master_->addLog( this, filename_, interfaceType_, format_, proxyString );

    //
    // create log file. Derived classes may re-implement this function to modify
    // the standard treatment of log formats.
    //
    createLogFile();
}

Logger::~Logger()
{
    if ( file_ ) {
        // debug
//         cout<<"Logger::~Logger(): closing file"<<endl;
        file_->close();
        delete file_;
    }
}

void
Logger::createAsciiFile()
{
    if ( file_ ) {
        std::string s = "Log file already exists.";
        context_.tracer()->warning( s );
        throw FileException( ERROR_INFO, s );
    }

    file_ = new ofstream( filename_.c_str() );
    if ( !file_->is_open() )  {
        std::string s = "Could not create data file " + filename_;
        context_.tracer()->warning( s );
        throw FileException( ERROR_INFO, s );
    }
}

void
Logger::createBinaryFile()
{
    if ( file_ ) {
        std::string s = "Log file already exists.";
        context_.tracer()->warning( s );
        throw FileException( ERROR_INFO, s );
    }

    file_ = new ofstream( filename_.c_str(), ios::binary );
    if ( !file_->is_open() )  {
        std::string s = "Could not create data file " + filename_;
        context_.tracer()->warning( s );
        throw FileException( ERROR_INFO, s );
    }
}

void
Logger::createLogFile()
{
    //
    // create log file, may throw and it will kill us
    //
    if ( format_ == "ice" ) {
        createBinaryFile();
    }
    else if ( format_ == "ascii" ) {
        createAsciiFile();
    }
}

void
Logger::appendMasterFile()
{
    context_.tracer()->debug(interfaceTag_+"Logger: received object. Writing to file.", 10);

    if ( id_ < 0 ) {
        context_.tracer()->warning( interfaceTag_+"Logger: received object when not registered. Ignoring.");
        return;
    }

    //
    // append master file, increment data counter
    //
    master_->addData( id_, dataCounter_ );
    ++dataCounter_;

    // Let the user know that something's going on.
    if ( dataCounter_ % 50 == 0 ) {
        ostringstream ss;
        ss << interfaceTag_ << "Logger: logged " << dataCounter_ << " data objects.";
        context_.tracer()->debug( ss.str(),1 );
    }
}
