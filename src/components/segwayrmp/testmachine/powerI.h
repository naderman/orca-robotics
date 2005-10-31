#ifndef __powerI_h__
#define __powerI_h__

#include <orca/power.h>

/*
namespace orca
{

class PowerI : virtual public Power
{
public:

    virtual ::orca::PowerDataPtr getData(const Ice::Current&) const;
};
*/
class PowerConsumerI : virtual public orca::PowerConsumer
{
public:

    virtual void setData(const ::orca::PowerDataPtr&,
			 const Ice::Current&);
};
/*
}
*/
#endif
