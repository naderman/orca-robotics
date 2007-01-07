/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_DEFAULT_FACTORY_H
#define ORCAGUI_DEFAULT_FACTORY_H

#include <QString>

#include <orca/orca.h>
#include <orcaice/context.h>
#include <orcaqgui/guielementfactory.h>
#include <QPainter>

namespace orcaqgui
{
    class IHumanManager;
    class GuiElement;

    //!
    //! Generates 2D Gui Elements based on its type.
    //!
    class DefaultFactory : public orcaqgui::GuiElementFactory
    {
    public:

        DefaultFactory();

        // returns a NULL pointer if something goes wrong
        virtual orcaqgui::GuiElement* create(const orcaice::Context     &context,
                                             const QStringList          &interfaceIds,
                                             const QStringList          &proxyStrList,
                                             QColor                      suggestedColor,
                                             orcaqgui::IHumanManager    *humanManager ) const;

    private:
        
    };

}


//
// Hook for dynamic loading
//
extern "C" {
    orcaqgui::GuiElementFactory *createFactory();
}

#endif
