#ifndef ORCA2_POINT_CLOUD_INTERFACE_ICE
#define ORCA2_POINT_CLOUD_INTERFACE_ICE
#include "orca/orca.ice" 
#include "orca/bros1.ice"

module orca {
/*!
    @ingroup interfaces
    @defgroup orca_interface_pointcloud PointCloud
    @brief Interface for point cloud producers and consumers

The Pointcloud interface supports devices that generate point clouds of some kind. This 
can include range imagers, data loaded from files or 3d scanning lasers. 

Because of the wide variety of devices, it is difficult to define a
single configuration interface. Particular devices may wish to
subclass the point cloud interfac.

    @{
*/

/*! Currently supports uncoloured and coloured point clouds. 
*/ 

enum PointCloudType { PCXYZ, PCXYZRGB }; 

/*! While it would be nice from a software engineering point of view
to have a collection of points as structs or classes, OpenGL works
better with an interleaved arrays, otherwise we'd be spending a lot of
time just packing and unpacking stuff. 
*/ 
sequence<float> FloatSequence; 

class PointCloudProducerGeometry extends OrcaObject {
  
  //! Offset of the centre of the sensor from the robot, int the robot CS
  Frame3d offset; 

 //! Dimensions of the sensor
  Size3d size; 
};

//! Point cloud Geometry data structure
class PointCloud extends OrcaObject {
  PointCloudType type; 

  /*! Points are interleaved. if the Type is PCXYZ, then the sequence
is X0,Y0,Z0,X1,Y1,Z1... If the type is PCXYZRGB, then it is
X0,Y0,Z0,R0,G0,B0,.... Colours are specified between 0.0 and 1.0  */ 
  FloatSequence points;
};

/*@ Point cloud consumer */
 
interface PointCloudConsumer {
  void setData(PointCloud data);   
};

interface PointCloudProducer {
  PointCloudProducerGeometry getGeometry();  
  PointCloud getPointCloud(); 
  idempotent void subscribe(PointCloudConsumer *consumer); 
  idempotent void unsubscribe(PointCloudConsumer *consumer); 
};

/*! @} */
};

#endif

