#ifndef __position2dI_h__
#define __position2dI_h__

#include <orca/position2d.h>
/*
namespace orca
{
*/
class Position2dConsumerI : virtual public orca::Position2dConsumer
{
public:

    virtual void setData(const ::orca::Position2dDataPtr&,
			 const Ice::Current&);
};
/*
class Position2dI : virtual public Position2d
{
public:

    virtual ::orca::Position2dDataPtr getData(const Ice::Current&) const;

    virtual ::orca::Position2dGeometryPtr getGeometry(const Ice::Current&) const;

    virtual void subscribe(const ::orca::Position2dConsumerPrx&,
			   ::Ice::Double,
			   const Ice::Current&);

    virtual void unsubscribe(const ::orca::Position2dConsumerPrx&,
			     const Ice::Current&);
};

}
*/
#endif
