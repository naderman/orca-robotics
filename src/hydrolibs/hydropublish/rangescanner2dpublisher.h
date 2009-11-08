#ifndef HYDROPUBLISH_RANGESCANNER2DPUBLISHER_H
#define HYDROPUBLISH_RANGESCANNER2DPUBLISHER_H

#include <boost/shared_ptr.hpp>
#include <hydrotime/time.h>
#include <hydroscanutil/scanutil.h>
#include <hydronavutil/pose.h>

namespace hydropublish {

//
// @brief Knows how to inform the world of RangeScanner2d information.
//
// @author Alex Brooks
//
class RangeScanner2dPublisher
{
public: 
    virtual ~RangeScanner2dPublisher() {}

    virtual void init()=0;
    virtual void localSetAndSend( const std::vector<float> &ranges,
                                  const hydrotime::Time &time = hydrotime::Time(0,0) )=0;

private:

};
typedef boost::shared_ptr<RangeScanner2dPublisher> RangeScanner2dPublisherPtr;

//
// Does nothing
//
class DummyRangeScanner2dPublisher : public RangeScanner2dPublisher {
public:
    void init() {}
    void localSetAndSend( const std::vector<float> &ranges,
                          const hydrotime::Time &time = hydrotime::Time(0,0) ) {}
};

//
// @brief Knows how to create RangeScanner2dPublisher's
//
// @author Alex Brooks
//
class RangeScanner2dPublisherFactory
{
public:
    virtual ~RangeScanner2dPublisherFactory() {}

    virtual RangeScanner2dPublisherPtr createPublisher( const hydroscanutil::ScannerConfig &scannerConfig,
                                                        const hydronavutil::Pose           &scannerOffset,
                                                        const std::string                  &name ) = 0;

private:
};
typedef boost::shared_ptr<RangeScanner2dPublisherFactory> RangeScanner2dPublisherFactoryPtr;

//
// Produces dummy publishers
//
class DummyRangeScanner2dPublisherFactory : public RangeScanner2dPublisherFactory {
public:

    RangeScanner2dPublisherPtr createPublisher( const hydroscanutil::ScannerConfig &scannerConfig,
                                                const hydronavutil::Pose           &scannerOffset,
                                                const std::string                  &name )
        { return RangeScanner2dPublisherPtr( new DummyRangeScanner2dPublisher ); }

};

}

#endif
