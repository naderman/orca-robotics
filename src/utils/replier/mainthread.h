/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_REPLIER_HANDLER_H
#define ORCA2_REPLIER_HANDLER_H

#include <hydroutil/thread.h>
#include <orcaice/context.h>

namespace replier
{


class Handler: public hydroutil::Thread
{    	
public:
    Handler( const orcaice::Context & context );
    ~Handler();

    virtual void run();

private:

    orcaice::Context context_;

};

} // namespace
#endif
