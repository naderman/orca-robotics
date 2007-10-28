/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <orcaice/orcaice.h>
#include <orcalog/exceptions.h>

#include "masterfilereader.h"
#include "utils.h"

using namespace std;
using namespace orcalog;


MasterFileReader::MasterFileReader( const std::string &filename, const orcaice::Context& context )
    : index_(-1),
      context_(context)
{
    // create master file
    file_ = new ifstream( filename.c_str() );
    if ( !file_->is_open() ) {
        throw orcalog::FileException( ERROR_INFO, "Could not open master file " + filename );
    }

    // remember the dir where the master file is located
    // the individual log files MUST be in the same dir.
    dir_ = hydroutil::dirname( filename );

    calcConstituentLogs();
}

MasterFileReader::~MasterFileReader()
{
    if ( file_ ) {
        file_->close();
        delete file_;
    }
}

void
MasterFileReader::calcConstituentLogs()
{
    std::string line;
    std::string filename;
    std::string interfaceType;
    std::string format;
    bool enabled;

    while ( std::getline( *file_, line ) ) {
        // data starts here, don't parse headers anymore
        if ( orcalog::isEndOfHeader( line ) ) {
            break;
        }
    
        // skip comments (check for it AFTER the end-of-header in case it starts with #)
        if ( line[0] == '#' ) {
            continue;
        }
        
        orcalog::parseHeaderLine( line, filename, interfaceType, format, enabled );

        // add full path to the file name
        filename = dir_ + hydroutil::pathDelimeter() + filename;

        filenames_.push_back( filename );
        interfaceTypes_.push_back( interfaceType );
        formats_.push_back( format );
        enableds_.push_back( enabled );

        context_.tracer()->debug( "MasterFileReader: Parsed header: file="+filename+" type="+interfaceType+" fmt="+format, 5);
    }

    // At this point we're at the start of the data.
    index_=0;
}

void
MasterFileReader::getLogs( std::vector<std::string>& filenames,
                           std::vector<std::string>& interfaceTypes,
                           std::vector<std::string>& formats,
                           std::vector<bool>& enableds )
{
    filenames      = filenames_;
    interfaceTypes = interfaceTypes_;
    formats        = formats_;
    enableds       = enableds_;
}

int 
MasterFileReader::rewindToStart()
{   
    // STL book p.634-36
    file_->clear();
    file_->seekg( 0 );

//     int count = 0;
    std::string line;
    while ( std::getline( *file_, line ) ) {
//         cout<<count++<<endl;
        // data starts here, don't parse headers anymore
        if ( orcalog::isEndOfHeader( line ) ) {
            return 0;
        }
    }
    // this is EOF, failed to find DATA
    return 1;
}

int 
MasterFileReader::getData( int& seconds, int& useconds, int& id, int& index )
{
    std::string line;

    while ( !file_->eof() ) 
    {
        // Remember the position of the start of this line.
        std::ios::pos_type lineStartPos = file_->tellg();

        std::getline( *file_, line );

        // skip empty lines and comments
        if ( line.empty() || line[0] == '#' ) {
            continue;
        }
        
        // this may throw when parsing fails, we just print a warning and go to the next line
        try
        {
            orcalog::parseDataLine( line, seconds, useconds, id, index );
        }
        catch ( const orcalog::Exception& e )
        {
            context_.tracer()->warning( "parsing error in master file line: '" + line + "'" );
            continue;
        }

        // success
        breadCrumbs_.placeCrumb( lineStartPos, seconds, useconds );
        return 0;
    }
    
//     cout<<"MasterFileReader::getData: this is EOF"<<endl;
    return 1;
}

int 
MasterFileReader::getDataAtOrAfterTime( int& seconds, int& useconds, int& id, int& index, int seekSec, int seekUsec )
{
    // Maybe the requested time is in the past, somewhere in the trail of breadcrumbs.
    std::ios::pos_type crumbPos;
    SeekResult result = breadCrumbs_.getCrumbAtOrAfterTime( seekSec, seekUsec, crumbPos );
    if ( result == SeekOK )
    {
        // Found it
        file_->seekg( crumbPos );
        return getData( seconds, useconds, id, index );
    }
    else if ( result == SeekQueryInFuture )
    {
        // The piece of data we want is some time into the future.  Have to search forwards.
        IceUtil::Time seekTime = 
            IceUtil::Time::seconds(seekSec) + IceUtil::Time::microSeconds(seekUsec);

        IceUtil::Time dataTime;
        while ( !getData( seconds, useconds, id, index ) ) 
        {
            dataTime = IceUtil::Time::seconds(seconds) + IceUtil::Time::microSeconds(useconds);
            if ( dataTime >= seekTime ) {
                // success
                return 0;
            }
        }
        // eof
        return 1;
    }
    else
    {
        assert( false && "unknown SeekResult." );
        return 1;
    }
}
