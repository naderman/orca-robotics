/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAWALL_INTERFACE_SIM_H
#define ORCA2_ORCAWALL_INTERFACE_SIM_H

#include <hydroutil/safethread.h>
#include <orcaice/context.h>

namespace orcawall
{

class InterfaceSim : public hydroutil::SafeThread
{

public:
    //! Constructor
    InterfaceSim( const std::string& tag, const orcaice::Context& context ) :
        SafeThread(context.tracer()),
        tag_(tag),
        context_(context) {};
        
protected:
    //! This is an implementation of a config file convention for the Wall component.
    //! E.g. an interface simulator with tag InterfaceTag7 would have a line like this
    //! in the config file:
    //! @verbatim
    //! Wall.Config.InterfaceTag7.SomeParameter=88
    //! @endverbatim
    std::string configPrefix() const 
    { 
        return context_.tag()+".Config."+tag_+".";
    };
    
    //! Component tag, used to access configuration settings.
    std::string tag_;

    //! Component communication context with pointers to Communicator, Tracer, etc.
    orcaice::Context context_;

};

} // namespace

#endif
