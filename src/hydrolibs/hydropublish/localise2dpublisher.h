#ifndef HYDROPUBLISH_LOCALISE2DPUBLISHER_H
#define HYDROPUBLISH_LOCALISE2DPUBLISHER_H

#include <boost/shared_ptr.hpp>
#include <hydronavutil/hydronavutil.h>
#include <hydrotime/time.h>
#include <hydronavutil/vehicledescription.h>

namespace hydropublish {

//
// @brief Knows how to inform the world of Localise2d information.
//
// @author Alex Brooks
//
class Localise2dPublisher
{
public: 
    virtual ~Localise2dPublisher() {}


    virtual void init()=0;
    virtual void localSetAndSend( const hydronavutil::Gmm &gmm,
                                  const hydrotime::Time &timeStamp )=0;
    virtual void localSetAndSend( const hydronavutil::Pose &pose,
                                  const hydrotime::Time    &time = hydrotime::Time(0,0) )=0;

private:

};
typedef boost::shared_ptr<Localise2dPublisher> Localise2dPublisherPtr;

//
// Does nothing
//
class DummyLocalise2dPublisher : public Localise2dPublisher {
public:
    void init() {}
    void localSetAndSend( const hydronavutil::Gmm &gmm,
                          const hydrotime::Time &timeStamp ) {}
    void localSetAndSend( const hydronavutil::Pose &pose,
                          const hydrotime::Time    &time = hydrotime::Time(0,0) ) {}
};

//
// @brief Knows how to create Localise2dPublisher's
//
// @author Alex Brooks
//
class Localise2dPublisherFactory
{
public:
    virtual ~Localise2dPublisherFactory() {}

    virtual Localise2dPublisherPtr createPublisher( 
        const hydronavutil::CylindricalGeometryDescription &cylindricalDescription,
        const std::string                                  &name ) = 0;

    virtual Localise2dPublisherPtr createPublisher( const std::string &name )=0;

private:
};
typedef boost::shared_ptr<Localise2dPublisherFactory> Localise2dPublisherFactoryPtr;

//
// Produces dummy publishers
//
class DummyLocalise2dPublisherFactory : public Localise2dPublisherFactory {
public:

    Localise2dPublisherPtr createPublisher( const hydronavutil::CylindricalGeometryDescription &cylindricalDescription,
                                            const std::string                  &name )
        { return Localise2dPublisherPtr( new DummyLocalise2dPublisher ); }
    Localise2dPublisherPtr createPublisher( const std::string &name )
        { return Localise2dPublisherPtr( new DummyLocalise2dPublisher ); }

};

}

#endif
