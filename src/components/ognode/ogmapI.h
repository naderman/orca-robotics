#ifndef _OG_MAP_I_H_
#define _OG_MAP_I_H_

#include <orca/ogmap.h>

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/ogmap.h>

// utilities
#include <orcaice/ptrbuffer.h>
#include <orcaice/context.h>

namespace ognode
{

//
// Implements the Laser interface: Handles all our remote calls.
//
class OgMapI : public orca::OgMap
{
public:

    OgMapI( orca::OgMapDataPtr  theMap,
            const std::string  &tag,
            orcaice::Context    context );

    // Remote calls:

    // Hand out data to people
    orca::OgMapDataPtr getData(const Ice::Current&) const;

    void localSetData(orca::OgMapDataPtr data);

    virtual void subscribe(const ::orca::OgMapConsumerPrx&,
                           const Ice::Current&);

    virtual void unsubscribe(const ::orca::OgMapConsumerPrx&,
                             const Ice::Current&);

private:

    // The topic to which we'll publish
    IceStorm::TopicPrx             topicPrx_;
    // The interface to which we'll publish
    orca::OgMapConsumerPrx         consumerPrx_;

    // The latest data goes into this buffer
    orcaice::PtrBuffer<orca::OgMapDataPtr> ogMapDataBuffer_;

    //orca::OgMapDataPtr theMap_;

    orcaice::Context context_;
};

} // namespace

#endif
