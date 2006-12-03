
#ifndef PLATFORMCSFINDER_H
#define PLATFORMCSFINDER_H

#include <orcaqgui/guielementmodel.h>
#include <QString>
#include <QList>

namespace orcaqgui {

//!
//! @brief Class to work out the coordinate-system of a platform.  
//!        Applies a bunch of heuristics.
//!
//! Keeping this thing in a separate class keeps the dodgy hack separate
//! from other things, and makes it easy to supply a different dodgy hack.
//!
//! @author Alex Brooks
//!
class PlatformCSFinder
{

public: 

    PlatformCSFinder() {};

    // returns false if it doesn't know.
    bool findPlatformCS( const QList<GuiElementModel::InterfaceNode*> elements,
                         const QString                                &platform,
                         float                                        &x,
                         float                                        &y,
                         float                                        &theta );

private: 


};

}

#endif
