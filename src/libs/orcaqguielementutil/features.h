/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef QORCA_FEATURES_H
#define QORCA_FEATURES_H

#include <qcolor.h>

//
// Simple file so that all the feature drawing guys use the same color
// scheme.
//

namespace orcaqguielementutil {

    //! Return the color associated with this feature type.
    QColor featureColour( int featureType );

}

#endif
