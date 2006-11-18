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

/*!
    @brief Defines cross-platform interfaces and data objects.
    @namespace orca

    This namespace defines the core reusable part of Orca: interfaces and objects.
*/
module orca
{

/*!
    @brief Unix absolute time
*/
struct Time
{
    //! Number of seconds.
    int seconds;
    //! Number of microseconds.
    int useconds;
};

/*!
    Time of day.
*/
struct TimeOfDay
{
    //! Hour
    int hours;
    //! Minutes
    int minutes;
    //! Seconds
    double seconds;
};

/*!
    @brief Date of the year.
*/
struct Date
{    
    //! Day
    int day;
    //! Month
    int month;
    //! Year
    int year;
};

/*!
    @brief Base class for all data types in Orca.
    
    Deriving from a single class allows polymorphism.
    
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

/*!
    @brief A generic run-time exception.
    
    Orca Components can use this to signal error conditions to their clients.
*/
exception OrcaException
{
    //! Error description.
    string what;
};

//! Server failed to configure itself as requrested by client.
exception ConfigurationNotExistException extends OrcaException {};

/*!
    Raised when the server does not have the requested data.

    Typically, this is because the server has not fully initialized yet.
*/
exception DataNotExistException extends OrcaException {};

//! Indicates a problem with robot hardware, e.g. sensors and actuators.
exception HardwareFailedException extends OrcaException {};

//! Raised when the server fails to subscribe client for periodic updates.
exception SubscriptionFailedException extends OrcaException {};

//! Raised when the component is unable to process requests.
exception BusyException extends OrcaException {};

//! Raised when a component failed to connect to a required interface
exception RequiredInterfaceFailedException extends OrcaException {};

//! Raised when one or more parameters to a remote request were malformed.
exception MalformedParametersException extends OrcaException {};

//! Raised when a remote operation is not implemented.
exception OperationNotImplementedException extends OrcaException {};

}; // module

#endif
