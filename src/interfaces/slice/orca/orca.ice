/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
sequence<byte> ByteSequence;


}; // module

#endif
