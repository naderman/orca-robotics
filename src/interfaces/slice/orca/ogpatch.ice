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

#ifndef ORCA_OGPATCH_ICE
#define ORCA_OGPATCH_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>


module orca
{

/*!
    @ingroup interfaces
    @defgroup orca_interface_ogpatch OGPatch
    @brief An occupancy grid patch 

An occupancy grid patch (and the associated OGPatchMap class)
represent a world as a set of patches. Rather than having a single
occupancy grid for the entire world; the world consists of a group of
patches. This allows more flexibility rather than a single OG Map, for
example, it allows the propagation of updates through the patch
sequence simply. 
 
 @{
*/

class OGPatch extends OrcaObject
{
    Frame2d location; 
    Size2d sizeInMetres;  
    Size2d sizeInPixels; 
    ByteSequence array;
};

sequence<OGPatch> PatchList; 

/*! An OGPatchMap consists of a sequence of OGPatches. 
*/

class OGPatchMap extends OrcaObject
{
    PatchList patches; 
};

/*! 
The OGPatchMapUpdateElement is one part of the update of an
OGPatchMap. This means that robots -- especially distributed robots --
don't have to send their entire map, but can just send the
updates. There are 3 updates possible: adding a new map, updating an
part of an existing patch, or changing the position of the OG
patches. 

For now, this code is not used, but it is an indication of how such an
update capability could be built.

*/

/*
class OGPatchMapUpdateElement extends OrcaObject {

}; 

class OGPatchMapNewPatch extends OGPatchMapUpdateElement {
   int insertionPoint; 
   OGPatch newPatch; 
};

class OGPatchMapUpdateSubPatch extends OGPatchMapUpdateElement {
   int patchId; 
   int xOffset;
   int yOffset; 
   Size2d patchSizeInPixels;
   ByteSequence pixelData;  
};

class OGPatchMapUpdateFrame extends OGPatchMapUpdateElement {
  int patchId; 
  Frame2d updatedFrame; 
};

sequence<OGPatchMapUpdateElement> OGPatchUpdateList; 

*/

interface OGPatchMapConsumer{
   void receiveMap(OGPatchMap map); 
};
 

interface OGPatchMapProvider {
  nonmutating OGPatchMap getMap(); 
  void subscribe(OGPatchMapConsumer *consumer); 
  idempotent void unsubscribe( OGPatchMapConsumer *subscriber );
};

}; // module

/*! @} */
#endif
