#ifndef ORCALOGFACTORY_LOGWRITERS_H
#define ORCALOGFACTORY_LOGWRITERS_H

#include <orcalog/logwriter.h>
#include <orca/cpu.h>
#include <orca/drivebicycle.h>
#include <orca/imu.h>
#include <orca/laserscanner2d.h>
#include <orca/localise2d.h>
#include <orca/localise3d.h>
#include <orca/odometry2d.h>
#include <orca/odometry3d.h>
#include <orca/polarfeature2d.h>
#include <orca/power.h>
#include <orca/wifi.h>
#include <orca/gps.h>

namespace orcalogfactory {

//
// A collection of classes capable of writing sub-logfiles for specific
// interface types.
//

//////////////////////////////////////////////////////////////////////

class CpuLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::CpuData &obj );       
};

//////////////////////////////////////////////////////////////////////

class DriveBicycleLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::DriveBicycleData &obj );
    void write( const orca::VehicleDescription &descr );
};

//////////////////////////////////////////////////////////////////////

class ImuLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::ImuData &obj );        
};

//////////////////////////////////////////////////////////////////////

class LaserScanner2dLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::LaserScanner2dDataPtr &obj );        
    void write( const orca::RangeScanner2dDescription &descr );        
};

//////////////////////////////////////////////////////////////////////

class Localise2dLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::Localise2dData &obj );        
    void write( const orca::VehicleGeometryDescriptionPtr &descr );
};

//////////////////////////////////////////////////////////////////////

class Localise3dLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::Localise3dData &obj );        
    void write( const orca::VehicleGeometryDescriptionPtr &descr );
};

//////////////////////////////////////////////////////////////////////

class Odometry2dLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::Odometry2dData &obj );
    void write( const orca::VehicleDescription &descr );
};

//////////////////////////////////////////////////////////////////////

class Odometry3dLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::Odometry3dData &obj );
    void write( const orca::VehicleDescription &descr );
};

//////////////////////////////////////////////////////////////////////

class PolarFeature2dLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::PolarFeature2dData &obj );
};

//////////////////////////////////////////////////////////////////////

class PowerLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::PowerData &obj );
};

//////////////////////////////////////////////////////////////////////

class WifiLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::WifiData &obj );
};

//////////////////////////////////////////////////////////////////////

class GpsLogWriter : public orcalog::LogWriter
{
public:
    void checkFormat( const std::string &format );
    void write( const orca::GpsData &obj );
    void write( const orca::GpsDescription &descr );
};

//////////////////////////////////////////////////////////////////////



}

#endif
