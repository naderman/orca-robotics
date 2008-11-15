#ifndef ORCASIM2D_RANGESCANPUBLISHER_H
#define ORCASIM2D_RANGESCANPUBLISHER_H

#include <hydrosim2d/irangescanpublisher.h>
#include <orcaifaceimpl/rangescanner2d.h>

namespace orcasim2d {

//!
//! @brief Publishes range scans out an orca interface
//!
//! @author Alex Brooks
//!
class RangeScanPublisher : public hydrosim2d::IRangeScanPublisher
{

public: 

    RangeScanPublisher( const orca::RangeScanner2dDescription &descr,
                        const std::string                     &interfaceTag,
                        const orcaice::Context                &context )
        : rangeScanner2dImpl_(new orcaifaceimpl::RangeScanner2dImpl(descr,interfaceTag,context)),
          descr_(descr)
        {}

    void initInterface()
        { rangeScanner2dImpl_->initInterface(); }

    // From hydrosim2d::IRangeScanPublisher
    void publish( const std::vector<float> &ranges );

private: 

    orcaifaceimpl::RangeScanner2dImplPtr rangeScanner2dImpl_;
    orca::RangeScanner2dDescription      descr_;

};

}

#endif
