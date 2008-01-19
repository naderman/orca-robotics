/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef FAKE_ALGORITHM_DRIVER_H
#define FAKE_ALGORITHM_DRIVER_H

#include "algodriver.h"

namespace pathplanner
{

class FakeDriver : public AlgoDriver 
{

public:

    FakeDriver()
    { };

    // Computes the path
    virtual void computePath( const orca::PathPlanner2dTask& task,
                              orca::PathPlanner2dData& pathData );

};

} // namespace

#endif
