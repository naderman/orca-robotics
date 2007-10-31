/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaifaceimpl/localise2dImpl.h>

#include "localise2dsim.h"

using namespace std;
using namespace orcawallfactory;

Localise2dSim::Localise2dSim( const std::string& tag, const orcaice::Context& context ) : 
    InterfaceSim(tag,context)
{
}
 
void 
Localise2dSim::run()
{
    orca::VehicleGeometryCuboidDescriptionPtr geom;
    geom->type = orca::VehicleGeometryCuboid;
    orcaice::setSane( geom->size );
    orcaice::setSane( geom->vehicleToGeometryTransform );
    
    orcaifaceimpl::Localise2dImplPtr iface;
    iface = new orcaifaceimpl::Localise2dImpl( geom, tag_, context_ );
    iface->initInterface( this );

    while ( !isStopping() )
    {
        try 
        {
            orca::Localise2dData data;
            orcaice::setSane( data );
            iface->localSetAndSend( data );

            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
            
        } // end of try
        catch ( Ice::CommunicatorDestroyedException & )
        {
            // This is OK: it means that the communicator shut down (eg via Ctrl-C)
            // somewhere in mainLoop. Eventually, component will tell us to stop.
        }
        catch ( const Ice::Exception &e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected exception: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( const std::exception &e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected exception: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( const std::string &e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected string: " << e;
            context_.tracer()->error( ss.str() );         
        }
        catch ( const char *e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected char *: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( ... )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected unknown exception.";
            context_.tracer()->error( ss.str() );
        }

        // If we got to here there's a problem.
        // Re-initialise the driver.
//         initialiseDriver();

    } // end of while

    // Laser hardware will be shut down in the driver's destructor.
    context_.tracer()->debug( "Localise2dSim::run(): Dropping out from run()", 2 );
};
