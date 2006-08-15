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
    orca.ice
    bros1.ice
    stochastic.ice
# objects
#    image.ice
    ogmap.ice
    opaque.ice
#interfaces
    binaryindicator.ice
    gps.ice
    home.ice
    rangescanner.ice
    laser.ice
    ogfusion.ice
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
    ogpatch.ice 
    localise2d.ice
    particle2d.ice
    pathfollower2d.ice
    featuremap2d.ice
    camera.ice
    qgraphics2d.ice
    imu.ice
)
