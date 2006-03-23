#include "component.h"
#include "fakemaploader.h"
#include "maploadutil.h"

// implementations of Ice objects
#include "ogmap_i.h"

#include <orcaice/orcaice.h>

using namespace std;
using namespace orca;
using orcaice::operator<<;

Component::Component()
    : orcaice::Component( "OgMapLoader" )
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

    Ice::PropertiesPtr prop = communicator()->getProperties();
    std::string prefix = tag();
    prefix += ".Config.";

    //
    // LOAD THE MAP
    //
    orca::OgMapDataPtr theMap = new OgMapData;

    std::string driverName = orcaice::getPropertyWithDefault( prop, prefix+"Driver", "fake" );
    if ( driverName == "fake" )
    {
        cout<<"TRACE(maploadercomponent.cpp): Instantiating fake driver" << endl;
        fakeLoadMap( theMap );
    }
    else if ( driverName == "real" )
    {
        std::string mapFileName = orcaice::getPropertyWithDefault( prop, prefix+"MapFileName", "mapfilename" );
        float worldSizeX = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Size.X", 20.0 );
        float worldSizeY = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Size.Y", 20.0 );
        float originX     = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Origin.X", 0.0 );
        float originY     = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Origin.Y", 0.0 );
        float originTheta = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Origin.Orientation", 0.0 ) * M_PI/180.0;
        bool  negate      = orcaice::getPropertyAsIntWithDefault( prop, prefix+"Negate", true );

        maploadutil::loadMap( mapFileName,
                              negate,
                              theMap->numCellsX,
                              theMap->numCellsY,
                              theMap->data );

        theMap->origin.p.x = originX;
        theMap->origin.p.y = originY;
        theMap->origin.o   = originTheta;

        // since we know that map size in pixels, we can calculate the cell size
        theMap->metresPerCellX = worldSizeX / (float)theMap->numCellsX;
        theMap->metresPerCellY = worldSizeY / (float)theMap->numCellsY;

        cout<<"TRACE(component.cpp): Loaded map: " << theMap << endl;
    }
    else
    {
        std::string errString = "Unknown driver type: "+driverName;
        throw orcaice::Exception( ERROR_INFO, errString );
    }

    //
    // EXTERNAL PROVIDED INTERFACES
    //
    // create servant for direct connections
    ogMapObj_ = new OgMapI( theMap, "OgMap", context() );
    orcaice::createInterfaceWithTag( context(), ogMapObj_, "OgMap" );

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
