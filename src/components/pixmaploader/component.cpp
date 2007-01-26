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
// #include "fakemaploader.h"
// #include "maploader.h"
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
        cout<<"TRACE(component.cpp): Instantiating fake driver. Not implemented yet." << endl;
//         fakeLoadMap( theMap );
    }
    else if ( driverName == "file" )
    {
        loadMapFromFile(theMap);
//         cout<<"TRACE(component.cpp): Loaded map: " << orcaice::toString(theMap) << endl;
    }
    else
    {
        std::string errString = "Unknown driver type: "+driverName;
        throw orcaice::Exception( ERROR_INFO, errString );
    }

    //
    // EXTERNAL PROVIDED INTERFACES
    //

    pixMapInterface_ = new orcaifaceimpl::PixMapI( "PixMap", context() );
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

void Component::loadMapFromFile(orca::PixMapData &theMap)
{
    Ice::PropertiesPtr prop = properties();
    std::string prefix = tag();
    prefix += ".Config.";

    std::string filename = orcaice::getPropertyWithDefault( prop, prefix+"MapFileName", "mapfilename" );  
    orcapixmapload::loadMap( filename.c_str(), theMap.numCellsX, theMap.numCellsY, theMap.rgbPixels );
    
}

}
