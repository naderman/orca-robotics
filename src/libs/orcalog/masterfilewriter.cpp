/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <IceUtil/Time.h>
#include <orcaice/orcaice.h>
#include <orcalog/exceptions.h>
#include "masterfilewriter.h"
#include "utils.h"

using namespace std;
using namespace orcalog;


MasterFileWriter::MasterFileWriter( const std::string      &filename,
                                    const orcaice::Context &context )
    : numLoggers_(0),
      numItemsLogged_(0),
      isStarted_(false),
      context_(context)
{
    // create master file
    file_ = new ofstream( filename.c_str() );
    if ( !file_->is_open() ) {
        context_.tracer().warning( "Could not create master file " + filename );
        throw orcalog::FileException( ERROR_INFO, "Could not create master file " + filename );
    }
    
    // write some global information as comments to the header
    (*file_) << "# == Component ==" << endl;
    (*file_) << "# Logger host    : " << hydroutil::getHostname() << endl;
    (*file_) << "# Log start time : " << IceUtil::Time::now().toDateTime() << endl;
    (*file_) << "# Ice version    : " << ICE_STRING_VERSION << endl;
    (*file_) << "# Orca version   : " << hydroutil::orcaVersion() << endl;
    (*file_) << "# " << endl;
    (*file_) << "# == Interfaces / Logs Legend ==" << endl;
    (*file_) << "# [interface name]" << endl;
    (*file_) << "# [log filename] [interface type] [log format] [replay enabled]" << endl;
    (*file_) << "# " << endl;
    (*file_) << "# == Interfaces / Logs ==" << endl;
}

MasterFileWriter::~MasterFileWriter()
{
    std::stringstream ss;
    ss << "shutting down after writing "<<numItemsLogged_<<" objects to "<<numLoggers_<<" logs.";
    context_.tracer().info( ss.str() );

    if ( file_ ) {
        file_->close();
        delete file_;
    }
}

int 
MasterFileWriter::loggerCount() const
{
    IceUtil::Mutex::Lock lock(mutex_);
    return numLoggers_;
}

int
MasterFileWriter::addLog( const std::string &filename, 
                          const std::string &interfaceType,
                          const std::string &format,
                          const std::string &comment )
{
    context_.tracer().debug( 
        "adding log: file="+filename+" id="+interfaceType+" fmt="+format+" comment="+comment, 5 );

    IceUtil::Mutex::Lock lock(mutex_);

    if ( isStarted_ ) {
        context_.tracer().warning("Can't register after started appending!");
        throw orcalog::Exception( ERROR_INFO, "Can't register after started appending!" );
    }

    // this is just a comment 
    (*file_) << "# " << comment << endl;

    // this will be used replay this log back, we enable playback by default
    bool enabled = true;
    (*file_) << orcalog::headerLine( filename, interfaceType, format, enabled ) << endl;

    // calc unique logger id
    int id = numLoggers_;
    numLoggers_++;
    return id;
}

void 
MasterFileWriter::notifyOfLogfileAddition( int loggerId, int indexInSubLogfile )
{
    IceUtil::Mutex::Lock lock(mutex_);

    if ( !isStarted_ ) {
        // Write the beginning of data field (end of the header)
        (*file_) << "# " << endl;
        (*file_) << "# == Data Legend ==" << endl;
        (*file_) << "# seconds useconds file_id object_id" << endl;
        (*file_) << "# " << endl;
        (*file_) << orcalog::endOfHeader() << endl;
        isStarted_ = true;
    }

    //
    // write to master file
    //
    orca::Time now = orcaice::getNow();
    (*file_) << orcalog::dataLine( now.seconds, now.useconds, loggerId, indexInSubLogfile ) << endl;

    // count data objects
    ++numItemsLogged_;
}
