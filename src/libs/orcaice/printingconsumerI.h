/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAICE_PRINTING_CONSUMER_I_H
#define ORCAICE_PRINTING_CONSUMER_I_H

#include <iostream>
// #include <orcaobj/orcaobj.h>

namespace orcaice
{
/*!
 *  @brief A convenience class for consumers with setData() operation.
 *
 *  When the data arrives it does one thing: prints it out to standard output.
 *
 *  Relies on the fact that the Consumer objects has only one operation
 *  to implement and it's called setData().
 */
template<class ConsumerType, class ObjectPtrType>
class PrintingConsumerI : public ConsumerType
{
public:
    virtual ~PrintingConsumerI() {};
    
    virtual void setData( const ObjectPtrType& data, const Ice::Current& );

protected:

    //! You can derive from this class and do something when an object is received.
    //! (besides printing it out). This adds the functionality of orcaice::PtrNotify.
    virtual void handleData( const ObjectPtrType & obj ) {};
};

template<class ConsumerType, class ObjectPtrType>
void
PrintingConsumerI<ConsumerType,ObjectPtrType>::setData( const ObjectPtrType& data, const Ice::Current& )
{
    std::cout << orcaice::toString(data) << std::endl;
}

} // namespace

#endif
