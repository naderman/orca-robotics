/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_BRICK_COMPONENT_H
#define ORCA2_BRICK_COMPONENT_H

#include <orcaice/component.h>
#include <orcaiceutil/thread.h>

namespace brick
{

class Component: public orcaice::Component
{

public:
    Component();
    virtual ~Component();

    virtual void start();
    virtual void stop();

    // This is an optional function. It is not typically overwritten in
    // Orca components but this can be useful in external projects.
    // This is done here for testing.
    virtual const std::string version() const { return std::string(PROJECT_VERSION); };
    
private:

    orcaiceutil::ThreadPtr mainThread_;
};

} // namespace

#endif
