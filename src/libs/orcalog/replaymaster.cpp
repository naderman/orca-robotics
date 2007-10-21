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

#include "replaymaster.h"
#include "utils.h"

using namespace std;
using namespace orcalog;


ReplayMaster::ReplayMaster( const std::string &filename, const orcaice::Context& context )
    : context_(context)
{
    // create master file
    file_ = new ifstream( filename.c_str() );
    if ( !file_->is_open() ) {
        context_.tracer()->warning( "Could not open master file " + filename );
        throw orcalog::FileException( ERROR_INFO, "Could not open master file " + filename );
    }

    // remember the dir where the master file is located
    // the individual log files MUST be in the same dir.
    dir_ = hydroutil::dirname( filename );
}

ReplayMaster::~ReplayMaster()
{
    if ( file_ ) {
        file_->close();
        delete file_;
    }
}


void
ReplayMaster::getLogs( std::vector<std::string>& filenames,
                        std::vector<std::string>& interfaceTypes,
                        std::vector<std::string>& formats,
                        std::vector<bool>& enableds )
{
    filenames.clear();
    interfaceTypes.clear();
    formats.clear();

    std::string line;
    std::string filename;
    std::string interfaceType;
    std::string format;
    bool enabled;
    // todo: go to top of the file

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

        filenames.push_back( filename );
        interfaceTypes.push_back( interfaceType );
        formats.push_back( format );
        enableds.push_back( enabled );

        context_.tracer()->debug( "Parsed header: file="+filename+" type="+interfaceType+" fmt="+format, 5);
    }
}

int 
ReplayMaster::seekDataStart()
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
ReplayMaster::getData( int& seconds, int& useconds, int& id, int& index )
{
    std::string line;

    while ( !file_->eof() ) {
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
//         stringstream ss;
//         ss << "parsed data line sec="<<seconds<<" usec="<<useconds<<" id="<<id<<" idx="<<index;
//         context_.tracer()->debug( line, 10);
//         context_.tracer()->debug( ss.str(), 10);
        return 0;
    }
    
//     cout<<"ReplayMaster::getData: this is EOF"<<endl;
    return 1;
}

int 
ReplayMaster::getData( int& seconds, int& useconds, int& id, int& index, int seekSec, int seekUsec )
{
//     cout<<"seeking "<<seekSec<<"s "<<seekUsec<<"us"<<endl;
    IceUtil::Time seekTime = 
            IceUtil::Time::seconds(seekSec) + IceUtil::Time::microSeconds(seekUsec);

    IceUtil::Time dataTime;
    while ( !getData( seconds, useconds, id, index ) ) {
//         cout<<"read "<<seconds<<"s "<<useconds<<"us"<<endl;
        dataTime = IceUtil::Time::seconds(seconds) + IceUtil::Time::microSeconds(useconds);
        if ( dataTime >= seekTime ) {
            // success
            return 0;
        }
    }
    // eof
    return 1;
}
