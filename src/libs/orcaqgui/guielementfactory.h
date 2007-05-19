/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_GUIELEMENTFACTORY_H
#define ORCAGUI_GUIELEMENTFACTORY_H

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

//
// Authors: Tobias Kaupp, Alex Brooks, Ben Upcroft
//
class GuiElementFactory
{                                 
public:
    
    virtual ~GuiElementFactory() {};

    //! Returns a vector of all supported interface types.
    //! Use addSupportedType() to define a list of supported types during
    //! initialization.
    std::vector<QString> supportedElementTypes() const;

    //! Returns TRUE if the specified list of interface types is supported by this factory.
    //! Use addSupportedType() to define a list of supported types during
    //! initialization.
    bool isSupported( const QString &elementType ) const;
    
    //! Looks up the elementType corresponding to the ids coming back from the required interfaces
    //! when connecting through clicking on the registry view.
    //! Example: 'ids' ::orca::LaserScanner2d maps to 'elementType' LaserScanner2d
    //! Needs to be augmented only for complicated elementTypes which have more than one interface.
    //! Returns true if an elementType was found otherwise false.
    virtual bool lookupElementType( const QStringList &ids, QString &elementType ) const = 0;

    //! Creates a GuiElement using the elementType and elementDetails information.
    //! The elementType must be in the list of supportedElementTypes_.
    //! ElementDetails contains the connection details relevant to interfaces to be set up within the element.
    //! ElementDetails is a list of strings that look like this: interface@platform/component
    //! Returns a NULL pointer if something goes wrong
    //! The factory should set a colour with guiElement->setColor, possibly
    //! using the suggested color.
    virtual GuiElement* create( const orcaice::Context         &context,
                                const QString                  &elementType,
                                const QStringList              &elementDetails,
                                QColor                          suggestedColor,
                                orcaqgui::IHumanManager        *humanManager ) const = 0;

protected:

    //! Adds type to the list of supported elements.
    void addSupportedType( const QString & elementType );


private:
    std::vector<QString> supportedElementTypes_;
    
};

} // namespace


// Function for instantiating plug-in factories.
// A plug-in library must have a function like so:
// extern "C" {
//     orcaqgui::GuiElementFactory *createFactory();
// }
typedef orcaqgui::GuiElementFactory *FactoryMakerFunc();

#endif
