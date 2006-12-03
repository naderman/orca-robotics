/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_BUFFER_H
#define ORCAICE_BUFFER_H

#include <queue>
#include <orcaice/common.h>
#include <orcaice/exceptions.h>

#include <Ice/Ice.h>
#include <IceUtil/Monitor.h>
#include <IceUtil/Time.h>

namespace orcaice
{

/*!
 *  @brief A thread-safe data pipe with buffer semantics.
 *
 * For a type-safe buffer, template over the specific Object type you want to put in it.
 * Buffering Ice smart pointers requires a specialized class Buffer.
 *
 * @note This implementation uses IceUtil threading classes. See example in sec. 28.9.2
 * @see PtrBuffer, Nofity, Proxy
 */
template<class Type>
class Buffer : public IceUtil::Monitor<IceUtil::Mutex>
{
public:

    /*!
     *  Buffer depth:
     *      - positive numbers to specify finite depth,
     *      - negative numbers for infinite depth, limited by memory size (DEFAULT)
     *      - zero is undefined
     *  Default buffer type is CIRCULAR, of depth 1.
     */
    Buffer( int depth=1, BufferType type=BufferTypeCircular );

    virtual ~Buffer();

    //! Typically is called before the buffer is used, or if, for some reason, the configuration
    //! information was not available at the time when the constructor was called.
    //! Careful: all data currently in the buffer is lost, because @ref purge is called.
    //! NOTE: can do smarter by trancating queue only as much as needed.
    void configure( int depth, BufferType type=BufferTypeCircular );

    //! Returns TRUE if there's something in the buffer.
    bool isEmpty() const;

    //! Returns the number of items in the buffer.
    int  size() const;

    //! Delete all entries, makes the buffer empty.
    void purge();

    /*!
     *  Add an object to the end of the buffer. If there is no room left in a finite-depth
     *  circular buffer, the front element of the buffer is quietly deleted and the new data
     *  is added to the end. If there is no room left in a finite-depth queue buffer
     *  the new data is quietly ignored.
     */
    void push( const Type & obj );

    /*! 
     *  Pop the front element off and discards it (usually after calling @ref get).
     *  If the buffer is empty this command is quietly ignored.
     */
    void pop();

    /*!
     *  Non-popping and non-blocking read from the front of the buffer.
     *
     *  Calls to @ref get on an empty buffer raises an orcaice::Exception exception.
     *  You can trap these and call @ref getNext which will block until new data arrives.
     */
    void  get( Type & obj ) const;

    /*!
     *  Array style access version
     */
    void  get( Type & obj, unsigned int n ) const;

    /*!
     *  Same as @ref get but calls @ref pop afterwards.
     */
    void  getAndPop( Type & obj );

    /*! 
     *  If the buffer is empty, @ref getNext blocks until an object is pushed in and returns the new value.
     *  By default, there is no timeout (negative value). Returns 0 if successful.
     *
     *  If timeout is set to a positive value and the wait times out, the function returns -1
     *  and the object argument itself is not touched. In the rare event of spurious wakeup,
     *  the return value is 1.
     */
    int  getNext( Type & obj, int timeoutMs=-1 );

    /*!
     *  Same as @ref getNext but calls @ref pop afterwards.
     */
    int  getAndPopNext( Type & obj, int timeoutMs=-1 );

protected:
    
    // The buffer itself
    std::deque<Type> queue_;

    // Reimplement this function for non-standard types.
    virtual void internalGet( Type & obj ) const ;

    // Reimplement this function for non-standard types.
    virtual void internalGet( Type & obj, unsigned int n ) const ;
    
    // Reimplement this function for non-standard types.
    virtual void internalPush( const Type & obj );

private:

    // buffer depth:
    //      positive numbers to specify finite depth,
    //      negative numbers for infinite depth (memory size),
    //      zero is undefined
    int depth_;

    // buffer type (see type definitions in BufferType enum)
    BufferType type_;

    // internal implementation of front( obj, -1 ); returns 0.
    int  getNextNoWait( Type & obj );
};


//////////////////////////////////////////////////////////////////////


template<class Type>
Buffer<Type>::Buffer( int depth, BufferType type )
    : depth_(depth),
      type_(type)
{
    purge();
}

template<class Type>
Buffer<Type>::~Buffer()
{
}

template<class Type>
void Buffer<Type>::configure( int depth, BufferType type )
{
    // all data is lost!
    purge();

    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    depth_ = depth;
    type_ = type;
}

template<class Type>
void Buffer<Type>::purge()
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    queue_.resize(0);
}

template<class Type>
bool Buffer<Type>::isEmpty() const
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    return queue_.empty();
}

template<class Type>
int Buffer<Type>::size() const
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    return queue_.size();
}

template<class Type>
void Buffer<Type>::pop()
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    if ( queue_.empty() ) {
        return;
    }
    // must check for empty queue above, otherwise get seg fault!
    queue_.pop_front();
}

template<class Type>
void Buffer<Type>::get( Type &obj ) const
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    if ( !queue_.empty() )
    {
        internalGet( obj );
    }
    else
    {
        throw orcaice::Exception( ERROR_INFO, "trying to read from an empty buffer." );
    }
}

template<class Type>
void Buffer<Type>::get( Type &obj, unsigned int n ) const
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    if ( queue_.empty() ){
        throw orcaice::Exception( ERROR_INFO, "trying to read from an empty buffer." );
    }
    else if( n >= queue_.size()){
        throw orcaice::Exception( ERROR_INFO, "index out of bounds while trying to read buffer." );
    }
    else{
        internalGet( obj ,n );
    }
}

template<class Type>
void Buffer<Type>::getAndPop( Type &obj )
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    if ( !queue_.empty() )
    {
        internalGet( obj );
    }
    else
    {
        throw orcaice::Exception( ERROR_INFO, "trying to read from an empty buffer." );
    }
    queue_.pop_front();
}

template<class Type>
int Buffer<Type>::getNext( Type &obj, int timeoutMs )
{
    // special case: infinite wait time
    if ( timeoutMs == -1 ) 
    {
        return getNextNoWait( obj );
    }

    // finite wait time
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

    // if already have data in the buffer, return it and get out
    if ( !queue_.empty() )
    {
        internalGet( obj );
        return 0;
    }

    // empty buffer: figure out when to wake up
    // notice that we are still holding the lock, so it's ok to call timedWait()
    if (  this->timedWait( IceUtil::Time::milliSeconds( timeoutMs ) ) )  
    {
        // someone woke us up, we are holding the lock again
        // check new data again (could be a spurious wakeup)
        if ( !queue_.empty() ) 
        {
            internalGet( obj );
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
int Buffer<Type>::getAndPopNext( Type &obj, int timeoutMs )
{
    int ret = getNext( obj, timeoutMs );
    if ( ret==0 ) {
        pop();
    }
    return ret;
}

// internal utility function (waits for update infinitely)
template<class Type>
int Buffer<Type>::getNextNoWait( Type &obj )
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    
    // check the condition before and after waiting to deal with spurious wakeups
    // (see Ice manual sec. 28.9.2)
    while ( queue_.empty() ) 
    {
        this->wait();
    }
    
    internalGet( obj );
    return 0;
}

// NOTE: see notes on efficient notification in Ice sec. 28.9.3
template<class Type>
void Buffer<Type>::push( const Type & obj )
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

    // buffer is not full, or buffer is configured to be of infinite size (at least for STL)
    if ( (int)queue_.size() < depth_ || depth_<0 )
    {
        internalPush( obj );
    }
    else if ( type_ == BufferTypeCircular )
    {
        // pop the oldest entry
        queue_.pop_front();
        // push the new enty
        internalPush( obj );
    }
    else // we have a full, non-circular buffer
    {
        // do nothing, the new object is lost
    }

    // wakeup someone who's waiting for an update
    this->notify();
}

template<class Type>
void Buffer<Type>::internalGet( Type & obj ) const
{
    obj = queue_.front();
}

template<class Type>
void Buffer<Type>::internalGet( Type & obj, unsigned int n ) const
{
    obj = queue_[n];
}

template<class Type>    
void Buffer<Type>::internalPush( const Type & obj )
{
    queue_.push_back( obj );
}

} // end namespace

#endif
