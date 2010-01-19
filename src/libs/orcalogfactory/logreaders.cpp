#include "logreaders.h"
#include <iostream>
#include <fstream>
#include <orcalog/orcalog.h>
#include <orcaifaceutil/vehicledescription.h>
#include "logstringutils.h"
#include <orcalog/formatcheckutil.h>

// alexm: I haven't checked which ones are actually used.
#include <orcaifacelog/datetime.h>
#include <orcaifacelog/exceptions.h>
#include <orcaifacelog/containers.h>
#include <orcaifacelog/common.h>
#include <orcaifacelog/ocm.h>
#include <orcaifacelog/bros1.h>
#include <orcaifacelog/stochastic.h>
#include <orcaifacelog/button.h>
#include <orcaifacelog/camera.h>
#include <orcaifacelog/cpu.h>
#include <orcaifacelog/drivebicycle.h>
#include <orcaifacelog/drivedifferential.h>
#include <orcaifacelog/estop.h>
#include <orcaifacelog/featuremap2d.h>
#include <orcaifacelog/gps.h>
#include <orcaifacelog/home.h>
#include <orcaifacelog/image.h>
#include <orcaifacelog/ins.h>
#include <orcaifacelog/imu.h>
#include <orcaifacelog/localise2d.h>
#include <orcaifacelog/localise3d.h>
#include <orcaifacelog/multicamera.h>
#include <orcaifacelog/rangerarray.h>
#include <orcaifacelog/odometry2d.h>
#include <orcaifacelog/odometry3d.h>
#include <orcaifacelog/ogfusion.h>
#include <orcaifacelog/ogmap.h>
#include <orcaifacelog/particle2d.h>
#include <orcaifacelog/pathfollower2d.h>
#include <orcaifacelog/pathplanner2d.h>
#include <orcaifacelog/pinger.h>
#include <orcaifacelog/pixmap.h>
#include <orcaifacelog/pointcloud.h>
#include <orcaifacelog/polarfeature2d.h>
#include <orcaifacelog/power.h>
#include <orcaifacelog/properties.h>
#include <orcaifacelog/qgraphics2d.h>
#include <orcaifacelog/rangescanner2d.h>
#include <orcaifacelog/laserscanner2d.h>
#include <orcaifacelog/status.h>
#include <orcaifacelog/systemstatus.h>
#include <orcaifacelog/tracer.h>
#include <orcaifacelog/vehicledescription.h>
#include <orcaifacelog/velocitycontrol2d.h>
#include <orcaifacelog/wifi.h>

using namespace std;

namespace orcalogfactory {

namespace {

    void
    readFromFile( std::ifstream                   *file, 
                  const std::string               &format,
                  orcaice::Context                 context,
                  orca::RangeScanner2dDescription &obj )
    {        
        if ( format=="ice" )
        {
            orcalog::IceReadHelper helper( context.communicator(), file );
            ice_readRangeScanner2dDescription( helper.stream_, obj );
            helper.read();
        }
        else if ( format=="asciigenerated" )
        {
            ifacelog::fromLogStream( obj, *file);
        }
        else if ( format=="ascii" )
        {   
            std::string line;
            std::getline( *file, line );
    
            std::stringstream ss( line );
            fromLogString( ss, obj );
        }    
        else
        {
            stringstream ss;
            ss <<  "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void
    readFromFile( std::ifstream            *file, 
                  const std::string        &format,
                  orcaice::Context          context,
                  orca::VehicleDescription &obj )
    {        
        if ( format=="ice" )
        {
            orcalog::IceReadHelper helper( context.communicator(), file );
            ice_readVehicleDescription( helper.stream_, obj );
            helper.read();
        }
        else if ( format=="asciigenerated" )
        {
            ifacelog::fromLogStream( obj, *file);
        }
        else if ( format=="ascii" )
        {   
            std::string line;
            std::getline( *file, line );
    
            std::stringstream ss( line );
            fromLogString( ss, obj );
        }    
        else
        {
            stringstream ss;
            ss <<  "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
        stringstream ss;
        ss << "TODO: re-enable printout of orca::VehicleDescription" << endl;
//        ss << "Read orca::VehicleDescription: " << ifaceutil::toString(obj);
        context.tracer().info( ss.str() );
    }

    void
    readFromFile( std::ifstream                       *file, 
                  const std::string                   &format,
                  orcaice::Context                     context,
                  orca::VehicleGeometryDescriptionPtr &obj )
    {        
        if ( format=="ice" )
        {
            orcalog::IceReadHelper helper( context.communicator(), file );
            ice_readVehicleGeometryDescription( helper.stream_, obj );
            helper.read();
        }
        else if ( format=="asciigenerated" )
        {
            ifacelog::fromLogStream( obj, *file);
        }
        else
        {
            stringstream ss;
            ss <<  "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void
    readFromFile( std::ifstream            *file, 
                  const std::string        &format,
                  orcaice::Context          context,
                  orca::GpsDescription &obj )
    {        
        if ( format=="ice" )
        {
            orcalog::IceReadHelper helper( context.communicator(), file );
            ice_readGpsDescription( helper.stream_, obj );
            helper.read();
        }
        else if ( format=="asciigenerated" )
        {
            ifacelog::fromLogStream( obj, *file);
        }
        else if ( format=="ascii" )
        {   
            std::string line;
            std::getline( *file, line );
    
            std::stringstream ss( line );
            fromLogString( ss, obj );
        }    
        else
        {
            stringstream ss;
            ss <<  "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void
    readFromFile( std::ifstream     *file, 
                  const std::string &format,
                  orcaice::Context   context,
                  orca::DriveBicycleData   &obj )
    {        
        if ( format=="ice" )
        {
            orcalog::IceReadHelper helper( context.communicator(), file );
            ice_readDriveBicycleData( helper.stream_, obj );
            helper.read();
        }
        else if ( format=="asciigenerated" )
        {
            ifacelog::fromLogStream( obj, *file);
        }
        else
        {
            stringstream ss;
            ss <<  "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void
    readFromFile( std::ifstream     *file, 
                  const std::string &format,
                  orcaice::Context   context,
                  orca::GpsData   &obj )
    {        
        if ( format=="ice" )
        {
            orcalog::IceReadHelper helper( context.communicator(), file );
            ice_readGpsData( helper.stream_, obj );
            helper.read();
        }
        else if ( format=="asciigenerated" )
        {
            ifacelog::fromLogStream( obj, *file);
        }
        else if ( format=="ascii" )
        {   
            std::string line;
            std::getline( *file, line );
    
            std::stringstream ss( line );
            fromLogString( ss, obj );
        }    
        else
        {
            stringstream ss;
            ss <<  "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void
    readFromFile( std::ifstream     *file, 
                  const std::string &format,
                  orcaice::Context   context,
                  orca::LaserScanner2dDataPtr   &obj )
    {        
        if ( format=="ice" )
        {
            orcalog::IceReadHelper helper( context.communicator(), file );
            ice_readLaserScanner2dData( helper.stream_, obj );
            helper.read();
        }
        else if ( format=="asciigenerated" )
        {
            ifacelog::fromLogStream( obj, *file);
        }
        else if ( format=="ascii" )
        {   
            obj = new orca::LaserScanner2dData;
            std::string line;
            std::getline( *file, line );

            std::stringstream ss( line );
            fromLogString( ss, *obj );
        }    
        else
        {
            stringstream ss;
            ss <<  "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void
    readFromFile( std::ifstream     *file, 
                  const std::string &format,
                  orcaice::Context   context,
                  orca::RangeScanner2dDataPtr   &obj )
    {        
        if ( format=="ice" )
        {
            orcalog::IceReadHelper helper( context.communicator(), file );
            ice_readRangeScanner2dData( helper.stream_, obj );
            helper.read();
        }
        else if ( format=="asciigenerated" )
        {
            ifacelog::fromLogStream( obj, *file);
        }
        else
        {
            stringstream ss;
            ss <<  "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void
    readFromFile( std::ifstream        *file, 
                  const std::string    &format,
                  orcaice::Context      context,
                  orca::Localise2dData &obj )
    {        
        if ( format=="ice" )
        {
            orcalog::IceReadHelper helper( context.communicator(), file );
            ice_readLocalise2dData( helper.stream_, obj );
            helper.read();
        }
        else if ( format=="asciigenerated" )
        {
            ifacelog::fromLogStream( obj, *file);
        }
        else if ( format=="ascii" )
        {   
            std::string line;
            std::getline( *file, line );

            std::stringstream ss( line );
            fromLogString( ss, obj );
        }    
        else
        {
            stringstream ss;
            ss <<  "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void
    readFromFile( std::ifstream        *file, 
                  const std::string    &format,
                  orcaice::Context      context,
                  orca::Localise3dData &obj )
    {        
        if ( format=="ice" )
        {
            orcalog::IceReadHelper helper( context.communicator(), file );
            ice_readLocalise3dData( helper.stream_, obj );
            helper.read();
        }
        else if ( format=="asciigenerated" )
        {
            ifacelog::fromLogStream( obj, *file);
        }
        else
        {
            stringstream ss;
            ss <<  "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void
    readFromFile( std::ifstream        *file, 
                  const std::string    &format,
                  orcaice::Context      context,
                  orca::Odometry2dData &obj )
    {        
        if ( format=="ice" )
        {
            orcalog::IceReadHelper helper( context.communicator(), file );
            ice_readOdometry2dData( helper.stream_, obj );
            helper.read();
        }
        else if ( format=="asciigenerated" )
        {
            ifacelog::fromLogStream( obj, *file);
        }
        else
        {
            stringstream ss;
            ss <<  "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void
    readFromFile( std::ifstream        *file, 
                  const std::string    &format,
                  orcaice::Context      context,
                  orca::Odometry3dData &obj )
    {        
        if ( format=="ice" )
        {
            orcalog::IceReadHelper helper( context.communicator(), file );
            ice_readOdometry3dData( helper.stream_, obj );
            helper.read();
        }
        else if ( format=="asciigenerated" )
        {
            ifacelog::fromLogStream( obj, *file);
        }
        else
        {
            stringstream ss;
            ss <<  "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void
    readFromFile( std::ifstream     *file, 
                  const std::string &format,
                  orcaice::Context   context,
                  orca::PowerData   &obj )
    {        
        if ( format=="ice" )
        {
            orcalog::IceReadHelper helper( context.communicator(), file );
            ice_readPowerData( helper.stream_, obj );
            helper.read();
        }
        else if ( format=="asciigenerated" )
        {
            ifacelog::fromLogStream( obj, *file);
        }
        else if ( format=="ascii" )
        {   
            std::string line;
            std::getline( *file, line );
    
            std::stringstream ss( line );
            fromLogString( ss, obj );
        }    
        else
        {
            stringstream ss;
            ss <<  "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void
    readFromFile( std::ifstream     *file, 
                  const std::string &format,
                  orcaice::Context   context,
                  orca::WifiData   &obj )
    {        
        if ( format=="ice" )
        {
            orcalog::IceReadHelper helper( context.communicator(), file );
            ice_readWifiData( helper.stream_, obj );
            helper.read();
        }
        else if ( format=="asciigenerated" )
        {
            ifacelog::fromLogStream( obj, *file);
        }
        else
        {
            stringstream ss;
            ss <<  "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void
    readFromFile( std::ifstream     *file, 
                  const std::string &format,
                  orcaice::Context   context,
                  orca::PathFollower2dData   &obj )
    {        
        if ( format=="ice" )
        {
            orcalog::IceReadHelper helper( context.communicator(), file );
            ice_readPathFollower2dData( helper.stream_, obj );
            helper.read();
        }
        else if ( format=="asciigenerated" )
        {
            ifacelog::fromLogStream( obj, *file);
        }
        else
        {
            stringstream ss;
            ss <<  "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

}

//////////////////////////////////////////////////////////////////////

CameraLogReader::CameraLogReader( const orcalog::LogReaderInfo &logReaderInfo )
    : orcalog::LogReader( logReaderInfo )
{
    vector<string> okFormats;
    okFormats.push_back("ice");
    okFormats.push_back("jpeg");
    orcalog::checkFormats( logReaderInfo, okFormats );
}
void
CameraLogReader::read( orca::ImageDataPtr &obj )
{
    cameraReader.readFromFile( file_, 
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::advanceLogIndex();
}
void
CameraLogReader::read( orca::CameraDescriptionPtr &obj )
{
    assert( orcalog::LogReader::logIndex() == -1 );
    cameraReader.readFromFile( file_, 
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::zeroLogIndex();
}

//
//alen - had to overwrite this method from logwriter.h to include jpegs
//
void
CameraLogReader::openLogFile()
{
    logReaderInfo_.context.tracer().debug( "Opening log file "+logReaderInfo_.filename,2 );

    if ( file_ ) {
        std::string s = "Log file already exists.";
        throw orcalog::FileException( ERROR_INFO, s );
    }

    //
    // open log file, may throw and it will kill us
    //
    if ( logReaderInfo_.format == "ice" ) {
        file_ = orcalog::openBinaryFileForReading( logReaderInfo_.filename );
    }
    else if ( logReaderInfo_.format == "jpeg" ) {
        file_ = orcalog::openAsciiFileForReading( logReaderInfo_.filename );
    }
    else {
        assert( false && "base class can only handle 'ice' and 'ascii' and 'asciigenerated' formats" );
    }
}

//////////////////////////////////////////////////////////////////////

MultiCameraLogReader::MultiCameraLogReader( const orcalog::LogReaderInfo &logReaderInfo )
    : orcalog::LogReader( logReaderInfo )
{
    vector<string> okFormats;
    okFormats.push_back("ice");
    okFormats.push_back("jpeg");
    okFormats.push_back("bmp");
    okFormats.push_back("avi");
    orcalog::checkFormats( logReaderInfo, okFormats );
}
void
MultiCameraLogReader::read( orca::MultiCameraDataPtr &obj )
{
    multiCameraReader.readFromFile( file_,
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::advanceLogIndex();
}
void
MultiCameraLogReader::read( orca::MultiCameraDescriptionPtr &obj )
{
    assert( orcalog::LogReader::logIndex() == -1 );
    multiCameraReader.readFromFile( file_,
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::zeroLogIndex();
}

//
//alen - had to overwrite this method from logwriter.h to include jpegs
//
void
MultiCameraLogReader::openLogFile()
{
    logReaderInfo_.context.tracer().debug( "Opening log file "+logReaderInfo_.filename,2 );

    if ( file_ ) {
        std::string s = "Log file already exists.";
        throw orcalog::FileException( ERROR_INFO, s );
    }

    //
    // open log file, may throw and it will kill us
    //
    if ( logReaderInfo_.format == "ice" ) {
        file_ = orcalog::openBinaryFileForReading( logReaderInfo_.filename );
    }
    else if ( logReaderInfo_.format == "jpeg" ||  logReaderInfo_.format == "bmp"  ||  logReaderInfo_.format == "avi") {
        file_ = orcalog::openAsciiFileForReading( logReaderInfo_.filename );
    }
    else {
        assert( false && "base class can only handle 'ice', 'jpeg', 'bmp' or 'avi' formats" );
    }
}


//////////////////////////////////////////////////////////////////////

DriveBicycleLogReader::DriveBicycleLogReader( const orcalog::LogReaderInfo &logReaderInfo )
    : orcalog::LogReader( logReaderInfo )
{
    orcalog::checkFormatDefaultsOnly( logReaderInfo );
}
void
DriveBicycleLogReader::read( orca::DriveBicycleData &obj )
{
    readFromFile( file_, 
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::advanceLogIndex();
}
void
DriveBicycleLogReader::read( orca::VehicleDescription &obj )
{
    assert( orcalog::LogReader::logIndex() == -1 );
    readFromFile( file_, 
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::zeroLogIndex();
}

//////////////////////////////////////////////////////////////////////

GpsLogReader::GpsLogReader( const orcalog::LogReaderInfo &logReaderInfo )
    : orcalog::LogReader( logReaderInfo )
{
    orcalog::checkFormatDefaultsOrAscii( logReaderInfo );
}
void
GpsLogReader::read( orca::GpsData &obj )
{
    readFromFile( file_, 
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::advanceLogIndex();
}
void
GpsLogReader::read( orca::GpsDescription &obj )
{
    assert( orcalog::LogReader::logIndex() == -1 );
    readFromFile( file_, 
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::zeroLogIndex();
}

//////////////////////////////////////////////////////////////////////

LaserScanner2dLogReader::LaserScanner2dLogReader( const orcalog::LogReaderInfo &logReaderInfo )
    : orcalog::LogReader( logReaderInfo )
{
    orcalog::checkFormatDefaultsOrAscii( logReaderInfo );
}
void
LaserScanner2dLogReader::read( orca::LaserScanner2dDataPtr &obj )
{
    readFromFile( file_, 
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::advanceLogIndex();
}
void
LaserScanner2dLogReader::read( orca::RangeScanner2dDescription &obj )
{
    assert( orcalog::LogReader::logIndex() == -1 );
    readFromFile( file_, 
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::zeroLogIndex();
}

//////////////////////////////////////////////////////////////////////

RangeScanner2dLogReader::RangeScanner2dLogReader( const orcalog::LogReaderInfo &logReaderInfo )
    : orcalog::LogReader( logReaderInfo )
{
    orcalog::checkFormatDefaultsOnly( logReaderInfo );
}
void
RangeScanner2dLogReader::read( orca::RangeScanner2dDataPtr &obj )
{
    readFromFile( file_, 
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::advanceLogIndex();
}
void
RangeScanner2dLogReader::read( orca::RangeScanner2dDescription &obj )
{
    assert( orcalog::LogReader::logIndex() == -1 );
    readFromFile( file_, 
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::zeroLogIndex();
}

//////////////////////////////////////////////////////////////////////

Localise2dLogReader::Localise2dLogReader( const orcalog::LogReaderInfo &logReaderInfo )
    : orcalog::LogReader( logReaderInfo )
{
    orcalog::checkFormatDefaultsOrAscii( logReaderInfo );
}
void
Localise2dLogReader::read( orca::Localise2dData &obj )
{
    readFromFile( file_, 
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::advanceLogIndex();
}
void
Localise2dLogReader::read( orca::VehicleGeometryDescriptionPtr &obj )
{
    assert( orcalog::LogReader::logIndex() == -1 );
    readFromFile( file_, 
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::zeroLogIndex();
}

//////////////////////////////////////////////////////////////////////

Localise3dLogReader::Localise3dLogReader( const orcalog::LogReaderInfo &logReaderInfo )
    : orcalog::LogReader( logReaderInfo )
{
    orcalog::checkFormatDefaultsOnly( logReaderInfo );
}
void
Localise3dLogReader::read( orca::Localise3dData &obj )
{
    readFromFile( file_, 
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::advanceLogIndex();
}
void
Localise3dLogReader::read( orca::VehicleGeometryDescriptionPtr &obj )
{
    assert( orcalog::LogReader::logIndex() == -1 );
    readFromFile( file_, 
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::zeroLogIndex();
}

//////////////////////////////////////////////////////////////////////

Odometry2dLogReader::Odometry2dLogReader( const orcalog::LogReaderInfo &logReaderInfo )
    : orcalog::LogReader( logReaderInfo )
{
    orcalog::checkFormatDefaultsOnly( logReaderInfo );
}
void
Odometry2dLogReader::read( orca::Odometry2dData &obj )
{
    readFromFile( file_, 
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::advanceLogIndex();
}
void
Odometry2dLogReader::read( orca::VehicleDescription &obj )
{
    assert( orcalog::LogReader::logIndex() == -1 );
    readFromFile( file_, 
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::zeroLogIndex();
}

//////////////////////////////////////////////////////////////////////

Odometry3dLogReader::Odometry3dLogReader( const orcalog::LogReaderInfo &logReaderInfo )
    : orcalog::LogReader( logReaderInfo )
{
    orcalog::checkFormatDefaultsOnly( logReaderInfo );
}
void
Odometry3dLogReader::read( orca::Odometry3dData &obj )
{
    readFromFile( file_, 
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::advanceLogIndex();
}
void
Odometry3dLogReader::read( orca::VehicleDescription &obj )
{
    assert( orcalog::LogReader::logIndex() == -1 );
    readFromFile( file_, 
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::zeroLogIndex();
}

//////////////////////////////////////////////////////////////////////

PowerLogReader::PowerLogReader( const orcalog::LogReaderInfo &logReaderInfo )
    : orcalog::LogReader( logReaderInfo )
{
    orcalog::checkFormatDefaultsOrAscii( logReaderInfo );
}
void
PowerLogReader::read( orca::PowerData &obj )
{
    readFromFile( file_, 
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::advanceLogIndex();
}

//////////////////////////////////////////////////////////////////////

WifiLogReader::WifiLogReader( const orcalog::LogReaderInfo &logReaderInfo )
    : orcalog::LogReader( logReaderInfo )
{
    orcalog::checkFormatDefaultsOnly( logReaderInfo );
}
void
WifiLogReader::read( orca::WifiData &obj )
{
    readFromFile( file_, 
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::advanceLogIndex();
}

//////////////////////////////////////////////////////////////////////

PathFollower2dLogReader::PathFollower2dLogReader( const orcalog::LogReaderInfo &logReaderInfo )
    : orcalog::LogReader( logReaderInfo )
{
    orcalog::checkFormatDefaultsOnly( logReaderInfo );
}
void
PathFollower2dLogReader::read( orca::PathFollower2dData &obj )
{
    readFromFile( file_, 
                  orcalog::LogReader::logReaderInfo().format,
                  orcalog::LogReader::logReaderInfo().context,
                  obj );
    orcalog::LogReader::advanceLogIndex();
}


}

