/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include "featurecolours.h"
#include <orca/featuremap2d.h>

namespace orcaqguielementutil {

    namespace {
        // Color values from /etc/X11/rgb.txt
        const QColor DARK_GRAY(169, 169, 169);
        
        const QColor ORANGE(255, 165, 0);
        const QColor DARK_ORANGE(200, 110, 0); // a bit diff from /etc/X11/rgb.txt, coz I couldn't tell the diff.
    }

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
        case orca::feature::EXTERNALCORNER:
            return Qt::green;
            break;
        case orca::feature::INTERNALCORNER:
            return Qt::yellow;
            break;
        case orca::feature::VEHICLEPOSE:
            return Qt::gray;
            break;
        case orca::feature::GPSFIX:
            return ORANGE;
            break;

        case 10 + orca::feature::LASERREFLECTOR:
            return Qt::darkBlue;
            break;
        case 10 + orca::feature::FOREGROUNDPOINT:
            return Qt::darkCyan;
            break;
        case 10 + orca::feature::DOOR:
            return Qt::darkMagenta;
            break;
        case 10 + orca::feature::CORNER:
            return Qt::darkGreen;
            break;
        case 10 + orca::feature::LINE:
            return Qt::darkCyan;
            break;
        case 10 + orca::feature::EXTERNALCORNER:
            return Qt::darkGreen;
            break;
        case 10 + orca::feature::INTERNALCORNER:
            return Qt::darkYellow;
            break;
        case 10 + orca::feature::VEHICLEPOSE:
            return DARK_GRAY;
            break;
        case 10 + orca::feature::GPSFIX:
            return DARK_ORANGE;
            break;

        default:
            // Don't know what this feature is.
            // Could probably do better, eg return
            // a (deterministic) random colour.
            return Qt::gray;
        }
    }

}
