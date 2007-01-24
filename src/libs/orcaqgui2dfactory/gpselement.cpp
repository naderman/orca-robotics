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
        return;
    }

    orca::GpsMapGridData data;
    
    if ( !gpsListener_.buffer().isEmpty() )
    {
        // cout << "TRACE(gpselement.cpp): buffer has object" << endl;

            gpsListener_.buffer().getAndPop( data );
            // cout << "TRACE(gpselement.cpp): Just got data from buffer" << endl;
            // shift the coordinates by gpsOrigin coordinates so that orcaview starts at (0,0)
//             x_ = gpsOrigin_.x - data.northing;
//             y_ = gpsOrigin_.y - data.easting;
            
//             x_ = data.northing - gpsOrigin_.x;
//             y_ = data.easting - gpsOrigin_.y;
            
            x_ = data.easting - gpsOrigin_.y;
            y_ = data.northing - gpsOrigin_.x;
            
            theta_ = data.heading;
            
            if ( displayGps_ )
            {
                gpsPainter_.setData( x_, y_, theta_ );
            }
    } 
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
        case 1 :
            displayGps_ = !displayGps_;
            break;
        default:
            assert(false);
            break;
    }
}
