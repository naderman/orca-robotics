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

#include <orcaqgui/ihumanmanager.h>

#include "gpselement.h"

using namespace std;
using namespace orca;
using namespace orcaqgui;

GpsElement::GpsElement( const orcaice::Context  &context,
                        const std::string       &interfaceTag,
                        IHumanManager           *humanManager,
                        int                      timeoutMs )
    : gpsListener_(context,interfaceTag),
      context_(context),
      humanManager_(humanManager),
      timeoutMs_(timeoutMs),
      isConnected_(false),
      gotDescription_(false),
      displayGps_(true)
{
    if (gpsListener_.connect() == 0) 
        isConnected_=true;
}  

void
GpsElement::actionOnConnection()
{
    if (!isConnected_) return;
    
    try
    {
        const orca::GpsPrx& prx = gpsListener_.proxy();
        orca::GpsDescription descr = prx->getDescription();
        gpsOff_ = descr.offset;
    }
    catch ( ... )
    {
        humanManager_->showStatusMsg(Warning, "Problem getting description from GPS interface. Will try again later.");
        return;
    }
    
    humanManager_->showStatusMsg(Information, "Got GPS description");
    gotDescription_ = true;    
}
        
bool
GpsElement::needToUpdate()
{
    if ( !gotDescription_ ) 
    {
        actionOnConnection();
        return false;
    }
    
    if ( !gpsListener_.buffer().isEmpty() )
    {
        // An object has arrived in one of the buffers.  We need to update.
        return true;
    }
    
    if ( !isConnected_ )
    {
        if ( gpsListener_.connect() == 0 )
        {
            actionOnConnection();
            isConnected_ = true;
        }
        return false;
    }
    
    // The buffer is empty.  How long since we last received something?
    if ( timeoutMs_ != -1 && gpsListener_.msSinceReceipt() >= timeoutMs_ ) 
    {
        cout << "TRACE(gpselement.cpp): Timing out..." << endl;
        cout << "TRACE(gpselement.cpp): Haven't received anything in gpselement for " << gpsListener_.msSinceReceipt()  << "ms" << endl;
        gpsPainter_.clear();
        gpsListener_.resetTimer();
        if ( gpsListener_.connect() == 0 ) actionOnConnection();

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
        
        transformToGuiCs( data.easting, 
                         data.northing, 
                         data.heading,
                         gpsOff_.p.x, 
                         gpsOff_.p.y, 
                         gpsOff_.o, 
                         x_, 
                         y_,
                         theta_ );
        
        cout << "data.easting,northing,heading: " << data.easting << " " << data.northing << " " << RAD2DEG(data.heading) << endl;
        cout << "gpsOff(x,y,o): " << gpsOff_.p.x << " " << gpsOff_.p.y << " " << RAD2DEG(gpsOff_.o) << endl;
        cout << "gui(x,y,theta): " << x_ << " " << y_ << " " << RAD2DEG(theta_) << endl << endl;
        
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
//                << "  gpsOrigin_: " << orcaice::toString( gpsOrigin_ ) << endl
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
