/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <orcaice/orcaice.h>

#include "component.h"
#include <orcamapload/pixmaploadutil.h>

using namespace std;
using namespace orca;

namespace pixmaploader {

Component::Component()
    : orcaice::Component( "PixMapLoader" )
{
}

Component::~Component()
{
}

void
Component::start()
{
    //
    // INITIAL CONFIGURATION
    //

    Ice::PropertiesPtr prop = properties();
    std::string prefix = tag();
    prefix += ".Config.";

    //
    // LOAD THE MAP
    //
    orca::PixMapData theMap;

    std::string driverName = orcaice::getPropertyWithDefault( prop, prefix+"Driver", "fake" );
    if ( driverName == "fake" )
    {
        cout<<"TRACE(component.cpp): Instantiating fake driver." << endl;
        orcaice::setSane( theMap );
    }
    else if ( driverName == "file" )
    {
        loadMapFromFile(theMap);
    }
    else
    {
        std::string errString = "Unknown driver type: "+driverName;
        throw orcaiceutil::Exception( ERROR_INFO, errString );
    }

    cout<<"TRACE(component.cpp): Loaded map: " << orcaice::toString(theMap) << endl;
    
    //
    // EXTERNAL PROVIDED INTERFACES
    //

    pixMapInterface_ = new orcaifaceimpl::PixMapImpl( "PixMap", context() );
    pixMapInterface_->initInterface();
    pixMapInterface_->localSetAndSend( theMap );

    ////////////////////////////////////////////////////////////////////////////////

    //
    // ENABLE NETWORK CONNECTIONS
    //
    activate();

    //
    // MAIN DRIVER LOOP: No need to do anything here, since we don't need our own thread.
    //
}

void Component::stop()
{
    // Nothing to do, since we don't have our own thread.
}

void Component::loadMapFromFile(orca::PixMapData &map)
{
    Ice::PropertiesPtr prop = properties();
    std::string prefix = tag();
    prefix += ".Config.";

    std::string filename = orcaice::getPropertyWithDefault( prop, prefix+"MapFileName", "mapfilename" );  
    orcapixmapload::loadMap( filename.c_str(), map.numCellsX, map.numCellsY, map.rgbPixels );
    
    map.offset.p.x = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Offset.X", 0.0 );
    map.offset.p.y = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Offset.Y", 0.0 );
    map.offset.o   = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Offset.Orientation", 0.0 ) * M_PI/180.0;

    // since we know that map size in pixels, we can calculate the cell size
    float worldSizeX = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Size.X", 20.0 );
    float worldSizeY = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Size.Y", 20.0 );
    map.metresPerCellX = worldSizeX / (float)map.numCellsX;
    map.metresPerCellY = worldSizeY / (float)map.numCellsY;

    // Make up a timestamp
    map.timeStamp.seconds  = 0;
    map.timeStamp.useconds = 0;
    
}

}
