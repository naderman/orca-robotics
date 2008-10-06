#include "logreaders.h"
#include <iostream>
#include <fstream>
#include <orcalog/orcalog.h>
#include <orcaobj/stringutils.h>
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
        checkFormats( logReaderInfo, okFormats );
    }

    void
    checkFormatIceOrAscii( const orcalog::LogReaderInfo &logReaderInfo )
    {
        vector<string> okFormats;
        okFormats.push_back("ice");
        okFormats.push_back("ascii");
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
        else
        {
            stringstream ss;
            ss <<  "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
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

