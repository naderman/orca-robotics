/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <cstdlib>
#include <hydroiceutil/localtracer.h>
#include <hydroiceutil/localstatus.h>
#include <gbxutilacfr/exceptions.h>

using namespace std;

int main(int argc, char * argv[])
{
    hydroiceutil::LocalTracer tracer;
    hydroutil::Properties props;

    hydroiceutil::LocalStatus status( tracer, props );

    cout<<"setting infrastructure to Initialising ...";
    status.infrastructureInitialising();
    if ( status.infrastructureState() != gbxutilacfr::SubsystemInitialising ) {
        cout<<"infrastructure is not in Initialising state: "<<gbxutilacfr::toString(status.infrastructureState())<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"setting infrastructure to Working ...";
    status.infrastructureWorking();
    if ( status.infrastructureState() != gbxutilacfr::SubsystemWorking ) {
        cout<<"infrastructure is not in Working state: "<<gbxutilacfr::toString(status.infrastructureState())<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"setting infrastructure to Finalising ...";
    status.infrastructureFinalising();
    if ( status.infrastructureState() != gbxutilacfr::SubsystemFinalising ) {
        cout<<"infrastructure is not in Finalising state: "<<gbxutilacfr::toString(status.infrastructureState())<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"setMaxHeartbeatInterval on non-existent subsystems ...";
    try {
        status.setMaxHeartbeatInterval( "core", 10 );
    }
    catch ( const gbxutilacfr::Exception& e ) {
        // ok
    }
    cout<<"ok"<<endl;

    cout<<"adding subsystem ...";
    status.addSubsystem( "core", 10 );
    
    vector<string> names = status.subsystems();
    if ( names.size()!=1 || names[0]!="core" ) {
        cout<<"subsystem names do not much expectations"<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"getting subsystem status ...";
    try {
        gbxutilacfr::SubsystemStatus substatus;
        substatus = status.subsystemStatus("core");
        cout<<"state="<<gbxutilacfr::toString(substatus.state)<<" health="<<gbxutilacfr::toString(substatus.health)<<" ... "<<endl;
    }
    catch ( const gbxutilacfr::Exception& e ) {
        cout<<"cannot retrieve subsystem status."<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"checking initial status ...";
    if ( status.subsystemStatus("core").state != gbxutilacfr::SubsystemIdle ) {
        cout<<"subsystem initialised with wrong state: "<<gbxutilacfr::toString(status.subsystemStatus("core").state)<<endl;
        return EXIT_FAILURE;
    }
    if ( status.subsystemStatus("core").health != gbxutilacfr::SubsystemOk ) {
        cout<<"subsystem initialised with wrong health: "<<gbxutilacfr::toString(status.subsystemStatus("core").state)<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    status.setMaxHeartbeatInterval( "core", 20 );
    status.initialising( "core", "holding fingers crossed" );

    cout<<"checking new status ...";
    if ( status.subsystemStatus("core").state != gbxutilacfr::SubsystemInitialising ) {
        cout<<"subsystem is not in Initialising state: "<<gbxutilacfr::toString(status.subsystemStatus("core").state)<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;
    

    // this one should not appear
    status.initialising( "core", "holding fingers crossed" );
    status.initialising( "core", "still holding fingers crossed" );
    status.heartbeat( "core" );

    cout<<"checking same status ...";
    if ( status.subsystemStatus("core").state != gbxutilacfr::SubsystemInitialising ) {
        cout<<"subsystem is not in Initialising state: "<<gbxutilacfr::toString(status.subsystemStatus("core").state)<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    status.warning( "core", "all is weird" );
    status.warning( "core", "all is weird" );
    status.warning( "core", "still all is weird" );

    cout<<"checking new health ...";
    if ( status.subsystemStatus("core").health != gbxutilacfr::SubsystemWarning ) {
        cout<<"subsystem is not in Warning health: "<<gbxutilacfr::toString(status.subsystemStatus("core").health)<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    status.fault( "core", "all is bad" );
    status.fault( "core", "all is bad" );
    status.fault( "core", "still all is bad" );
    cout<<"checking new health ...";
    if ( status.subsystemStatus("core").health != gbxutilacfr::SubsystemFault ) {
        cout<<"subsystem is not in Fault health: "<<gbxutilacfr::toString(status.subsystemStatus("core").health)<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    status.ok( "core", "all is good again" );
    status.ok( "core" );
    // this one will not get traced!
    status.ok( "core" );

    status.working( "core" );
    cout<<"checking new state ...";
    if ( status.subsystemStatus("core").state != gbxutilacfr::SubsystemWorking) {
        cout<<"subsystem is not in Working state: "<<gbxutilacfr::toString(status.subsystemStatus("core").state)<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    status.finalising( "core" );
    cout<<"checking new state ...";
    if ( status.subsystemStatus("core").state != gbxutilacfr::SubsystemFinalising) {
        cout<<"subsystem is not in Finalising state: "<<gbxutilacfr::toString(status.subsystemStatus("core").state)<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    // should delete and start complaining again
    cout<<"removing existing subsystem ...";
    status.removeSubsystem( "core" );
    cout<<"ok"<<endl;

    cout<<"removing non-existing subsystem ...";
    try {
        status.removeSubsystem( "core" );
        cout<<"expect an exception"<<endl;
        return EXIT_FAILURE;
    }
    catch ( const gbxutilacfr::Exception& e ) {
        // ok
    }

    try {
        status.heartbeat( "core" );
        cout<<"expect an exception"<<endl;
        return EXIT_FAILURE;
    }
    catch ( const gbxutilacfr::Exception& e ) {
        // ok
    }

    return EXIT_SUCCESS;
}
