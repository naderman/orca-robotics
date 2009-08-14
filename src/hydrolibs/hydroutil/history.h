/*
 * Orca-Robotics Project: Components for robotics 
             http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROUTIL_HISTORY_H
#define HYDROUTIL_HISTORY_H

//windows is stupid
#include <string>


namespace hydroutil
{

//! 
//! @brief Experimental! Storage of historical information about component's activities.
//! The idea is to keep persistent record of components startups and shutdowns.
//! 
class History
{
public:
    virtual ~History() {}; 

    //! Sets the contents of the history without modifying the message.
    virtual void set( const std::string& message )=0;

    //! Appends the message to the standard start sequence.
    virtual void setWithStartSequence( const std::string& message )=0;

    //! Appends the message to the standard finish sequence.
    virtual void setWithFinishSequence( const std::string& message )=0;

    //! Sets the contents of the history to the standard start sequence.
    //! If the history is non-empty, nothing will be written unless the @c force flag is set to TRUE.
    virtual void autoStart( bool force=false )=0;

    //! Sets the contents of the history to the standard finish sequence.
    //! If the history is non-empty, nothing will be written unless the @c force flag is set to TRUE.
    virtual void autoFinish( bool force=false )=0;

    //! Write current contents to the storage medium and clear current history contents.
    virtual void flush()=0;

};

} // namespace

#endif
