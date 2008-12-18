#include "logreaders.h"
#include <iostream>
#include <fstream>
#include <orcalog/orcalog.h>
#include <orcaobj/stringutils.h>
#include <orcaifacelog/orcaifacelog.h>  // catch-all for auto-generated (ascii) to/fromStream functions
#include <orcaifaceutil/vehicledescription.h>
#include "logstringutils.h"

using namespace std;

namespace orcalogfactory {

namespace {

    void checkFormats( const orcalog::LogReaderInfo   &logReaderInfo,
                       const std::vector<std::string> &okFormats )
    {
        for ( size_t i=0; i < okFormats.size(); i++ )
        {
            if ( logReaderInfo.format == okFormats[i] )
            {
                // format is supported
                return;
            }
        }

        // format is not supported: throw exception
        stringstream ss;
        ss << logReaderInfo.interfaceName<<": unknown log format: "<<logReaderInfo.format<<endl
           << "  Supported formats are: ";
        for ( size_t i=0; i < okFormats.size(); i++ )
        {
            ss << okFormats[i];
            if ( i != 0 ) ss << ", ";
        }
        throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
    }

    void
    checkFormatIceOnly( const orcalog::LogReaderInfo &logReaderInfo )
    {
        vector<string> okFormats;
        okFormats.push_back("ice");
        okFormats.push_back("asciigenerated"); // auto-generated -> always available; need to adjust function name
        checkFormats( logReaderInfo, okFormats );
    }

    void
    checkFormatIceOrAscii( const orcalog::LogReaderInfo &logReaderInfo )
    {
        vector<string> okFormats;
        okFormats.push_back("ice");
        okFormats.push_back("ascii");
        okFormats.push_back("asciigenerated");
        checkFormats( logReaderInfo, okFormats );
    }

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

}

//////////////////////////////////////////////////////////////////////

CameraLogReader::CameraLogReader( const orcalog::LogReaderInfo &logReaderInfo )
    : orcalog::LogReader( logReaderInfo )
{
    vector<string> okFormats;
    okFormats.push_back("ice");
    okFormats.push_back("jpeg");
    checkFormats( logReaderInfo, okFormats );
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

DriveBicycleLogReader::DriveBicycleLogReader( const orcalog::LogReaderInfo &logReaderInfo )
    : orcalog::LogReader( logReaderInfo )
{
    checkFormatIceOnly( logReaderInfo );
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
    checkFormatIceOrAscii( logReaderInfo );
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
    checkFormatIceOrAscii( logReaderInfo );
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

Localise2dLogReader::Localise2dLogReader( const orcalog::LogReaderInfo &logReaderInfo )
    : orcalog::LogReader( logReaderInfo )
{
    checkFormatIceOrAscii( logReaderInfo );
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
    checkFormatIceOnly( logReaderInfo );
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
    checkFormatIceOrAscii( logReaderInfo );
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
    checkFormatIceOrAscii( logReaderInfo );
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
    checkFormatIceOrAscii( logReaderInfo );
    orcalog::LogReader::zeroLogIndex();
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
    checkFormatIceOnly( logReaderInfo );
    orcalog::LogReader::zeroLogIndex();
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


}

