/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_STOCHASTIC_ICE
#define ORCA2_STOCHASTIC_ICE

// Definitions in this file are ment to be building blocks used by interface definitions.

module orca
{

//! 2d Covariance Matrix
//! @see Frame2d
struct Covariance2d
{
    //! Matrix index (0,0)
    double xx;
    //! Matrix index (0,1)
    double xy;
    //! Matrix index (1,1)
    double yy;
    //! Matrix index (0,2)
    double xt;
    //! Matrix index (1,2)
    double yt;
    //! Matrix index (2,2)
    double tt;
};

}; // module

#endif
