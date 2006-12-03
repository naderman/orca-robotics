/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_PROXY_H
#define ORCAICE_PROXY_H

#include <orcaice/exceptions.h>

#include <Ice/Ice.h>
#include <IceUtil/Monitor.h>
#include <IceUtil/Time.h>

namespace orcaice
{

/*!
 *  @brief A thread-safe data pipe with proxy semantics.
 *
 *  This container is similar to a circular Buffer of size one but with two
 *  differences:
 *  - a copy of the data is always available, yet the user knows when new
 *    data has arrived by calling @ref isNew
 *  - @ref getNext returns the new data arrives (not when the buffer
 *    is non-empty.
 *
 *  Write to proxy with @ref set. Read its contents with @ref get. Trying to read from
 *  an empty proxy raises an orcaice::Exception.
 * 
 *  @see Proxy, Buffer, Notify
 */
template<class Type>
class Proxy : public IceUtil::Monitor<IceUtil::Mutex>
{
public:

    Proxy();
    virtual ~Proxy();

    //! Returns TRUE if there's something in the proxy. The proxy starts its life empty
    //! but after the object is set once, it will be non-empty again until @ref purge is
    //! called.
    bool isEmpty() const;
    
    //! Returns TRUE if the data in the proxy has not been accessed with @ref get yet.
    bool isNewData() const;

    //! Sets the contents of the proxy.
    void set( const Type & obj );

    //! Returns the contents of the proxy. This operation makes the data in the proxy
    //! "not new", i.e. @ref isNewData returns FALSE. Calls to @ref get when the proxy is empty
    //! raises an orcaice::Exception exception.
    void get( Type & obj ) const;

    /*!
     *  @brief Waits until the next update and returns the new value.
     *  If the proxy is empty, @ref getNext blocks until the proxy is set and returns the new value.
     *  By default, there is no timeout (negative value). Returns 0 if successful.
     *
     *  If timeout is set to a positive value (in milliseconds) and the wait times out, the function returns -1
     *  and the object argument itself is not touched. In the rare event of spurious wakeup,
     *  the return value is 1.
     *
     */
    int  getNext( Type & obj, int timeoutMs=-1 ) const;
    
    //! Makes the proxy empty.
    //! @see isEmpty
    void purge();

protected:
    
    // local copy of the object
    Type obj_;

    // Reimplement this function for non-standard types.
    virtual void internalGet( Type & obj ) const ;
    
    // Reimplement this function for non-standard types.
    virtual void internalSet( const Type & obj );

private:


    bool isEmpty_;
    
    // flag to keep track of new data. Make it mutable so that get() functions can be const.
    mutable bool isNewData_;

    // internal implementation of front( obj, -1 ); returns 0.
    int  getNextNoWait( Type & obj ) const;

};


//////////////////////////////////////////////////////////////////////

template<class Type>
Proxy<Type>::Proxy()
    : isEmpty_(true),
      isNewData_(false)
{
}

template<class Type>
Proxy<Type>::~Proxy()
{
}

template<class Type>
bool Proxy<Type>::isEmpty() const
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    return isEmpty_;
}

template<class Type>
bool Proxy<Type>::isNewData() const
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    return isNewData_;
}

template<class Type>
void Proxy<Type>::get( Type & obj ) const
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    if ( !isEmpty_ )
    {
        internalGet( obj );
        isNewData_ = false;
    }
    else
    {
        throw orcaice::Exception( ERROR_INFO, "trying to read from an empty proxy." );
    }
}

template<class Type>
int Proxy<Type>::getNext( Type & obj, int timeoutMs ) const
{
    // special case: infinite wait time
    if ( timeoutMs == -1 ) {
        return getNextNoWait( obj );
    }

    // finite wait time
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

    // if already have data in the buffer, return it and get out
    if ( isNewData_ )
    {
        internalGet( obj );
        isNewData_ = false;
        return 0;
    }

    // empty buffer: figure out when to wake up
    // notice that we are still holding the lock, so it's ok to call timedWait()
    if (  this->timedWait( IceUtil::Time::milliSeconds( timeoutMs ) ) )  
    {
        // someone woke us up, we are holding the lock again
        // check new data again (could be a spurious wakeup)
        if ( isNewData_ ) 
        {
            internalGet( obj );
            isNewData_ = false;
            return 0;
        }
        else {
            // spurious wakup, don't wait again, just return
            return 1;
        }
    }
    else {
        // wait timedout, nobody woke us up
        return -1;
    }
}

template<class Type>
int Proxy<Type>::getNextNoWait( Type & obj ) const
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

    // check the condition before and after waiting to deal with spurious wakeups
    // (see Ice manual sec. 28.9.2)
    while ( !isNewData_ ) 
    {
        this->wait();
    }
   
    internalGet( obj );
    isNewData_ = false;
    return 0;
}

// NOTE: see notes on efficient notification in Ice sec. 28.9.3
template<class Type>
void Proxy<Type>::set( const Type &obj )
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    
    internalSet( obj );

    // mark as having new data (nobody has looked at it yet)
    isNewData_ = true;
    
    // mark proxy non-empty (only usefull the very first time)
    isEmpty_ = false;
    
    // wakeup someone who's waiting for an update
    this->notify();
}

template<class Type>
void Proxy<Type>::purge()
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    isEmpty_ = true;
}

template<class Type>
void Proxy<Type>::internalGet( Type & obj ) const
{
    obj = obj_;
}

template<class Type>    
void Proxy<Type>::internalSet( const Type & obj )
{
    obj_ = obj;
}

} // end namespace

#endif
