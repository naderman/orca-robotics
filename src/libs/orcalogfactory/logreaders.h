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
#include <orca/gps.h>

namespace orcalogfactory {

//
// A collection of classes capable of reading sub-logfiles for specific
// interface types.
//

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
};

//////////////////////////////////////////////////////////////////////

class WifiLogReader : public orcalog::LogReader
{
public:
    WifiLogReader( const orcalog::LogReaderInfo &logReaderInfo );
    void read( orca::WifiData &obj );
    void read() { orca::WifiData obj; read(obj); }
};

}

#endif
