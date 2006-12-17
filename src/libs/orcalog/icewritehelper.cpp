/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <fstream>

#include "icewritehelper.h"

using namespace std;
using namespace orcalog;

IceWriteHelper::IceWriteHelper( const Ice::CommunicatorPtr & comm )
    : stream_(0)
{
    stream_ = Ice::createOutputStream( comm );
}

void 
IceWriteHelper::write( std::ofstream *file, char id )
{
    stream_->writePendingObjects();

    // this actually writes to data to the byte vector
    vector<Ice::Byte> byteData;
    stream_->finished(byteData);
    
    file->write( (char*)&id, sizeof(char) );

    size_t length = byteData.size();
    file->write( (char*)&length, sizeof(size_t) );
    file->flush();

    file->write( (char*)&byteData[0], length);
    file->flush();

//     cout << "ByteDataSize: " << byteData.size() << endl;
}
