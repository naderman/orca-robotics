#ifndef ORCAPUBLISH_RANGESCANNER2DPUBLISHER_H
#define ORCAPUBLISH_RANGESCANNER2DPUBLISHER_H

#include <hydropublish/rangescanner2dpublisher.h>
#include <orcaifaceimpl/rangescanner2d.h>
#include <orcascanutil/orcascanutil.h>

namespace orcapublish {

//!
//! @brief Publishes range scans out an orca interface
//!
//! @author Alex Brooks
//!
class RangeScanner2dPublisher : public hydropublish::RangeScanner2dPublisher
{
public:

    RangeScanner2dPublisher( const orca::RangeScanner2dDescription &descr,
                             const orcaice::Context                &context,
                             const std::string                     &interfaceString )
        : rangeScanner2dImpl_(new orcaifaceimpl::RangeScanner2dImpl(descr,context,interfaceString)),
          descr_(descr)
        {}

    void init()
        { rangeScanner2dImpl_->initInterface(); }

    // From hydropublish::RangeScanner2dPublisher
    void localSetAndSend( const std::vector<float> &ranges,
                          const hydrotime::Time    &time )
        {
            orca::RangeScanner2dDataPtr orcaData = new orca::RangeScanner2dData;

            orcaData->timeStamp.seconds  = time.seconds();
            orcaData->timeStamp.useconds = time.useconds();
            orcaData->ranges             = ranges;
            orcaData->minRange           = descr_.minRange;
            orcaData->maxRange           = descr_.maxRange;
            orcaData->fieldOfView        = descr_.fieldOfView;
            orcaData->startAngle         = descr_.startAngle;

            rangeScanner2dImpl_->localSetAndSend( orcaData );
        }

private:

    orcaifaceimpl::RangeScanner2dImplPtr rangeScanner2dImpl_;
    orca::RangeScanner2dDescription      descr_;

};

//!
//! @brief Knows how to create RangeScanner2dPublisher's
//!
//! @author Alex Brooks
//!
class RangeScanner2dPublisherFactory : public hydropublish::RangeScanner2dPublisherFactory
{
public:

    RangeScanner2dPublisherFactory( const orcaice::Context &context )
        : context_(context)
        {}

    hydropublish::RangeScanner2dPublisherPtr createPublisher( const hydroscanutil::ScannerConfig &scannerConfig,
                                                              const hydronavutil::Pose           &scannerOffset,
                                                              const std::string                  &name )
        { 
            return hydropublish::RangeScanner2dPublisherPtr(
                new RangeScanner2dPublisher( orcascanutil::convert( scannerConfig, scannerOffset ),
                                             context_,
                                             name ) ); 
        }

private:

    orcaice::Context context_;
};

}

#endif
