/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCALOG_ICE_WRITE_HELPER_H
#define ORCALOG_ICE_WRITE_HELPER_H

#include <Ice/Ice.h>

namespace orcalog
{

class IceWriteHelper 
{
public:
    IceWriteHelper( const Ice::CommunicatorPtr & comm );

    Ice::OutputStreamPtr stream_;

    void write( std::ofstream *file_, char id=0 );
};

}
#endif
