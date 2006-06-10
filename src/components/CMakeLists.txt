# A place for well-tested and well-documented components.

MESSAGE ( "== COMPONENTS ==" )

# when adding new directories, please maintain alphabetical order
SUBDIRS(
  faithlocaliser
  featuremaploader
  featuremap2dmon
  laserfeatureextractor
  lasermon
  localnav
  logger
  logplayer
  ogmaploader
  orcamon
  orcaview
  pathplanner
  position2dmon
  registrylist
  registryview
  robot2d
  sicklaser
  simlocaliser
  teleop
  imageserver
  imageviewer
)
