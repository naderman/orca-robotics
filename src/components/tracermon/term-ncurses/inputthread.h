/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TRACERMON_TERM_NCURSES_INPUT_HANDLER_H
#define ORCA2_TRACERMON_TERM_NCURSES_INPUT_HANDLER_H

#include <hydroutil/safethread.h>
#include <orcaice/context.h>
#include "../user.h"
#include "../network.h"

namespace tracermon
{

class MainThread : public hydroutil::SafeThread
{
public:

    MainThread( tracermon::Network& network, tracermon::User& user, const orcaice::Context& context );
    virtual ~MainThread();

    // from SafeThread
    virtual void walk();
    
private:

    tracermon::Network&    network_;
    tracermon::User&       user_;
};

} // namespace

#endif
