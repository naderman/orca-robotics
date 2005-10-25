/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
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

#ifndef ORCA2_POLARFEATURE_INTERFACE_ICE
#define ORCA2_POLARFEATURE_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{

// Define messages first
sequence<PolarPoint2d> PolarPoint2dSequence;


//! Data -- the 2D polar features
class PolarFeature2dData extends OrcaObject
{
    PolarPoint2dSequence features;
};

//! PolarFeature2D Interface
interface PolarFeature2d
{
    nonmutating PolarFeature2dData getData();
};

//! Consumer of PolarFeature2d data
interface PolarFeature2dConsumer
{    
    void setData( PolarFeature2dData obj );
};

}; // module

#endif
