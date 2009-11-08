#ifndef HYDROPUBLISH_OGMAPPUBLISHER_H
#define HYDROPUBLISH_OGMAPPUBLISHER_H

#include <boost/shared_ptr.hpp>
#include <hydroogmap/ogmap.h>

namespace hydropublish {

//
// @brief Allows publishing of an OG map.
//
// @author Alex Brooks
//
class OgMapPublisher
{
public: 
    virtual ~OgMapPublisher() {}

    virtual void init()=0;
    virtual void localSetAndSend( const hydroogmap::OgMap &ogMap )=0;

private: 

};
typedef boost::shared_ptr<OgMapPublisher> OgMapPublisherPtr;

// Does nothing
class DummyOgMapPublisher : public OgMapPublisher {
public:
    virtual void init() {}
    virtual void localSetAndSend( const hydroogmap::OgMap &ogMap ) {}
};

//
// @brief Allows pulishers of unknown concrete type to be created
//
// @author Alex Brooks
//
class OgMapPublisherFactory
{
public:
    virtual ~OgMapPublisherFactory() {}

    virtual OgMapPublisherPtr createPublisher( const std::string &name )=0;

private:
};
typedef boost::shared_ptr<OgMapPublisherFactory> OgMapPublisherFactoryPtr;

// Creates dummy publishers
class DummyOgMapPublisherFactory : public OgMapPublisherFactory {
public:
    OgMapPublisherPtr createPublisher( const std::string &name )
        { return OgMapPublisherPtr(new DummyOgMapPublisher); }    
};

}

#endif
