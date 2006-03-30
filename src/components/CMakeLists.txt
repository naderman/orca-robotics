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
  position2dmon
  registryview
  segwayrmp
  sicklaser
  simlocaliser
  teleop
  imageserver
  imageviewer
)
