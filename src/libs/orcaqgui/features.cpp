/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "features.h"
#include <orca/featuremap2d.h>

namespace orcaqgui {

    QColor featureColour( int featureType )
    {
        switch ( featureType )
        {
        case orca::feature::LASERREFLECTOR:
            return Qt::blue;
            break;
        case orca::feature::FOREGROUNDPOINT:
            return Qt::cyan;
            break;
        case orca::feature::DOOR:
            return Qt::magenta;
            break;
        case orca::feature::CORNER:
            return Qt::green;
            break;
        case orca::feature::POSSIBLECORNER:
            return Qt::yellow;
            break;
        default:
            // Don't know what this feature is.
            // Could probably do better, eg return
            // a (deterministic) random colour.
            return Qt::black;
        }
    }

}
