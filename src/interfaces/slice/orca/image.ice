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

#ifndef ORCA_IMAGE_ICE
#define ORCA_IMAGE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice> 

module orca
{

//! Specifies the format once it is decoded. 
enum ImageFormat {IMGMONO1, IMGMONO8, IMGMONO16,
IMGRGB565, IMGRGB888, IMGRGBA8888, IMGYUV422};

//! Specifies any encoding of the image. 
enum ImageCompression {IMGRAW, IMGJPG, IMGPNG}; 

class ImageConfig extends OrcaObject {
   int imgWidth; 
   int imgHeight;
   float frameRate; 
   ImageFormat format;
   ImageCompression compression; 
   //! Allows the camera to focus on regions of interest. 
   bool roiOn; 
   int roiCentreX; 
   int roiCentreY;
   int roiWidth;
   int roiHeight; 
  
}; 

class ImageGeometry extends OrcaObject 
{
   Frame3d offset; 
}; 

class Image extends OrcaObject
{
    int width;
    int height;
    long imageId; //! For sequences of images. 
    ImageFormat format; 
    ImageCompression encoding; 
    
   //! Note: This array should also contain any
   //! compression specific header information  
    ByteSequence imageData;
};

/*! 
	@brief Something that provides images. 

*/ 

interface ImageConsumer 
{
	void newImage(Image img); 
};

interface Imager 
{
	nonmutating Image getImage(); 
	nonmutating ImageConfig getConfig();
        nonmutating ImageGeometry getGeometry();

	idempotent void setConfig( ImageConfig config) throws ConfigurationNotExistException;

   	void subscribe (ImageConsumer *subscriber); 
        
	void unsubscribe (ImageConsumer *subscriber); 
}; 


/*! 
	Specifies a range of ImageIds -- this would typically 
	be used to query a caching imager for what images it has.
*/ 

struct ImageIdRange {
  long start;
  long end; 
}; 

/*! A set of ranges, just in case there were a few defective images or 
something. */

sequence<ImageIdRange> ImageIdRangeSet;

/*! 
Special class for imagers that provide caching. May not be necessary
to implement this class; in many cases, Imager should be enough. 

Examples of this would be things that provide images from 
a directory or an AVI file or who keep the last few images.

An especially useful feature is to be able to request the image that 
was closest temporally to a particular other measurement. 

*/

interface CachingImager extends Imager 
{
	ImageIdRangeSet getAvailableImageIds();
	Image getImageId(long imageId); 
	Image getImageClosestTo(Time closestTime); 
};

}; // module

#endif
