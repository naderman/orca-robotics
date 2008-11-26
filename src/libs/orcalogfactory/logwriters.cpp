#include "logwriters.h"
#include <iostream>
#include <fstream>
#include <orcalog/orcalog.h>
#include <orcaobj/stringutils.h>
#include <orcaifacelog/orcaifacelog.h>
#include "logstringutils.h"

using namespace std;

namespace orcalogfactory {

namespace {

    void checkFormats( const std::string              &format,
                       const std::vector<std::string> &okFormats )
    {
        for ( size_t i=0; i < okFormats.size(); i++ )
        {
            if ( format == okFormats[i] )
            {
                // format is supported
                return;
            }
        }

        // format is not supported: throw exception
        stringstream ss;
        ss << "Unknown log format: "<<format<<endl
           << "  Supported formats are: ";
        for ( size_t i=0; i < okFormats.size(); i++ )
        {
            ss << okFormats[i];
            if ( i != okFormats.size()-1 ) ss << ", ";
        }
        throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
    }

    void
    checkFormatIceOnly( const std::string &format )
    {
        vector<string> okFormats;
        okFormats.push_back("ice");
        okFormats.push_back("asciigenerated"); // auto-generated -> always available; need to adjust function name
        checkFormats( format, okFormats );
    }

    void
    checkFormatIceOrAscii( const std::string &format )
    {
        vector<string> okFormats;
        okFormats.push_back("ice");
        okFormats.push_back("ascii");
        okFormats.push_back("asciigenerated");
        checkFormats( format, okFormats );
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
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
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
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
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
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
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
        else if ( format=="asciigenerated" )
        {
            ifacelog::toLogStream( obj, *file);
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
    checkFormatIceOrAscii( format );
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
    checkFormatIceOrAscii( format );
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
    checkFormatIceOrAscii( format );
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
    checkFormatIceOrAscii( format );
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
Localise2dLogWriter::checkFormat( const std::string &format )
{
    checkFormatIceOrAscii( format );
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
    checkFormatIceOrAscii( format );
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
    checkFormatIceOrAscii( format );
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
    checkFormatIceOrAscii( format );
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
    checkFormatIceOrAscii( format );
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
    checkFormatIceOrAscii( format );
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
    checkFormatIceOrAscii( format );
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
    checkFormatIceOrAscii( format );
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
    checkFormatIceOnly( format );
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
    checkFormats( format, okFormats );
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
CameraLogWriter::write( const orca::CameraDataPtr &obj, const orca::Time &arrivalTime  )
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
}

