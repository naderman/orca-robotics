/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef _ORCA2_LASER2OG_COMPONENT_H_
#define _ORCA2_LASER2OG_COMPONENT_H_


#include <orcaice/component.h>
#include <orcaice/ptrbuffer.h>
#include <orca/rangescanner2d.h>
#include <orca/ogfusion.h>
#include <orca/localise2d.h>

namespace laser2og 
{

class Handler;
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
    Handler *handler_;

};

}; // namespace

#endif
