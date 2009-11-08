#ifndef HYDROPUBLISH_ODOMETRY2DPUBLISHER_H
#define HYDROPUBLISH_ODOMETRY2DPUBLISHER_H

#include <boost/shared_ptr.hpp>
#include <hydronavutil/hydronavutil.h>
#include <hydrotime/time.h>
#include <hydronavutil/vehicledescription.h>

namespace hydropublish {

//
// @brief Knows how to inform the world of Odometry2d information.
//
// @author Alex Brooks
//
class Odometry2dPublisher
{
public: 

    virtual ~Odometry2dPublisher() {}

    virtual void init()=0;
    virtual void localSetAndSend( const hydronavutil::Pose     &pose,
                                  const hydronavutil::Velocity &velocity,
                                  const hydrotime::Time        &time = hydrotime::Time(0,0),
                                  bool                          odometryWasReset=false )=0;

private:

};
typedef boost::shared_ptr<Odometry2dPublisher> Odometry2dPublisherPtr;

//
// Does nothing
//
class DummyOdometry2dPublisher : public Odometry2dPublisher {
public:
    void init() {}
    void localSetAndSend( const hydronavutil::Pose     &pose,
                          const hydronavutil::Velocity &velocity,
                          const hydrotime::Time        &time = hydrotime::Time(0,0),
                          bool                          odometryWasReset=false ) {}
};

//
// @brief Knows how to create Odometry2dPublisher's
//
// @author Alex Brooks
//
class Odometry2dPublisherFactory
{
public:
    virtual ~Odometry2dPublisherFactory() {}

    virtual Odometry2dPublisherPtr createPublisher( const hydronavutil::DiffDriveControlDescription    &controlDescription,
                                                    const hydronavutil::CylindricalGeometryDescription &cylindricalDescription,
                                                    const std::string                                  &name ) = 0;

private:
};
typedef boost::shared_ptr<Odometry2dPublisherFactory> Odometry2dPublisherFactoryPtr;

//
// Produces dummy publishers
//
class DummyOdometry2dPublisherFactory : public Odometry2dPublisherFactory {
public:

    Odometry2dPublisherPtr createPublisher( const hydronavutil::DiffDriveControlDescription            &controlDescription,
                                                    const hydronavutil::CylindricalGeometryDescription &cylindricalDescription,
                                            const std::string                                          &name )
        { return Odometry2dPublisherPtr( new DummyOdometry2dPublisher ); }

};

}

#endif
