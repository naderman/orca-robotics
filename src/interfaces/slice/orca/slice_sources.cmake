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
    camera.ice
    cpu.ice
    drivebicycle.ice
    drivedifferential.ice
    featuremap2d.ice
    gps.ice
    hand.ice
    home.ice
    image.ice
    imu.ice
    localise2d.ice
    localise3d.ice
    log.ice
    odometry2d.ice
    odometry3d.ice
    ogfusion.ice
    ogmap.ice
    ogpatch.ice 
    particle2d.ice
    pathfollower2d.ice
    pathplanner2d.ice
    pinger.ice
    pixmap.ice
    pointcloud.ice
    polarfeature2d.ice
    power.ice
    ptz.ice 
    qgraphics2d.ice
    rangescanner2d.ice
    laserscanner2d.ice
    replay.ice
    status.ice
    tracer.ice
    vehicledescription.ice
    velocitycontrol2d.ice
    wifi.ice
)
