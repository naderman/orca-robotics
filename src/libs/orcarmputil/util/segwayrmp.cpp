#include "segwayrmp.h"
#include <iostream>
#include <segwayrmpacfr/driver.h>
#include <segwayrmpfake/driver.h>
#include <hydroutil/stringutils.h>

using namespace std;

namespace segwayrmp {

SegwayRmp::SegwayRmp( hydrointerfaces::SegwayRmp::Capabilities &capabilitiesConstraints,
                      double                                    maxForwardAcceleration,
                      double                                    maxReverseAcceleration,
                      orcarmputil::AggregatorCallback          &callback,
                      bool                                      fakeDriver,
                      const hydroutil::Context                 &context )
    : context_(context)
{
    const hydroutil::Properties &prop = context.properties();

    std::vector<std::string> powerbaseNames = 
        hydroutil::toStringSeq( prop.getPropertyWithDefault( "PowerbaseNames", "" ), ' ' );
    if ( powerbaseNames.size() == 0 )
        powerbaseNames.push_back( "Main" );

    for ( size_t i=0; i < powerbaseNames.size(); i++ )
    {
        hydroutil::Properties propertiesThisPowerbase( prop.data(), powerbaseNames[i]+".", prop.config() );
        hydroutil::Context contextThisPowerbase( propertiesThisPowerbase,
                                                 context.tracer(),
                                                 context.status() );
        std::auto_ptr<hydrointerfaces::SegwayRmp> hydroDriver;
        if ( fakeDriver )
        {
            hydroDriver.reset( new segwayrmpfake::Driver( powerbaseNames[i], contextThisPowerbase ) );
        }
        else
        {
            hydroDriver.reset( new segwayrmpacfr::Driver( powerbaseNames[i], contextThisPowerbase ) );
        }
        powerbaseManagers_.push_back( orcarmputil::PowerbaseManagerPtr( 
                                          new orcarmputil::PowerbaseManager( powerbaseNames[i],
                                                                             i,
                                                                             callback,
                                                                             hydroDriver,
                                                                             contextThisPowerbase ) ) );
    }

    //
    // Set constraints
    //
    capabilities_ = powerbaseManagers_.back()->capabilities();
    if ( !capabilitiesConstraints.isSane() )
    {
        stringstream ss;
        ss << "Bad capabilitiesConstraints: " << capabilitiesConstraints;
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
    hydrointerfaces::constrain( capabilities_, capabilitiesConstraints );

    //
    // Initialise the powerbase managers
    //
    orcarmputil::DriverThread::Config cfg;
    cfg.driveInReverse = (bool)prop.getPropertyAsIntWithDefault( "DriveInReverse", 0 );
    cfg.isMotionEnabled = (bool)prop.getPropertyAsIntWithDefault( "EnableMotion", 0 );
    cfg.maxForwardAcceleration = maxForwardAcceleration;
    cfg.maxReverseAcceleration = maxReverseAcceleration;
//     std::string stallPrefix = "StallSensor.";
//     cfg.stallSensorConfig.torqueThreshold = prop.getPropertyAsDoubleWithDefault( stallPrefix+"TorqueThreshold", 3.0 );
//     cfg.stallSensorConfig.speedThreshold = prop.getPropertyAsDoubleWithDefault( stallPrefix+"speedThreshold", 0.5 );
//     cfg.stallSensorConfig.timeThreshold = prop.getPropertyAsDoubleWithDefault( stallPrefix+"TimeThreshold", 0.5 );
    // Forget about sensing stalls
    cfg.stallSensorConfig.torqueThreshold = 99;
    cfg.stallSensorConfig.speedThreshold  = 99;
    cfg.stallSensorConfig.timeThreshold   = 99;
    
    for ( size_t i=0; i < powerbaseManagers_.size(); i++ )
    {
        try {
            powerbaseManagers_[i]->init( cfg );
        }
        catch ( std::exception &e )
        {
            stringstream ss;
            ss << "Failed to init powerbase '"<<powerbaseManagers_[i]->name()<<": " << e.what();
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
    }

    //
    // Finally, start the powerbase threads rolling
    //
    for ( size_t i=0; i < powerbaseManagers_.size(); i++ )
    {
        powerbaseManagers_[i]->startThread();
    }
}

void
SegwayRmp::setCommand( const hydrointerfaces::SegwayRmp::Command &incomingCmd )
{
    if ( context_.tracer().verbosity( gbxutilacfr::DebugTrace ) >= 5 )
    {
        stringstream ss;
        ss << __func__ << "(" << incomingCmd << ")";
        context_.tracer().debug( "SegwayRmp", ss.str(), 5 );
    }

    hydrointerfaces::SegwayRmp::Command cmd = incomingCmd;
    if ( !hydrointerfaces::commandPossible( cmd,
                                            powerbaseManagers_.back()->capabilities() ) )
    {
        hydrointerfaces::limit( cmd,
                                powerbaseManagers_.back()->capabilities() );

        stringstream ss;
        ss << "Requested command ("<<incomingCmd<<") can not be achieved.  " << endl
           << "Capabilities: " << powerbaseManagers_.back()->capabilities().toString() << endl
           << "    --> limiting command to: " << cmd.toString();
        cout << ss.str() << endl;
    }

    for ( size_t i=0; i < powerbaseManagers_.size(); i++ )
    {
        powerbaseManagers_[i]->setDesiredSpeed( cmd );    
    }
}

}

