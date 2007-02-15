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
    gpsListener_.connect();
    
    // get gps specific properties from the cfg file   
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag();
    prefix += ".Config.";

    orcaice::setInit( gpsOrigin_ );
    orcaice::getPropertyAsCartesianPoint( prop, prefix+"Gps.Origin", gpsOrigin_ );

    stringstream ss; ss << "GpsElement: GPS Origin: " << orcaice::toString( gpsOrigin_ );
    context_.tracer()->info( ss.str() );
}  
        
bool
GpsElement::needToUpdate()
{
    if ( !gpsListener_.buffer().isEmpty() )
    {
        // An object has arrived in one of the buffers.  We need to update.
        return true;
    }
    
    // The buffer is empty.  How long since we last received something?
    if ( timeoutMs_ != -1 && gpsListener_.msSinceReceipt() >= timeoutMs_ ) 
    {
        cout << "TRACE(gpselement.cpp): Timing out..." << endl;
        cout << "TRACE(gpselement.cpp): Haven't received anything in gpselement for " << gpsListener_.msSinceReceipt()  << "ms" << endl;
        gpsPainter_.clear();
        gpsListener_.resetTimer();
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
        gpsListener_.buffer().getAndPop( data );
        
        x_ = data.easting - gpsOrigin_.x;
        y_ = data.northing - gpsOrigin_.y;
        
        theta_ = data.heading;
        
        if ( displayGps_ )
        {
            gpsPainter_.setData( x_, y_, theta_ );
        }

        int verbosity = context_.tracer()->verbosity( orcaice::Tracer::DebugTrace,
                                                      orcaice::Tracer::ToAny );
        if ( verbosity >=5 )
        {
            stringstream ss;
            ss << "GpsElement: " << endl
               << "  data:       " << orcaice::toString( data ) << endl
               << "  gpsOrigin_: " << orcaice::toString( gpsOrigin_ ) << endl
               << "  x_,y_:      " << x_ << ", " << y_;
            context_.tracer()->debug( ss.str(), 5 );
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
