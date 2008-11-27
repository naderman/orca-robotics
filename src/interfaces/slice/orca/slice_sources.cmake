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
    button.ice
    camera.ice
    cameracollection.ice
    cpu.ice
    drivebicycle.ice
    drivedifferential.ice
    estop.ice
    featuremap2d.ice
    gps.ice
    home.ice
    image.ice
    ins.ice
    imu.ice
    localise2d.ice
    localise3d.ice
    odometry2d.ice
    odometry3d.ice
    ogfusion.ice
    ogmap.ice
    particle2d.ice
    pathfollower2d.ice
    pathplanner2d.ice
    pinger.ice
    pixmap.ice
    pointcloud.ice
    polarfeature2d.ice
    power.ice
    properties.ice
    qgraphics2d.ice
    rangescanner2d.ice
    laserscanner2d.ice
    status.ice
    systemstatus.ice
    tracer.ice
    vehicledescription.ice
    velocitycontrol2d.ice
    wifi.ice
# only used for testing utilities
    test.ice
)
