MESSAGE ( "== COMPONENTS ==" )

# when adding new directories, please maintain alphabetical order
SUBDIRS(
  lasermon
  lasermon
  position2dmon
  powermon
  segwayrmp
#  sicklaser <- MWK removed: sicklaser no longer in repository
  teleop
)

IF ( GENERATE_DOCUMENTATION AND DOXYGEN )
  ADD_CUSTOM_TARGET( docs ALL doxygen )
ENDIF ( GENERATE_DOCUMENTATION AND DOXYGEN )
