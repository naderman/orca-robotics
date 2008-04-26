/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCA_ICE
#define ORCA2_ORCA_ICE

#include <orca/ocm.ice>
#include <orca/datetime.ice>
#include <orca/exceptions.ice>

/*!
    @brief Defines cross-platform interfaces and data objects.
    @namespace orca

    This namespace defines the core reusable part of Orca: interfaces and objects.
*/
module orca
{

/*!
    @brief Base class for data types in Orca.
    
    Deriving from this class is optional. It allows polymorphism. 
    
    @note An Orca @e object refers to a piece of data whereas an Ice @e object which
    refers to a remote servant.
*/
class OrcaObject
{
    //! Time when the object was observed, created, etc.
    Time timeStamp;
};

//! A byte array which can store any information.
// sequence<byte> ByteSequence;

/** A sequence of bools. **/
sequence<bool> BoolSeq;

/** A sequence of bytes. **/
sequence<byte> ByteSeq;

/** A sequence of shorts. **/
sequence<short> ShortSeq;

/** A sequence of ints. **/
sequence<int> IntSeq;

/** A sequence of longs. **/
sequence<long> LongSeq;

/** A sequence of floats. **/
sequence<float> FloatSeq;

/** A sequence of doubles. **/
sequence<double> DoubleSeq;

/** A sequence of strings. **/
sequence<string> StringSeq;

/** A sequence of objects. **/
sequence<Object> ObjectSeq;
    
/** A sequence of object proxies. **/
sequence<Object*> ObjectProxySeq;

}; // module

#endif
