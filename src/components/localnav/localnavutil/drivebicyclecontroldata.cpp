/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include <localnavutil/drivebicyclecontroldata.h>
#include <localnavutil/drivebicyclestatedata.h>
    
namespace localnav {

void 
DriveBicycleControlData::setStopCommand( IStateData* currentCmd ) 
{ 
    // do dynamic cast to ensure it is us
    const DriveBicycleStateData *data = dynamic_cast<DriveBicycleStateData*>( const_cast<IStateData*>(currentCmd) );
    if( data != NULL )
    {
        this->bicycleData_.speed = 0.0;
        this->bicycleData_.steerAngle = data->driveBicycleData().steerAngle;
    }
    else
    {
        assert( false );
        throw orca::DataNotExistException( "Error, we were passed a current state of NULL, which we need for safety!" );
    }
}


} // namespace mpcucb
