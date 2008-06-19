#
# When you add a new .ice file, insert its name into this list. This is
# the only file which has to be updated.
#

#
# Since CMake doesn't know how to automatically track dependencies for .ice files, 
# these have to be entered manually in proper order: such that the depended-on
# files are listed first.
#
SET( ORCA_SLICE_SOURCE_FILES
# definitions
    datetime.ice
    exceptions.ice
    containers.ice
    common.ice
    ocm.ice
    bros1.ice
    stochastic.ice
#interfaces
    binaryindicator.ice
    binaryswitch.ice
    button.ice
    camera.ice
    cpu.ice
    drivebicycle.ice
    drivedifferential.ice
    estop.ice
    featuremap2d.ice
    gps.ice
    hand.ice
    home.ice
    image.ice
    ins.ice
    imu.ice
    localise2d.ice
    localise3d.ice
    log.ice
    multicamera.ice
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
    properties.ice
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
