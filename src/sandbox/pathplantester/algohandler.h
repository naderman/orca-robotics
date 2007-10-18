/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_PATHPLANTESTER_ALGORITHM_HANDLER_H
#define ORCA2_PATHPLANTESTER_ALGORITHM_HANDLER_H

#include <orcaiceutil/thread.h>
#include <orcaice/context.h>

#include <orca/pathplanner2d.h>
#include "pathplanner2dconsumerI.h"

namespace pathplantester
{

class AlgoHandler : public orcaiceutil::Thread
{

public: 

    AlgoHandler( const orcaice::Context & context );
    ~AlgoHandler();

    virtual void run();

private:

    orca::PathPlanner2dPrx pathPlanner2dPrx_;
    Ice::ObjectPtr pathPlanner2dConsumerObj_;
    orca::PathPlanner2dConsumerPrx callbackPrx_;
    
    orcaice::Context context_;

    void initNetwork();
    void getConfig();    

    orca::Path2d coarsePath_;

};

} // namespace

#endif
