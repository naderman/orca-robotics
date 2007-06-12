/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
        case orca::feature::LINE:
            return Qt::black;
            break;
        case 7:
            return Qt::darkYellow;
            break;
        case 8:
            return Qt::yellow;
            break;
        case 9:
            return Qt::darkMagenta;
            break;
        case 10:
            return Qt::darkRed;
            break;
        default:
            // Don't know what this feature is.
            // Could probably do better, eg return
            // a (deterministic) random colour.
            return Qt::darkBlue;
        }
    }

}
