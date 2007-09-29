/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_FAITH_LOCALISER_MAIN_LOOP_H
#define ORCA2_FAITH_LOCALISER_MAIN_LOOP_H

#include <orcaiceutil/safethread.h>
#include <orcaice/context.h>

namespace faithlocaliser
{

class MainLoop : public orcaiceutil::SafeThread
{

public:

    MainLoop( const orcaice::Context &context );
    ~MainLoop();

    // from SafeThread
    virtual void walk();

private:
    double stdDevPosition_;
    double stdDevHeading_;

    orcaice::Context context_;
};

} // namespace

#endif
