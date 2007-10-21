/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAICE_TESTER_TEST_LOOP_H
#define ORCA2_ORCAICE_TESTER_TEST_LOOP_H

#include <Ice/Ice.h>

#include <hydroutil/thread.h>
#include <orcaice/context.h>

#include <orca/home.h>
#include <orca/status.h>

class TestLoop : public hydroutil::Thread
{
public:

    TestLoop( const orca::StatusConsumerPrx & statusPrx, const orcaice::Context & context );
    ~TestLoop();

    virtual void run();

private:

    // local object to receive data
    orca::StatusConsumerPrx statusCallbackPrx_;

    // component current context
    orcaice::Context context_;
};

#endif

