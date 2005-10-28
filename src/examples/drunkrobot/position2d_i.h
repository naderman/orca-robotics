

#ifndef ORCA2_DR_POSITION2D_I_H
#define ORCA2_DR_POSITION2D_I_H

// include provided interfaces
#include <orca/position2d.h>

// utilities
#include <orcaiceutil/ptrbuffer.h>
#include <orcaiceutil/connectutils.h>

// This interface allows us to get information in and out of our 
// robot. It uses a pointer buffer (a nice thread safe class from the 
// orcaiceutil library) to allow us to handle things asynchronously. 
// We also use a pointer for geometry, since it's unlikely the geometry 
// will change much. 

class Position2dI : public orca::Position2d
{
public:

    //!Constructor takes a pointer buffer 
    Position2dI( orcaiceutil::PtrBuffer<orca::Position2dDataPtr> &posBuffer, 
		 orca::Position2dGeometryPtr &geomBuffer, 
		 const IceStorm::TopicPrx &topic);

    // Make these virtual so derived classes can override them. 
    
    // We pass in the Ice context as well -- these are added to the normal list of 
    // parameters. 

    virtual ::orca::Position2dDataPtr getData(const ::Ice::Current& ) const;

    virtual ::orca::Position2dGeometryPtr getGeometry(const ::Ice::Current& ) const;

    // These functions are for subscribing and unsubscribing listeners. They give us a
    // consumer proxy, and we do the callback whenever we want to push data. 

    virtual void subscribe(const ::orca::Position2dConsumerPrx&, ::Ice::Double, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::Position2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());



    // the driver will put the latest data into this proxy
    orcaiceutil::PtrBuffer<orca::Position2dDataPtr> &posBuffer_;
    
   // the driver will use this member to return geometry when requested. 
    orca::Position2dGeometryPtr& geomBuffer_; 

    // This is used to set up the topic for us to push information. 
    IceStorm::TopicPrx topic_;


};


#endif
