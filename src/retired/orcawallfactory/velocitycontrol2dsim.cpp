/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include <orcaobjutil/vehicleutil.h>
#include <orcaifaceimpl/velocitycontrol2dImpl.h>

#include "velocitycontrol2dsim.h"

using namespace std;
using namespace orcawallfactory;


class PrivateNotifyHandler : public gbxiceutilacfr::NotifyHandler<orca::VelocityControl2dData>
{
public:
    PrivateNotifyHandler( gbxutilacfr::Tracer& tracer ) :
        tracer_(tracer) {};

    // from gbxiceutilacfr::NotifyHandler
    virtual void handleData( const orca::VelocityControl2dData& data )
    {
        tracer_.debug( "VelocityControl2dSim::handleData: received "+orcaobj::toString(data), 6 );
    }
private:
    gbxutilacfr::Tracer& tracer_;
};

VelocityControl2dSim::VelocityControl2dSim( const std::string& tag, const orcaice::Context& context ) : 
    InterfaceSim(tag,context)
{
}
 
void 
VelocityControl2dSim::walk()
{
    orca::VehicleDescription descr;
    orcaobjutil::readVehicleDescription( context_.properties(), configPrefix(), descr );

    orcaifaceimpl::VelocityControl2dImplPtr iface;
    iface = new orcaifaceimpl::VelocityControl2dImpl( descr, tag_, context_ );
    iface->initInterface( this );

    // create notify handler and hook him up to the notify guy.
    PrivateNotifyHandler notifyHandler( context_.tracer() );
    iface->setNotifyHandler( &notifyHandler );

    while ( !isStopping() )
    {
        try 
        {
            // unlike DriveBicycle, this interface currently does not serve reference command
//             orca::VelocityControl2dData data;
//             orcaobj::setSane( data );
//             iface->localSetAndSend( data );

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
            context_.tracer().error( ss.str() );
        }
        catch ( const std::exception &e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected exception: " << e.what();
            context_.tracer().error( ss.str() );
        }
        catch ( ... )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected unknown exception.";
            context_.tracer().error( ss.str() );
        }

    } // end of while
};
