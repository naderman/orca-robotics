// /*
//  * Orca Project: Components for robotics 
//  *               http://orca-robotics.sf.net/
//  * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
//  *
//  * This copy of Orca is licensed to you under the terms described in the
//  * ORCA_LICENSE file included in this distribution.
//  *
//  */
 
// #ifndef ORCAIFACEIMPL_BUFFERED_CONSUMER_I_H
// #define ORCAIFACEIMPL_BUFFERED_CONSUMER_I_H

// #include <orcaice/buffer.h>

// namespace orcaifaceimpl
// {

// /*!
//  *  @brief A convenience class for consumers with setData() operation.
//  *
//  *  Relies on the fact that the Consumer objects has only one operation
//  *  to implement and it's called setData().
//  */
// template<class ConsumerType, class ObjectType>
// class BufferedConsumerI : public ConsumerType
// {
// public:
//     //! Passes the arguments to its internal buffer.
//     BufferedConsumerI( int bufferDepth, orcaice::BufferType bufferType ) : 
//         buffer_(bufferDepth,bufferType) {};

//     // from ConsumerType
//     virtual void setData( const ObjectType& data, const Ice::Current& )
//     {
//         // push recieved object into the internal buffer
//         buffer_.push( data );
    
//         // call event handler which can be reimplemented in derived classes
//         handleData( data );
//     }

//     //! Access to the internal buffer.
//     orcaice::Buffer<ObjectType> &buffer() { return buffer_; }

// protected:

//     //! You can reimplement this function in the derived class and do something when 
//     //! an object is received (besides sticking it in the buffer). 
//     //! This adds the functionality of orcaice::Notify.
//     virtual void handleData( const ObjectType & obj ) {};

// private:
//     orcaice::Buffer<ObjectType> buffer_;
// };

// } // namespace

// #endif
