/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <orcaice/orcaice.h>

#include "component.h"
#include <hydromapload/pixmaploadutil.h>

using namespace std;
using namespace pixmaploader;

namespace {

void convert( const std::vector<char> &rgbPixels,
                orca::Pixels &orcaPixels )
{
    orcaPixels.resize( rgbPixels.size()/3 );

    for ( uint i=0; i < rgbPixels.size(); i+=3 )
    {
        orca::Pixel pix;

        pix.r = rgbPixels[i];
        pix.g = rgbPixels[i+1];
        pix.b = rgbPixels[i+2];

        orcaPixels[i/3] = pix;
    }
}

}

/////////////////////////////////////////


Component::Component()
    : orcaice::Component( "PixMapLoader" )
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
        throw hydroutil::Exception( ERROR_INFO, errString );
    }

    cout<<"TRACE(component.cpp): Loaded map: " << orcaice::toString(theMap) << endl;
    
    //
    // EXTERNAL PROVIDED INTERFACES
    //

    pixMapInterface_ = new orcaifaceimpl::PixMapImpl( "PixMap", context() );
    pixMapInterface_->initInterface();
    pixMapInterface_->localSetAndSend( theMap );

    //
    // ENABLE NETWORK CONNECTIONS
    //
    activate();
}

void 
Component::loadMapFromFile(orca::PixMapData &map)
{
    Ice::PropertiesPtr prop = properties();
    std::string prefix = tag();
    prefix += ".Config.";

    std::string filename = orcaice::getPropertyWithDefault( prop, prefix+"MapFileName", "mapfilename" );  

    std::vector<char> rgbPixels;
    hydromapload::loadMap( filename.c_str(), map.numCellsX, map.numCellsY, rgbPixels );
    convert( rgbPixels, map.rgbPixels );
    
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
