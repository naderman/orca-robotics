pointcloudserver loads a point cloud from a VRML file, or if none is 
given, creates a colour sphere to provide to the client (usually point 
cloud viewer). It sets itself up on port 10101. 

Example VRML files can be found at: 

http://kodiak.cse.unsw.edu.au/data/wrl/ 

It is a hack to demonstrate point cloud usage. It is not up to the usual 
ORCA coding standards and should not be used to judge ORCA. 

Typical command line: 

% pointcloudserver 

or 

% pointcloudserver --Ice.MessageSizeMax=200000000 test5.wrl

The message max size is required if you are passing around big point 
files. The MessageSizeMax should be at least 24 times the number of 
points in a point cloud. 

