/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_IGUIELEMENTFACTORY_H
#define ORCAGUI_IGUIELEMENTFACTORY_H

#include <QColor>
#include <vector>
#include <hydroqguielementutil/ihumanmanager.h>
#include <hydroqguielementutil/mouseeventmanager.h>
#include <hydroqguielementutil/shortcutkeymanager.h>
#include <hydroqgui/guielementset.h>
#include <hydroutil/uncopyable.h>

class QSplitter;

namespace hydroqgui
{
    
namespace hydroqguielementutil {
    class GuiElement;
    class GuiElementInfo;
}
        
//!     
//! Intended as a base class for plug-in Gui Element factories.
//!     

//
// Authors: Tobias Kaupp, Alex Brooks, Ben Upcroft
//
class IGuiElementFactory : public hydroutil::Uncopyable
{                                 
public:
    
    virtual ~IGuiElementFactory() {};

    //! Returns a vector of all supported interface types.
    //! Use addSupportedType() to define a list of supported types during
    //! initialization.
    std::vector<QString> supportedElementTypes() const;

    //! An STL version of the function above.
    std::vector<std::string> supportedElementTypesAsStdString() const;

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

    //! Creates a GuiElement using the the information stored in guiElementInfo. 
    //! Returns a NULL pointer if something goes wrong
    virtual ::hydroqguielementutil::GuiElement* create( const ::hydroqguielementutil::GuiElementInfo &guiElementInfo,
                                                         ::hydroqguielementutil::IHumanManager        &humanManager,
                                                         ::hydroqguielementutil::MouseEventManager    &mouseEventManager,
                                                         ::hydroqguielementutil::ShortcutKeyManager   &shortcutKeyManager,
                                                         const GuiElementSet                          &guiElementSet,
                                                         QSplitter                                    *spaceBottomRight = 0 ) const = 0;
                                                                 
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
//     hydroqgui::IGuiElementFactory *createFactory();
// }
typedef hydroqgui::IGuiElementFactory *FactoryMakerFunc();

#endif
