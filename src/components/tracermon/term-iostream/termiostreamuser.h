/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TRACERMON_TERM_IOSTREAM_USER_H
#define ORCA2_TRACERMON_TERM_IOSTREAM_USER_H

#include <hydroutil/safethread.h>
#include <orcaice/context.h>
#include <hydroutil/eventqueue.h>
#include "../user.h"
#include "../network.h"

namespace tracermon
{

class TermIostreamUser : public hydroutil::SafeThread, public User
{
public:

    TermIostreamUser( const orcaice::Context & context );
    virtual ~TermIostreamUser();

    // from Thread
    virtual void walk();

    // from User
    virtual void enable( Network* network );

    // no inputs for this driver
    virtual void previousField() {};
    virtual void nextField() {};
    virtual void valueUp() {};
    virtual void valueDown() {};
    virtual void action() {};

    virtual void newTraceMessage( const orca::TracerData & data );

    virtual void newVerbosityLevel( int error, int warn, int info, int debug );

    virtual void newLocalTrace( const std::string& msg );
    
private:

    Network* network_;
    hydroutil::EventQueuePtr events_;
   
    orcaice::Context context_;
};

} // namespace

#endif
