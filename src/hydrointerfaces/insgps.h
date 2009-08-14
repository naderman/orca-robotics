/*
 * Orca-Robotics Project: Components for robotics
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Michael Moser
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_INTERFACES_INSGPS_H
#define HYDRO_INTERFACES_INSGPS_H

#include <cstdlib>
#include <string>
#include <memory>
#include <vector>

#include <hydroutil/context.h>


namespace hydrointerfaces
{

/*!
    @ingroup hydro_drivers
    @defgroup hydro_drivers_insgps InsGps drivers
    @brief Implementations of InsGps interface
*/
/*!
    @ingroup hydro_interfaces
    @defgroup hydro_interface_insgps InsGps
    @brief GPS aided INS.

    INS stands for inertial navigation system; basically it works by measuring the acceleration
    and turnrate vectors with an IMU (inertial measurement unit) and (double) integrating those
    to yield position velocity and attitude (PVA). To compensate for unbounded (quadratic) error growth
    it is often combined with a global sensor, e.g. GPS.
    Such a combined system ideally yields a locally smooth, globally correct PVA solution.

    @{
*/
//!
//! @brief Abstract class for a Ins/Gps system
//! 
//! 
//! All drivers should implement this, making it simple
//! to add support for different drivers
//! 
//! This guy is @b not guaranteed to be thread-safe!
//!
//! @author Michael Moser
//!
class SOEXPORT InsGps
{

public:

    //! possible types GenericData can contain
    enum Datatype {
        //! GenericData is really InsData
        Ins,
        //! GenericData is really GpsData
        Gps,
        //! GenericData is really ImuData
        Imu
    };

    //! possible Status Messages GenericData can contain
    enum StatusMessageType {
        //! Nothing new, no message
        NoMsg,
        //! Not quite ready
        Initialising,
        //! All good, but something to say
        Ok,
        //! Problem, likely to go away
        Warning,
        //! Problem, probably fatal
        Fault
    };

    //! Generic (base) type returned by a read
    class GenericData
    {
        public:
            virtual ~GenericData(){};
            virtual Datatype type() const=0;
            StatusMessageType statusMessageType;
            std::string statusMessage;
        private:
    };

    //! blocking read on the driver
    virtual std::auto_ptr<GenericData> read() = 0;

    // datatypes that can be encapsulated in GenericData
    //! Ins: global position plus velocity orientation
    class InsData : public GenericData {
    public:
        Datatype type() const { return Ins; }
        //! latitude [rad]
        double lat;
        //! longitude [rad]
        double lon;
        //! altitude [m] (above ellipsoid); alt - altAMSL == undulation
        double alt;
        //! altitude [m] (above mean sea level); use this to compare to (paper) maps
        double altAMSL;
        //! velocity vector [m/s] East, North, Up, relative to UTM grid
        double vENU[3];
        //! orientation vector [rad] Roll, Pitch, Yaw relative to local UTM grid (yaw == 0 --> East)
        double oRPY[3];
        struct timeval time;
    };

    //! @name Gps Status
    //! @{
    //! split up into solution status, correction-method and -source, to break down the bazillion possibilities. This is not as granular as it could be
    //! but should cover the major cases.

    //! solution status
    enum GpsSolutionStatus{
        //! no solution (no satellites, starting up, ...)
        NoFix,
        //! the receiver has a solution, but it's likely to be invalid (singularity, non-conversion, ...)
        BadFix,
        //! Calculated using some assumptions about Z-coordinate (sealevel, same as last, ...); probably not usable for control/mapping
        TwoDFix,
        //! usable with the usual caveats about GPS (multipath, ionosphere/sunspots, phase of the moon ...)
        GoodFix
    };
    //! what type of corrections do we have
    enum GpsCorrectionMethod{
        //! standalone GPS
        NoDiff,
        //! normal differential GPS; best case accuracy 1-2m; _not_ a filter
        CodeDiff,
        //! Carrier differential GPS; best case accuracy tens of cm; filtered: expect start and stop discontinuieties
        CarrierDiff
    };
    //! where do the corrections come from
    enum GpsCorrectionSource{
        //! implies short baseline, high rate, reliable (i.e. your own basestation with direct radio link)
        Local,
        //! long (virtual) baseline, lower rate  (i.e US-coastguard dgps)
        Distant,
        //! long (virtual) baseline, lower rate, point source (easily shaded) (e.g. WAAS/EGNOS, OmniStarXXX)
        Satellite
    };
    //! @}

    //! Gps: global position, plus velocity
    class GpsData : public GenericData {
    public:
        Datatype type() const { return Gps; }
        //! latitude [rad]
        double lat;
        //! error estimate in latitude (1 sigma?) [m]; at best a lower bound
        double sigmaLat;
        //! longitude [rad]
        double lon;
        //! error estimate in longitude (1 sigma?) [m]; at best a lower bound
        double sigmaLon;
        //! altitude [m] (above ellipsoid); alt - altAMSL == undulation
        double alt;
        //! altitude [m] (above mean sea level/geoid); use this to compare to (paper) maps
        double altAMSL;
        //! error estimate in altitude (1 sigma?) [m]; at best a lower bound
        double sigmaAlt;

        //!name Velocity vector
        //!@{
        double heading;   //!< [rad]
        double speed;     //!< [m/s]
        double climbRate; //!< [m/s]
        std::vector<double > latency; //!< [s] optional; velocity is often integrated over the measurement interval --> delayed by 1/2 the interval, however, it can be instantanious
        //!@}

        //! Nr of satellites used for solution
        int sat;
        //! Nr of L1 observations used for solution
        int obsL1;
        //! Nr of L2 observations used for solution
        int obsL2;

        //! what's the solution status
        enum GpsSolutionStatus gpsSolutionStatus;

        //! what correction method are we using
        enum GpsCorrectionMethod gpsCorrectionMethod;

        //! where we get corrections from; optional
        std::vector<enum GpsCorrectionSource> gpsCorrectionSource;

        //! age of corrections [s]; optional
        std::vector<double> gpsCorrectionAge;

        //! base station id (optional)
        std::string gpsBaseStationId;

        struct timeval time;
    };

    //! Imu: strapdown inertial measurements
    class ImuData : public GenericData {
    public:
        Datatype type() const { return Imu; }
        //! acceleration vector [m/s/s] in sensor coordinates
        double acc[3];
        //! turn rate vector [rad/s] in sensor coordinates
        double turnRate[3];
        //! true: reported values are internally corrected for sensor offsets; can be integrated as is, but use with care for filtering
        bool biasCorrected;
        //! optional sensor temperature [K]
        std::vector<double> tempr;
        struct timeval time;
    };

    //! general configuration of a Ins/Gps system; empty for now
    class Config
    {
    public:
        Config();
        bool isValid() const;
        std::string toString() const;
        //bool operator==( const Config & other );
        //bool operator!=( const Config & other );
    };

    virtual ~InsGps() {};

private:

};

//! Helper class to instantiate drivers
class SOEXPORT InsGpsFactory {
public:
    virtual ~InsGpsFactory() {};
    //! Instantiates a driver and returns a pointer to it.
    virtual InsGps *createDriver( const InsGps::Config &config,
                                  const hydroutil::Context &context ) const=0;
};

/*! @} */
} // namespace

// Function for dynamically instantiating drivers.
// A driver must have a function like so:
// extern "C" {
//     hydrointerfaces::InsGpsFactory *InsGpsFactory();
// }
typedef hydrointerfaces::InsGpsFactory *DriverFactoryMakerFunc();

#endif
