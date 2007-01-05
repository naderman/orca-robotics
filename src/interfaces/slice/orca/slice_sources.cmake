#
# When you add a new .ice file, insert its name into this list. This is
# the only file which has to be updated.
#

#
# Since CMake doesn't know how to automatically track dependencies for .ice files, 
# these have to be entered manually in proper order: such that the depended-on
# files are listed first.
#
SET( SLICE_SOURCE_FILES
# definitions
    ocm.ice
    datetime.ice
    exceptions.ice
    orca.ice
    bros1.ice
    stochastic.ice
#interfaces
    binaryindicator.ice
    binaryswitch.ice
    cpu.ice
    drivebicycle.ice
    drivedifferential.ice
    gps.ice
    home.ice
    rangescanner2d.ice
    laserscanner2d.ice
    ogfusion.ice
    ogmap.ice
    pathplanner2d.ice
    pingreply.ice
    position2d.ice
    position3d.ice
    platform2d.ice
    polarfeature2d.ice
    power.ice
    status.ice
    ptz.ice 
    hand.ice
    odometry2d.ice
    ogpatch.ice 
    localise2d.ice
    particle2d.ice
    pathfollower2d.ice
    featuremap2d.ice
    camera.ice
    qgraphics2d.ice
    imu.ice
    pointcloud.ice
    tracer.ice
    velocitycontrol2d.ice
    wifi.ice
)
