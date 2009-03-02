/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef __pingerI_h__
#define __pingerI_h__

// include provided interfaces
#include <orca/pinger.h>
#include <orcaice/context.h>

namespace hydroping {
    class Pinger;
}

namespace pingserver {

class PingerI : public orca::Pinger
{
public:

    PingerI( const std::string      &ifaceTag,
             const orcaice::Context &context );
    ~PingerI();

    // Remote calls, defined in orca::Pinger
    virtual double ping( const std::string &host, const Ice::Current& );

    // local calls
    void initInterface();

private:

    // Does the work
    hydroping::Pinger *pinger_;

    std::string ifaceTag_;
    orcaice::Context context_;
};

typedef IceUtil::Handle<PingerI> PingerIPtr;

}

#endif
