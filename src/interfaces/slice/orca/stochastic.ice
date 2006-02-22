/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
    //! (0,0)
    float xx;
    //! (0,1)
    float xy;
    //! (1,1)
    float yy;
    //! (0,2)
    float xt;
    //! (1,2)
    float yt;
    //! (2,2)
    float tt;
};

}; // module

#endif
