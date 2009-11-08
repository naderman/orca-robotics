#ifndef ORCALOGFACTORY_LOGREADERS_H
#define ORCALOGFACTORY_LOGREADERS_H

#include <orcalog/logreader.h>
#include <orca/camera.h>
#include <orca/drivebicycle.h>
#include <orca/laserscanner2d.h>
#include <orca/localise2d.h>
#include <orca/localise3d.h>
#include <orca/power.h>
#include <orca/odometry2d.h>
#include <orca/odometry3d.h>
#include <orca/wifi.h>
#include <orca/pathfollower2d.h>
#include <orca/gps.h>

#include "camerareader.h"

namespace orcalogfactory {

//
// A collection of classes capable of reading sub-logfiles for specific
// interface types.
//

class CameraLogReader : public orcalog::LogReader
{
public:
    CameraLogReader( const orcalog::LogReaderInfo &logReaderInfo );
    void read( orca::ImageDataPtr &obj );
    void read( orca::CameraDescriptionPtr &obj );
    void read() { orca::ImageDataPtr obj=new orca::ImageData; read(obj); }
    void openLogFile();
private:
// alen - member class for camera log readingso we can handle jpegs
    CameraReader cameraReader;

};

//////////////////////////////////////////////////////////////////////

class DriveBicycleLogReader : public orcalog::LogReader
{
public:
    DriveBicycleLogReader( const orcalog::LogReaderInfo &logReaderInfo );
    void read( orca::DriveBicycleData &obj );
    void read( orca::VehicleDescription &obj );
    void read() { orca::DriveBicycleData obj; read(obj); }
};

//////////////////////////////////////////////////////////////////////

class GpsLogReader : public orcalog::LogReader
{
public:
    GpsLogReader( const orcalog::LogReaderInfo &logReaderInfo );
    void read( orca::GpsData &obj );
    void read( orca::GpsDescription &obj );
    void read() { orca::GpsData obj; read(obj); }
};

//////////////////////////////////////////////////////////////////////

class LaserScanner2dLogReader : public orcalog::LogReader
{
public:
    LaserScanner2dLogReader( const orcalog::LogReaderInfo &logReaderInfo );
    void read( orca::LaserScanner2dDataPtr &obj );
    void read( orca::RangeScanner2dDescription &obj );
    void read() { orca::LaserScanner2dDataPtr obj=new orca::LaserScanner2dData; read(obj); }
};

//////////////////////////////////////////////////////////////////////

class RangeScanner2dLogReader : public orcalog::LogReader
{
public:
    RangeScanner2dLogReader( const orcalog::LogReaderInfo &logReaderInfo );
    void read( orca::RangeScanner2dDataPtr &obj );
    void read( orca::RangeScanner2dDescription &obj );
    void read() { orca::RangeScanner2dDataPtr obj=new orca::RangeScanner2dData; read(obj); }
};

//////////////////////////////////////////////////////////////////////

class Localise2dLogReader : public orcalog::LogReader
{
public:
    Localise2dLogReader( const orcalog::LogReaderInfo &logReaderInfo );
    void read( orca::Localise2dData &obj );
    void read( orca::VehicleGeometryDescriptionPtr &obj );
    void read() { orca::Localise2dData obj; read(obj); }
};

//////////////////////////////////////////////////////////////////////

class Localise3dLogReader : public orcalog::LogReader
{
public:
    Localise3dLogReader( const orcalog::LogReaderInfo &logReaderInfo );
    void read( orca::Localise3dData &obj );
    void read( orca::VehicleGeometryDescriptionPtr &obj );
    void read() { orca::Localise3dData obj; read(obj); }
};

//////////////////////////////////////////////////////////////////////

class Odometry2dLogReader : public orcalog::LogReader
{
public:
    Odometry2dLogReader( const orcalog::LogReaderInfo &logReaderInfo );
    void read( orca::Odometry2dData &obj );
    void read( orca::VehicleDescription &obj );
    void read() { orca::Odometry2dData obj; read(obj); }
};

//////////////////////////////////////////////////////////////////////

class Odometry3dLogReader : public orcalog::LogReader
{
public:
    Odometry3dLogReader( const orcalog::LogReaderInfo &logReaderInfo );
    void read( orca::Odometry3dData &obj );
    void read( orca::VehicleDescription &obj );
    void read() { orca::Odometry3dData obj; read(obj); }
};

//////////////////////////////////////////////////////////////////////

class PowerLogReader : public orcalog::LogReader
{
public:
    PowerLogReader( const orcalog::LogReaderInfo &logReaderInfo );
    void read( orca::PowerData &obj );
    void read() { orca::PowerData obj; read(obj); }
    void initialised() { orcalog::LogReader::zeroLogIndex(); }
};

//////////////////////////////////////////////////////////////////////

class WifiLogReader : public orcalog::LogReader
{
public:
    WifiLogReader( const orcalog::LogReaderInfo &logReaderInfo );
    void read( orca::WifiData &obj );
    void read() { orca::WifiData obj; read(obj); }
    void initialised() { orcalog::LogReader::zeroLogIndex(); }
};

//////////////////////////////////////////////////////////////////////

class PathFollower2dLogReader : public orcalog::LogReader
{
public:
    PathFollower2dLogReader( const orcalog::LogReaderInfo &logReaderInfo );
    void read( orca::PathFollower2dData &obj );
    void read() { orca::PathFollower2dData obj; read(obj); }
    void initialised() { orcalog::LogReader::zeroLogIndex(); }
};

}

#endif
