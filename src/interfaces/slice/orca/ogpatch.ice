/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_OGPATCH_ICE
#define ORCA_OGPATCH_ICE

#include <orca/common.ice>
#include <orca/bros1.ice>


module orca
{

/*!
    @ingroup orca_interfaces
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

class OGPatch
{
    //! Time when the object was observed, created, etc.
    Time timeStamp;
    Frame2d location; 
    Size2d sizeInMetres;  
    Size2d sizeInPixels; 
    ByteSeq array;
};

sequence<OGPatch> PatchList; 

/*! An OGPatchMap consists of a sequence of OGPatches. 
*/

class OGPatchMap
{
    //! Time when the object was observed, created, etc.
    Time timeStamp;
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
class OGPatchMapUpdateElement
{
    //! Time when the object was observed, created, etc.
    Time timeStamp;
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
   ByteSeq pixelData;  
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
  ["cpp:const"] idempotent OGPatchMap getMap(); 
  void subscribe(OGPatchMapConsumer *consumer); 
  idempotent void unsubscribe( OGPatchMapConsumer *subscriber );
};

}; // module

/*! @} */
#endif
