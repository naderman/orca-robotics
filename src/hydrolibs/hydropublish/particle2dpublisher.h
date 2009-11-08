#ifndef HYDROPUBLISH_PARTICLEPUBLISHER_H
#define HYDROPUBLISH_PARTICLEPUBLISHER_H

#include <boost/shared_ptr.hpp>
#include <vector>

namespace hydropublish {

//
// @author Alex Brooks
//
class Particle2d {
public:
    double x;
    double y;
    double theta;
    double weight;
};

//
// @author Alex Brooks
//
class Particle2dPublisher
{
public: 
    virtual ~Particle2dPublisher() {}

    virtual void init()=0;
    virtual void localSetAndSend( const std::vector<Particle2d> &particles )=0;

private: 


};
typedef boost::shared_ptr<Particle2dPublisher> Particle2dPublisherPtr;

//
// @author Alex Brooks
//
class Particle2dPublisherFactory
{
public:
    virtual ~Particle2dPublisherFactory() {}

    virtual Particle2dPublisherPtr createPublisher( const std::string &name )=0;

private:
};
typedef boost::shared_ptr<Particle2dPublisherFactory> Particle2dPublisherFactoryPtr;

}

#endif
