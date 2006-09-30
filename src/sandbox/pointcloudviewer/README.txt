pointcloudviewer connects to a pointcloudserver, grabs a point cloud, 
and renders it in 3D. It connects to a pointcloudserver on the localhost 
at port 10101. 

It is a hack to demonstrate point cloud usage. It is not up to the usual 
ORCA coding standards and should not be used to judge ORCA. 

Typical command line: 

% pointcloudviewer

or 

% pointcloudviewer --Ice.MessageSizeMax=200000000

The message max size is required if you are passing around big point 
files. The MessageSizeMax should be at least 24 times the number of 
points in a point cloud. 

The viewer can be controlled via the keyboard. 

Keyboard commands include:

* W,A,S,D for translating the scene. 
* [ and ] for zooming in and out. 
* Arrow keys for controlling pitch and yaw. 
* R resets the view. 
* O and P decrease and increase the point size. 
* C to toggle point cloud display 
* L to toggle grid line display. 
* G to toggle occupancy grid display
* B to toggle point cloud border display 
