#ifndef ORCAQGUI2D_IGUIELEMENTFACTORY_H
#define ORCAQGUI2D_IGUIELEMENTFACTORY_H

#include <hydroqgui/hydroqgui.h>
#include <orcaice/context.h>

namespace orcaqgui2d {

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
