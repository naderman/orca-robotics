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

#include "icereadhelper.h"
#include "exceptions.h"

using namespace std;
using namespace orcalog;

IceReadHelper::IceReadHelper( const Ice::CommunicatorPtr & comm, std::ifstream *file )
    : stream_(0)
{
    // read the id which distinguishes between diff types of objects
    file->read( (char*)&id_, sizeof(char) );

    size_t length;
    file->read( (char*)&length, sizeof(length) );

    if ( length==0 ) {
        throw orcalog::FileException( ERROR_INFO, "attempt to read object of size 0" );
    }

    if ( file->bad() ) {
        throw orcalog::FileException( ERROR_INFO, "log file is 'bad'" );
    }

    byteData_.resize( length );
    file->read( (char*)&byteData_[0], length );

    if ( byteData_.empty() ) {
        throw orcalog::FileException( ERROR_INFO, "read object of size 0" );
    }

    if ( file->bad() ) {
        throw orcalog::FileException( ERROR_INFO, "log file is 'bad'" );
    }

    stream_ = Ice::createInputStream( comm, byteData_ );
}

void 
IceReadHelper::read()
{
    stream_->readPendingObjects();

//     cout << "ByteDataSize: " << byteData_.size() << endl;
}
