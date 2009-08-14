/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDROINTERFACES_PATHPLANNER2D_H
#define HYDROINTERFACES_PATHPLANNER2D_H

#include <hydroutil/context.h>
#include <gbxutilacfr/exceptions.h>
#include <hydroogmap/hydroogmap.h>

// fwd decl
class QPicture;

namespace hydrointerfaces {

/*!
    @ingroup hydro_drivers
    @defgroup hydro_drivers_pathplanner2d PathPlanner2d drivers
    @brief Implementations of 2D Path-Planning
*/
/*!
    @ingroup hydro_interfaces
    @defgroup hydro_interface_pathplanner2d PathPlanner2d
    @brief Implementations of 2D Path-Planning

    @{
*/

//!
//! @brief Abstract class for 2D path planning
//!
//! @author Alex Brooks
//!
class SOEXPORT PathPlanner2d
{

public: 

    //!
    //! This abstract class allows drivers to publish 2D graphics showing their
    //! internal details, in Qt format.
    //!
    class QGraphics2dPublisher {
    public:
        virtual void init()=0;
        virtual void localSetAndSend( const QPicture &picture )=0;
    };

    //! Puts together the configuration details
    struct Config {
        Config( const hydroogmap::OgMap &m )
            : ogMap(m), graphicsPublisher(0) {}
        const hydroogmap::OgMap &ogMap;
        QGraphics2dPublisher    *graphicsPublisher;
    };

    //!
    //! Base class for exceptions thrown by Path Planners.
    //!
    class Exception : public gbxutilacfr::Exception
    {
    public:

        Exception( const char *file, const char *line, const std::string &message )
            : gbxutilacfr::Exception( file, line, message ) {}    
        virtual ~Exception() throw() {}

        //! Used for keeping the type of the exception, but adding more information
        void prependMsg( const char *file, const char *line, const std::string &msg )
            { message_ = gbxutilacfr::Exception::toMessageString(file,line,msg) + message_; }
    protected:
        std::string  message_;
    };

    class PathStartNotValidException : public Exception
    {
    public:
        PathStartNotValidException( const char *file, const char *line, const std::string &message )
            : Exception( file, line, std::string("PathStartNotValid: ")+message ) {}
    };

    class PathDestinationNotValidException : public Exception
    {
    public:
        PathDestinationNotValidException( const char *file, const char *line, const std::string &message )
            : Exception( file, line, std::string("PathDestinationNotValid: ")+message ) {}
    };

    class PathDestinationUnreachableException : public Exception
    {
    public:
        PathDestinationUnreachableException( const char *file, const char *line, const std::string &message )
            : Exception( file, line, std::string("PathDestinationUnreachable: ")+message ) {}
    };

    struct Point {
        Point() {}
        Point(double ix, double iy)
            : x(ix),y(iy) {}

        double x;
        double y;
    };

    virtual ~PathPlanner2d() {}

    //
    // Compute the path from 'start' to 'goal'.
    // In the returned path, the first element will be on the start, the last will be on the goal.
    //
    virtual std::vector<Point> computePath( const Point &start,
                                            const Point &goal ) = 0;

private: 


};

//! Helper class to instantiate drivers
class SOEXPORT PathPlanner2dFactory {
public:
    virtual ~PathPlanner2dFactory() {};
    //! Instantiates a driver and returns a pointer to it.
    virtual PathPlanner2d *createDriver( const PathPlanner2d::Config &config,
                                         const hydroutil::Context    &context ) const = 0;
};

/*! @} */
} // namespace

// Function for dynamically instantiating drivers.
// A driver must have a function like so:
// extern "C" {
//     hydrointerfaces::PathPlanner2dFactory *createDriverFactory();
// }
typedef hydrointerfaces::PathPlanner2dFactory *DriverFactoryMakerFunc();

#endif
