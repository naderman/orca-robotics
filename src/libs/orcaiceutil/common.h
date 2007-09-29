/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICEUTIL_COMMON_H
#define ORCAICEUTIL_COMMON_H

// This file contains common definitions used throughout orcaiceutil:: namespace.

namespace orcaiceutil
{

//! %Buffer type defines behavior when the buffer is full
enum BufferType
{
    //! When the maximum depth of the buffer is reached, removes the oldest entry to
    //! make room for the new data.
    BufferTypeCircular,
    //! When the maximum depth of the buffer is reached, ignores new incoming data,
    //! until the oldest entry is popped to make room.
    BufferTypeQueue
};

} // end namespace

#endif
