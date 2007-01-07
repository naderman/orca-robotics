/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <QPainter>
#include <QString>

#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>

#include "gpselement.h"

using namespace std;
using namespace orca;
using namespace orcaqgui;

GpsElement::GpsElement( const orcaice::Context  &context,
                        const std::string       &interfaceTag,
                        int                     timeoutMs )
    : timeoutMs_(timeoutMs),
      gpsListener_(context,interfaceTag),
      context_(context),
      displayGps_(true)
{
    cout<<"TRACE(gpselement.cpp): WARNING: connecting in constructor!" << endl;
    gpsListener_.connect();
    
    // get gps specific properties from the cfg file   
    getGpsProperties();   
}

void
GpsElement::getGpsProperties()
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag();
    prefix += ".Config.";

    orcaice::setInit( gpsOrigin_ );
    orcaice::getPropertyAsCartesianPoint( prop, prefix+"Gps.Origin", gpsOrigin_ );
    return;
}  
        
bool
GpsElement::needToUpdate()
{
    if ( !gpsListener_.buffer().isEmpty() )
    {
        // An object has arrived in one of the buffers.  We need to update.
        // cout << "TRACE(gpselement.cpp): An object has arrived in the buffer" << endl;
        return true;
    }

    // cout << "timeoutMs_: " << timeoutMs_ << endl;
    
    // The buffer is empty.  How long since we last received something?
    if ( gpsListener_.msSinceReceipt() >= timeoutMs_ ) 
    {
        cout << "TRACE(gpselement.cpp): Timing out..." << endl;
        gpsPainter_.clear();
        cout << "TRACE(gpselement.cpp): Haven't received anything in gpselement for " << gpsListener_.msSinceReceipt()  << "ms" << endl;
        gpsListener_.connect();

    }
    return false;
}

void
GpsElement::update()
{
    if ( !needToUpdate() )
    {
        cout << "TRACE(gpselement.cpp): buffer is empty" << endl;
    }

    if ( !gpsListener_.buffer().isEmpty() )
    {
        // cout << "TRACE(gpselement.cpp): buffer has object" << endl;
        if ( displayGps_ )
        {
            gpsListener_.buffer().getAndPop( gpsMapGridData_ );
            // cout << "TRACE(gpselement.cpp): Just got data from buffer" << endl;
            // shift the coordinates by gpsOrigin coordinates so that orcaview starts at (0,0)
            gpsMapGridData_.northing -= gpsOrigin_.x;
            gpsMapGridData_.easting -= gpsOrigin_.y;
            gpsPainter_.setData( gpsMapGridData_ );
        }
    }
//     if ( !localiseListener_.buffer().isEmpty() )
//     {
//         localiseListener_.buffer().getAndPop( localiseData_ );
//         localisePainter_.setData( localiseData_ );
//     }
//    std::cout<<"TRACE(gpselement.cpp): update(): pos: " << localiseData_ << std::endl;
//    const Pose2dHypothesis &h = orcaice::mlHypothesis( localiseData_ );
//     x_ = h.mean.p.x;
//     y_ = h.mean.p.y;
//     theta_ = h.mean.o;

    x_ = gpsMapGridData_.northing;
    y_ = gpsMapGridData_.easting;
    theta_ = gpsMapGridData_.heading;
    return; 
}

QStringList
GpsElement::contextMenu()
{
    QStringList s;
    s<<"Toggle History"<<"Toggle Gps Display";
    return s;
}

void
GpsElement::execute( int action )
{
    switch ( action )
    {
        case 0 :
            gpsPainter_.toggleDisplayHistory();
            break;
//         case 1 :
//             displayGps_ = !displayGps_;
//             break;
        default:
            assert(false);
            break;
    }
}
