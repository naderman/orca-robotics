#ifndef __ogmapI_h__
#define __ogmapI_h__

#include <orca/ogmap.h>

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/ogmap.h>

//!
//! Implements the Laser interface: Handles all our remote calls.
//!
class OgMapI : public orca::OgMap
{
public:

    OgMapI( orca::OgMapDataPtr theMap );

    // Remote calls:

    //! Hand out data to people
    orca::OgMapDataPtr getData(const Ice::Current&) const;

    //! We ignore this request
    virtual void subscribe(const ::orca::OgMapConsumerPrx&,
                           const Ice::Current&);

    //! We ignore this request
    virtual void unsubscribe(const ::orca::OgMapConsumerPrx&,
                             const Ice::Current&);

private:

    orca::OgMapDataPtr theMap_;
};

#endif
