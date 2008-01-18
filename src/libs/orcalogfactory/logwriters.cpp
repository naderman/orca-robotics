#include "logwriters.h"
#include <iostream>
#include <fstream>
#include <orcalog/orcalog.h>
#include <orcaobj/stringutils.h>
#include "logstringutils.h"

using namespace std;

namespace orcalogfactory {

namespace {

    void checkFormats( const orcalog::LogWriterInfo  &logWriterInfo,
                       const std::vector<std::string> &okFormats )
    {
        for ( uint i=0; i < okFormats.size(); i++ )
        {
            if ( logWriterInfo.format == okFormats[i] )
            {
                // format is supported
                return;
            }
        }

        // format is not supported: throw exception
        stringstream ss;
        ss << logWriterInfo.comment<<": unknown log format: "<<logWriterInfo.format<<endl
           << "  Supported formats are: ";
        for ( uint i=0; i < okFormats.size(); i++ )
        {
            ss << okFormats[i];
            if ( i != 0 ) ss << ", ";
        }
        throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
    }

    void
    checkFormatIceOnly( const orcalog::LogWriterInfo &logWriterInfo )
    {
        vector<string> okFormats;
        okFormats.push_back("ice");
        checkFormats( logWriterInfo, okFormats );
    }

    void
    checkFormatIceOrAscii( const orcalog::LogWriterInfo &logWriterInfo )
    {
        vector<string> okFormats;
        okFormats.push_back("ice");
        okFormats.push_back("ascii");
        checkFormats( logWriterInfo, okFormats );
    }

    void 
    logToFile( std::ofstream                  *file, 
               const std::string              &format,
               orcaice::Context                context,
               const orca::VehicleDescription &obj )
    {
        if ( format == "ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writeVehicleDescription( helper.stream_, obj );
            helper.write( file ); 
        }
        else if ( format == "ascii" )
        {
            for ( uint i=0; i < 20; i++ )
                cout<<"TRACE(logwriters.cpp): TODO: log orca::VehicleDescription" << endl;
        }
        else
        {
            stringstream ss;
            ss << "Can't handle format " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    } 

    void
    logToFile( std::ofstream                              *file, 
               const std::string                          &format,
               orcaice::Context                            context,
               const orca::VehicleGeometryDescriptionPtr  &obj )
    {
        if ( format == "ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writeVehicleGeometryDescription( helper.stream_, obj );
            helper.write( file );
        }
        else if ( format == "ascii" )
        {
            (*file) << toLogString(obj) << flush;
        }
        else
        {
            stringstream ss;
            ss << "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void logToFile( std::ofstream       *file, 
                    const std::string   &format,
                    orcaice::Context     context,
                    const orca::RangeScanner2dDescription &obj )
    {
        if ( format == "ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writeRangeScanner2dDescription( helper.stream_, obj );
            helper.write( file );  
        }
        else if ( format == "ascii" )
        {
            (*file) << orcaice::toString(obj) << endl;
        }
        else
        {
            stringstream ss;
            ss << "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void logToFile( std::ofstream       *file, 
                    const std::string   &format,
                    orcaice::Context     context,
                    const orca::GpsDescription &obj )
    {
        if ( format=="ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writeGpsDescription( helper.stream_, obj );
            helper.write( file );  
        }
        else if ( format=="ascii" )
        {
            (*file) << toLogString(obj) << endl;
        }
        else
        {
            stringstream ss;
            ss << "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void logToFile( std::ofstream       *file, 
                    const std::string   &format,
                    orcaice::Context     context,
                    const orca::CpuData &obj )
    {        
        if ( format == "ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writeCpuData( helper.stream_, obj );
            helper.write( file );
        }
        else if ( format == "ascii" )
        {
            (*file) << toLogString(obj) << endl;
        }
        else
        {
            stringstream ss;
            ss << "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void logToFile( std::ofstream       *file, 
                    const std::string   &format,
                    orcaice::Context     context,
                    const orca::DriveBicycleData &obj )
    {        
        if ( format == "ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writeDriveBicycleData( helper.stream_, obj );
            helper.write( file );
        }
        else if ( format == "ascii" )
        {
            (*file) << toLogString(obj) << endl;
        }
        else
        {
            stringstream ss;
            ss << "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void logToFile( std::ofstream       *file, 
                    const std::string   &format,
                    orcaice::Context     context,
                    const orca::ImuData &obj )
    {        
        if ( format == "ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writeImuData( helper.stream_, obj );
            helper.write( file );
        }
        else if ( format == "ascii" )
        {
            (*file) << toLogString(obj) << endl;
        }
        else
        {
            stringstream ss;
            ss << "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void logToFile( std::ofstream       *file, 
                    const std::string   &format,
                    orcaice::Context     context,
                    const orca::LaserScanner2dDataPtr &obj )
    {        
        if ( format == "ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writeLaserScanner2dData( helper.stream_, obj );
            helper.write( file );
        }
        else if ( format == "ascii" )
        {
            (*file) << toLogString(obj) << endl;
        }
        else
        {
            stringstream ss;
            ss << "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void logToFile( std::ofstream       *file, 
                    const std::string   &format,
                    orcaice::Context     context,
                    const orca::Localise2dData &obj )
    {        
        if ( format == "ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writeLocalise2dData( helper.stream_, obj );
            helper.write( file );
        }
        else if ( format == "ascii" )
        {
            (*file) << toLogString(obj) << endl;
        }
        else
        {
            stringstream ss;
            ss << "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void logToFile( std::ofstream       *file, 
                    const std::string   &format,
                    orcaice::Context     context,
                    const orca::Localise3dData &obj )
    {        
        if ( format == "ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writeLocalise3dData( helper.stream_, obj );
            helper.write( file );
        }
        else if ( format == "ascii" )
        {
            (*file) << toLogString(obj) << endl;
        }
        else
        {
            stringstream ss;
            ss << "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void logToFile( std::ofstream                              *file, 
                    const std::string                          &format,
                    orcaice::Context                            context,
                    const orca::Odometry2dData                 &obj )
    {        
        if ( format == "ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writeOdometry2dData( helper.stream_, obj );
            helper.write( file );
        }
        else if ( format == "ascii" )
        {
            (*file) << toLogString(obj) << endl;
        }
        else
        {
            stringstream ss;
            ss << "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void logToFile( std::ofstream              *file, 
                    const std::string          &format,
                    orcaice::Context            context,
                    const orca::Odometry3dData &obj )
    {        
        if ( format == "ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writeOdometry3dData( helper.stream_, obj );
            helper.write( file );
        }
        else if ( format == "ascii" )
        {
            (*file) << toLogString(obj) << endl;
        }
        else
        {
            stringstream ss;
            ss << "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void logToFile( std::ofstream                  *file, 
                    const std::string              &format,
                    orcaice::Context                context,
                    const orca::PolarFeature2dData &obj )
    {        
        if ( format == "ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writePolarFeature2dData( helper.stream_, obj );
            helper.write( file );
        }
        else
        {
            stringstream ss;
            ss << "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void logToFile( std::ofstream         *file, 
                    const std::string     &format,
                    orcaice::Context       context,
                    const orca::PowerData &obj )
    {        
        if ( format == "ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writePowerData( helper.stream_, obj );
            helper.write( file );
        }
        else if ( format == "ascii" )
        {
            (*file) << toLogString(obj) << endl;
        }
        else
        {
            stringstream ss;
            ss << "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void logToFile( std::ofstream                              *file, 
                    const std::string                          &format,
                    orcaice::Context                            context,
                    const orca::WifiData                 &obj )
    {        
        if ( format == "ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writeWifiData( helper.stream_, obj );
            helper.write( file );
        }
        else if ( format == "ascii" )
        {
            (*file) << toLogString(obj) << endl;
        }
        else
        {
            stringstream ss;
            ss << "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

    void logToFile( std::ofstream                              *file, 
                    const std::string                          &format,
                    orcaice::Context                            context,
                    const orca::GpsData                 &obj )
    {        
        if ( format == "ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writeGpsData( helper.stream_, obj );
            helper.write( file );
        }
        else if ( format == "ascii" )
        {
            (*file) << toLogString(obj) << endl;
        }
        else
        {
            stringstream ss;
            ss << "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
    }

}

//////////////////////////////////////////////////////////////////////

CpuLogWriter::CpuLogWriter( const orcalog::LogWriterInfo &logWriterInfo )
    : orcalog::LogWriter( logWriterInfo )
{
    checkFormatIceOrAscii( logWriterInfo );
}
void 
CpuLogWriter::write( const orca::CpuData &obj )
{
    writeReferenceToMasterFile();
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               obj );
}

//////////////////////////////////////////////////////////////////////

DriveBicycleLogWriter::DriveBicycleLogWriter( const orcalog::LogWriterInfo   &logWriterInfo )
    : orcalog::LogWriter( logWriterInfo )
{
    checkFormatIceOrAscii( logWriterInfo );
}
void 
DriveBicycleLogWriter::write( const orca::DriveBicycleData &obj )
{
    writeReferenceToMasterFile();
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               obj );
}
void 
DriveBicycleLogWriter::write( const orca::VehicleDescription &descr )
{
    if ( orcalog::LogWriter::numItemsLogged() > 0 )
        throw orcalog::Exception( ERROR_INFO, "Tried to write description after logging started" );
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               descr );
}

//////////////////////////////////////////////////////////////////////

ImuLogWriter::ImuLogWriter( const orcalog::LogWriterInfo      &logWriterInfo )
    : orcalog::LogWriter( logWriterInfo )
{
    checkFormatIceOrAscii( logWriterInfo );
}
void 
ImuLogWriter::write( const orca::ImuData &obj )
{
    writeReferenceToMasterFile();
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               obj );
}

//////////////////////////////////////////////////////////////////////

LaserScanner2dLogWriter::LaserScanner2dLogWriter( const orcalog::LogWriterInfo      &logWriterInfo )
                                                  
    : orcalog::LogWriter( logWriterInfo )
{
    checkFormatIceOrAscii( logWriterInfo );
}
void 
LaserScanner2dLogWriter::write( const orca::LaserScanner2dDataPtr &obj )
{
    writeReferenceToMasterFile();
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               obj );
}
void 
LaserScanner2dLogWriter::write( const orca::RangeScanner2dDescription &descr )
{
    if ( orcalog::LogWriter::numItemsLogged() > 0 )
        throw orcalog::Exception( ERROR_INFO, "Tried to write description after logging started" );
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               descr );
}

//////////////////////////////////////////////////////////////////////

Localise2dLogWriter::Localise2dLogWriter( const orcalog::LogWriterInfo      &logWriterInfo )
    : orcalog::LogWriter( logWriterInfo )
{
    checkFormatIceOrAscii( logWriterInfo );
}
void 
Localise2dLogWriter::write( const orca::Localise2dData &obj )
{
    writeReferenceToMasterFile();
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               obj );
}
void 
Localise2dLogWriter::write( const orca::VehicleGeometryDescriptionPtr &descr )
{
    if ( orcalog::LogWriter::numItemsLogged() > 0 )
        throw orcalog::Exception( ERROR_INFO, "Tried to write description after logging started" );
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               descr );
}

//////////////////////////////////////////////////////////////////////

Localise3dLogWriter::Localise3dLogWriter( const orcalog::LogWriterInfo      &logWriterInfo )
    : orcalog::LogWriter( logWriterInfo )
{
    checkFormatIceOrAscii( logWriterInfo );
}
void 
Localise3dLogWriter::write( const orca::Localise3dData &obj )
{
    writeReferenceToMasterFile();
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               obj );
}
void 
Localise3dLogWriter::write( const orca::VehicleGeometryDescriptionPtr &descr )
{
    if ( orcalog::LogWriter::numItemsLogged() > 0 )
        throw orcalog::Exception( ERROR_INFO, "Tried to write description after logging started" );
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               descr );
}

//////////////////////////////////////////////////////////////////////

Odometry2dLogWriter::Odometry2dLogWriter( const orcalog::LogWriterInfo &logWriterInfo )
    : orcalog::LogWriter( logWriterInfo )
{
    checkFormatIceOrAscii( logWriterInfo );
}
void 
Odometry2dLogWriter::write( const orca::Odometry2dData &obj )
{
    writeReferenceToMasterFile();
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               obj );
}
void 
Odometry2dLogWriter::write( const orca::VehicleDescription &descr )
{
    if ( orcalog::LogWriter::numItemsLogged() > 0 )
        throw orcalog::Exception( ERROR_INFO, "Tried to write description after logging started" );
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               descr );
}

//////////////////////////////////////////////////////////////////////

Odometry3dLogWriter::Odometry3dLogWriter( const orcalog::LogWriterInfo &logWriterInfo )
    : orcalog::LogWriter( logWriterInfo )
{
    checkFormatIceOrAscii( logWriterInfo );
}
void 
Odometry3dLogWriter::write( const orca::Odometry3dData &obj )
{
    writeReferenceToMasterFile();
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               obj );
}
void 
Odometry3dLogWriter::write( const orca::VehicleDescription &descr )
{
    if ( orcalog::LogWriter::numItemsLogged() > 0 )
        throw orcalog::Exception( ERROR_INFO, "Tried to write description after logging started" );
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               descr );
}

//////////////////////////////////////////////////////////////////////

PolarFeature2dLogWriter::PolarFeature2dLogWriter( const orcalog::LogWriterInfo      &logWriterInfo )
    : orcalog::LogWriter( logWriterInfo )
{
    checkFormatIceOrAscii( logWriterInfo );
}
void 
PolarFeature2dLogWriter::write( const orca::PolarFeature2dData &obj )
{
    writeReferenceToMasterFile();
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               obj );
}

//////////////////////////////////////////////////////////////////////

PowerLogWriter::PowerLogWriter( const orcalog::LogWriterInfo      &logWriterInfo )
    : orcalog::LogWriter( logWriterInfo )
{
    checkFormatIceOrAscii( logWriterInfo );
}
void 
PowerLogWriter::write( const orca::PowerData &obj )
{
    writeReferenceToMasterFile();
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               obj );
}

//////////////////////////////////////////////////////////////////////

WifiLogWriter::WifiLogWriter( const orcalog::LogWriterInfo      &logWriterInfo )
    : orcalog::LogWriter( logWriterInfo )
{
    checkFormatIceOrAscii( logWriterInfo );
}
void 
WifiLogWriter::write( const orca::WifiData &obj )
{
    writeReferenceToMasterFile();
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               obj );
}

//////////////////////////////////////////////////////////////////////

GpsLogWriter::GpsLogWriter( const orcalog::LogWriterInfo  &logWriterInfo )
    : orcalog::LogWriter( logWriterInfo )
{
    checkFormatIceOrAscii( logWriterInfo );
}
void 
GpsLogWriter::write( const orca::GpsData &obj )
{
    writeReferenceToMasterFile();
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               obj );
}
void 
GpsLogWriter::write( const orca::GpsDescription &descr )
{
    if ( orcalog::LogWriter::numItemsLogged() > 0 )
        throw orcalog::Exception( ERROR_INFO, "Tried to write description after logging started" );
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               descr );
}

}

