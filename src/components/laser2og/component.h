/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef _ORCA2_LASER2OG_COMPONENT_H_
#define _ORCA2_LASER2OG_COMPONENT_H_


#include <orcaice/component.h>

namespace laser2og 
{

class MainThread;
class Laser2Og;

class Component : public orcaice::Component
{
public:
    Component();
    virtual ~Component();

    // component interface
    virtual void start();
    virtual void stop();
    
private:
    MainThread *mainThread_;

};

}; // namespace

#endif
