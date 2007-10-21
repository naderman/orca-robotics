/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef _ORCA2_OGNODE_COMPONENT_H_
#define _ORCA2_OGNODE_COMPONENT_H_

#include <orcaice/component.h>
#include <hydroutil/thread.h>

namespace ognode
{

class Handler;

class Component : public orcaice::Component
{
public:
    Component();
    virtual ~Component();

    // component interface
    virtual void start();
    virtual void stop();

private:
    hydroutil::ThreadPtr handler_;

};

} // namespace

#endif
