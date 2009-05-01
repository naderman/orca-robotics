/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAQGUI2D_IGUIELEMENTFACTORY_H
#define ORCAQGUI2D_IGUIELEMENTFACTORY_H

#include <hydroqgui/iguielementfactory.h>
#include <orcaice/context.h>

namespace orcaqgui {

//!
//! @brief Simply allows for orcaice::Context to be set.
//!
//! @author Alex Brooks
//!
class IGuiElementFactory : public hydroqgui::IGuiElementFactory
{
    public:
        virtual void setContext( const orcaice::Context &context )=0;
};

}

#endif
