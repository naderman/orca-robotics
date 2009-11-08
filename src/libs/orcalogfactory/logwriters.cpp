#include "logwriters.h"
#include <iostream>
#include <fstream>
#include <orcalog/orcalog.h>
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
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
            (*file) << endl;
        }
        else if ( format == "ascii" )
        {
            (*file) << toLogString(obj) << endl;
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
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
            (*file) << endl;
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
                    const orca::RangeScanner2dDescription &obj )
    {
        if ( format == "ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writeRangeScanner2dDescription( helper.stream_, obj );
            helper.write( file );  
        }
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
            (*file) << endl;
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
                    const orca::GpsDescription &obj )
    {
        if ( format=="ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writeGpsDescription( helper.stream_, obj );
            helper.write( file );  
        }
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
            (*file) << endl;
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
                    const orca::ImageDescriptionPtr &obj )
    {
        if ( format=="ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writeImageDescription( helper.stream_, obj );
            helper.write( file );  
        }
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
            (*file) << endl;
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
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
            (*file) << endl;
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
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
            (*file) << endl;
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
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
            (*file) << endl;
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
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
            (*file) << endl;
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
                    const orca::RangeScanner2dDataPtr &obj )
    {        
        if ( format == "ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writeRangeScanner2dData( helper.stream_, obj );
            helper.write( file );
        }
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
            (*file) << endl;
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
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
            (*file) << endl;
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
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
            (*file) << endl;
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
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
            (*file) << endl;
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
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
            (*file) << endl;
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
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
            (*file) << endl;
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
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
            (*file) << endl;
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
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
            (*file) << endl;
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
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
            (*file) << endl;
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
                    const orca::ImageDataPtr                   &obj )
    {        

        if ( format == "ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writeImageData( helper.stream_, obj );
            helper.write( file );
        }
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
            (*file) << endl;
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
                    const orca::PathFollower2dData &obj )
    {        
        if ( format == "ice" )
        {
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writePathFollower2dData( helper.stream_, obj );
            helper.write( file );
        }
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
            (*file) << endl;
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

void
CpuLogWriter::checkFormat( const std::string &format )
{
    orcalog::checkFormatDefaultsOrAscii( format );
}
void 
CpuLogWriter::write( const orca::CpuData &obj, const orca::Time &arrivalTime  )
{
    writeReferenceToMasterFile(arrivalTime);
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               obj );
}

//////////////////////////////////////////////////////////////////////

void
DriveBicycleLogWriter::checkFormat( const std::string &format )
{
    orcalog::checkFormatDefaultsOrAscii( format );
}
void 
DriveBicycleLogWriter::write( const orca::DriveBicycleData &obj, const orca::Time &arrivalTime  )
{
    writeReferenceToMasterFile(arrivalTime);
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

void
ImuLogWriter::checkFormat( const std::string &format )
{
    orcalog::checkFormatDefaultsOrAscii( format );
}
void 
ImuLogWriter::write( const orca::ImuData &obj, const orca::Time &arrivalTime  )
{
    writeReferenceToMasterFile(arrivalTime);
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               obj );
}

//////////////////////////////////////////////////////////////////////

void
LaserScanner2dLogWriter::checkFormat( const std::string &format )
{
    orcalog::checkFormatDefaultsOrAscii( format );
}
void 
LaserScanner2dLogWriter::write( const orca::LaserScanner2dDataPtr &obj, const orca::Time &arrivalTime  )
{
    writeReferenceToMasterFile(arrivalTime);
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

void
RangeScanner2dLogWriter::checkFormat( const std::string &format )
{
    orcalog::checkFormatDefaultsOnly( format );
}
void 
RangeScanner2dLogWriter::write( const orca::RangeScanner2dDataPtr &obj, const orca::Time &arrivalTime  )
{
    writeReferenceToMasterFile(arrivalTime);
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               obj );
}
void 
RangeScanner2dLogWriter::write( const orca::RangeScanner2dDescription &descr )
{
    if ( orcalog::LogWriter::numItemsLogged() > 0 )
        throw orcalog::Exception( ERROR_INFO, "Tried to write description after logging started" );
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               descr );
}

//////////////////////////////////////////////////////////////////////

void
Localise2dLogWriter::checkFormat( const std::string &format )
{
    orcalog::checkFormatDefaultsOrAscii( format );
}
void 
Localise2dLogWriter::write( const orca::Localise2dData &obj, const orca::Time &arrivalTime  )
{
    writeReferenceToMasterFile(arrivalTime);
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

void
Localise3dLogWriter::checkFormat( const std::string &format )
{
    orcalog::checkFormatDefaultsOrAscii( format );
}
void 
Localise3dLogWriter::write( const orca::Localise3dData &obj, const orca::Time &arrivalTime  )
{
    writeReferenceToMasterFile(arrivalTime);
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

void
Odometry2dLogWriter::checkFormat( const std::string &format )
{
    orcalog::checkFormatDefaultsOnly( format );
}
void 
Odometry2dLogWriter::write( const orca::Odometry2dData &obj, const orca::Time &arrivalTime  )
{
    writeReferenceToMasterFile(arrivalTime);
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

void
Odometry3dLogWriter::checkFormat( const std::string &format )
{
    orcalog::checkFormatDefaultsOnly( format );
}
void 
Odometry3dLogWriter::write( const orca::Odometry3dData &obj, const orca::Time &arrivalTime  )
{
    writeReferenceToMasterFile(arrivalTime);
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

void
PolarFeature2dLogWriter::checkFormat( const std::string &format )
{
    orcalog::checkFormatDefaultsOrAscii( format );
}
void 
PolarFeature2dLogWriter::write( const orca::PolarFeature2dData &obj, const orca::Time &arrivalTime  )
{
    writeReferenceToMasterFile(arrivalTime);
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               obj );
}

//////////////////////////////////////////////////////////////////////

void
PowerLogWriter::checkFormat( const std::string &format )
{
    orcalog::checkFormatDefaultsOrAscii( format );
}
void 
PowerLogWriter::write( const orca::PowerData &obj, const orca::Time &arrivalTime  )
{
    writeReferenceToMasterFile(arrivalTime);
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               obj );
}

//////////////////////////////////////////////////////////////////////

void
WifiLogWriter::checkFormat( const std::string &format )
{
    orcalog::checkFormatDefaultsOrAscii( format );
}
void 
WifiLogWriter::write( const orca::WifiData &obj, const orca::Time &arrivalTime  )
{
    writeReferenceToMasterFile(arrivalTime);
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               obj );
}

//////////////////////////////////////////////////////////////////////

void
GpsLogWriter::checkFormat( const std::string &format )
{
    orcalog::checkFormatDefaultsOrAscii( format );
}
void 
GpsLogWriter::write( const orca::GpsData &obj, const orca::Time &arrivalTime  )
{
    writeReferenceToMasterFile(arrivalTime);
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

//////////////////////////////////////////////////////////////////////

void
ImageLogWriter::checkFormat( const std::string &format )
{
    orcalog::checkFormatDefaultsOnly( format );
}
void 
ImageLogWriter::write( const orca::ImageDataPtr &obj, const orca::Time &arrivalTime  )
{
    writeReferenceToMasterFile(arrivalTime);
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               obj );
}
void 
ImageLogWriter::write( const orca::ImageDescriptionPtr &descr )
{
    if ( orcalog::LogWriter::numItemsLogged() > 0 )
        throw orcalog::Exception( ERROR_INFO, "Tried to write description after logging started" );
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               descr );
}

//////////////////////////////////////////////////////////////////////

void
CameraLogWriter::checkFormat( const std::string &format )
{
    vector<string> okFormats;
    okFormats.push_back("ice");
    okFormats.push_back("jpeg");
    orcalog::checkFormats( format, okFormats );
}

void 
CameraLogWriter::write( const orca::CameraDescriptionPtr &descr )
{
    if ( orcalog::LogWriter::numItemsLogged() > 0 )
        throw orcalog::Exception( ERROR_INFO, "Tried to write description after logging started" );

    cameraWriter.logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               descr );

}

void 
CameraLogWriter::write( const orca::ImageDataPtr &obj, const orca::Time &arrivalTime  )
{
    writeReferenceToMasterFile(arrivalTime);

    cameraWriter.logToFile( file_,
            orcalog::LogWriter::logWriterInfo().format,
            orcalog::LogWriter::logWriterInfo().context,
            obj);

}
//
//alen - had to overwrite this method from logwriter.h to include jpegs
//
void 
CameraLogWriter::createLogFile( const std::string &filename, const std::string &format )
{
    // Check that the file's not already open
    if ( file_ ) {
        std::string s = "Log file already exists.";
        throw orcalog::FileException( ERROR_INFO, s );
    }

    //
    // create log file, will throw expection otherwise
    //
    if ( format == "ice" ) {
        file_ = orcalog::openBinaryFileForWriting(filename);
    }
    else if ( format == "jpeg" ) {
        cameraWriter.createLogDirectory(filename);
        file_ = orcalog::openAsciiFileForWriting(filename);
    }
    else {
        // format is not supported: throw exception
        stringstream ss;
        ss << "Unknown log format: "<<format<<endl;
        throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
    }
}

//////////////////////////////////////////////////////////////////////

void
MultiCameraLogWriter::checkFormat( const std::string &format )
{
    vector<string> okFormats;
    okFormats.push_back("ice");
    okFormats.push_back("jpeg");
    okFormats.push_back("avi");
    orcalog::checkFormats( format, okFormats );
}

void
MultiCameraLogWriter::write( const orca::MultiCameraDescriptionPtr &descr )
{
    if ( orcalog::LogWriter::numItemsLogged() > 0 )
        throw orcalog::Exception( ERROR_INFO, "Tried to write description after logging started" );

    multiCameraWriter.logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               descr );

}

void
MultiCameraLogWriter::write( const orca::MultiCameraDataPtr &obj, const orca::Time &arrivalTime  )
{
    writeReferenceToMasterFile(arrivalTime);

    multiCameraWriter.logToFile( file_,
            orcalog::LogWriter::logWriterInfo().format,
            orcalog::LogWriter::logWriterInfo().context,
            obj);

}


void MultiCameraLogWriter::initMultiCameraWriter( const orca::MultiCameraDescriptionPtr &descr ) {
   multiCameraWriter.init(descr, orcalog::LogWriter::logWriterInfo().format);
}
//
//alen - had to overwrite this method from logwriter.h to include jpegs
//
void
MultiCameraLogWriter::createLogFile( const std::string &filename, const std::string &format )
{
    // Check that the file's not already open
    if ( file_ ) {
        std::string s = "Log file already exists.";
        throw orcalog::FileException( ERROR_INFO, s );
    }

    //
    // create log file, will throw expection otherwise
    //
    if ( format == "ice" ) {
        file_ = orcalog::openBinaryFileForWriting(filename);
    }
    else if ( format == "jpeg" || format == "avi") {
        multiCameraWriter.createLogDirectory(filename);
        file_ = orcalog::openAsciiFileForWriting(filename);
    }
    else {
        // format is not supported: throw exception
        stringstream ss;
        ss << "Unknown log format: "<<format<<endl;
        throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
    }
}

//////////////////////////////////////////////////////////////////////

void
PathFollower2dLogWriter::checkFormat( const std::string &format )
{
    orcalog::checkFormatDefaultsOnly( format );
}
void 
PathFollower2dLogWriter::write( const orca::PathFollower2dData &obj, const orca::Time &arrivalTime  )
{
    writeReferenceToMasterFile(arrivalTime);
    logToFile( file_,
               orcalog::LogWriter::logWriterInfo().format,
               orcalog::LogWriter::logWriterInfo().context,
               obj );
}

}

