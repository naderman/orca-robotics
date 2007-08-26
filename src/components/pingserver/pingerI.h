/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef __pingerI_h__
#define __pingerI_h__

// include defnition of Ice runtime
#include <Ice/Ice.h>

// include provided interfaces
#include <orca/pinger.h>
#include <orcaice/context.h>

namespace orcaping {
    class Pinger;
}

namespace pingserver {

class PingerI : public orca::Pinger
{
public:

    PingerI( const std::string      &ifaceTag,
             const orcaice::Context &context );
    ~PingerI();

    // Remote calls:
    double ping( const std::string &host, const Ice::Current& );

    // local calls
    void initInterface();

private:

    // Does the work
    orcaping::Pinger *pinger_;

    std::string ifaceTag_;
    orcaice::Context context_;
};

}

#endif
