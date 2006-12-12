/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_GUIELEMENTFACTORY_H
#define ORCAGUI_GUIELEMENTFACTORY_H

#include <QStringList>
#include <QColor>
#include <orcaice/context.h>
#include <vector>

namespace orcaqgui
{
    
class IHumanManager;
class GuiElement;
        
//!     
//! Intended as a base class for plug-in Gui Element factories.
//!     
//! NOTE: Inherited classes need to implement a setPainter() method for
//! passing specific a painter pointer to elements when created.
//! Factories that inherit from this class must downcast the painter
//! to the correct type.

// Author: Alex Brooks, Tobias Kaupp, Ben Upcroft
//     
class GuiElementFactory
{                                 
public:
    
    virtual ~GuiElementFactory() {};

    //! Returns a vector of all supported interface types.
    //! Use addSupportedType() to define a list of supported types during
    //! initialization.
    std::vector<QStringList> supportedInterfaceIds() const;

    //! Returns TRUE if the specified list of interface types is supported by this factory.
    //! Use addSupportedType() to define a list of supported types during
    //! initialization.
    bool isSupported( const QStringList &interfaceIds ) const;

    //
    // The factory should set a colour with guiElement->setColor, possibly
    // using the suggested color.
    //
    // returns a NULL pointer if something goes wrong
    virtual GuiElement* create( const orcaice::Context         &context,
                                const QStringList              &interfaceId,
                                const QStringList              &proxyStrList,
                                QColor                          suggestedColor,
                                orcaqgui::IHumanManager  *msgDisplayer ) const = 0;

protected:

    //! Adds type to the list of supported interfaces.
    void addSupportedType( const QStringList & interfaceType );


private:
    std::vector<QStringList> supportedInterfaceIds_;
    
};

} // namespace


// Function for instantiating plug-in factories.
// A plug-in library must have a function like so:
// extern "C" {
//     orcaqgui::GuiElementFactory *createFactory();
// }
typedef orcaqgui::GuiElementFactory *FactoryMakerFunc();

#endif
