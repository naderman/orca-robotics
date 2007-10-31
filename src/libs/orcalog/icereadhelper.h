/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCALOG_ICE_READ_HELPER_H
#define ORCALOG_ICE_READ_HELPER_H

#include <Ice/Ice.h>

namespace orcalog
{

class IceReadHelper 
{
public:
    IceReadHelper( const Ice::CommunicatorPtr & comm, std::ifstream *file );

    Ice::InputStreamPtr stream_;

    void read();

    char id() { return id_; };

private:
    std::vector<Ice::Byte> byteData_;

    char id_;
};

}
#endif
