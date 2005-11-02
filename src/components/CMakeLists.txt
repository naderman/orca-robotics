MESSAGE ( "== COMPONENTS ==" )

# when adding new directories, please maintain alphabetical order
SUBDIRS(
  laserfeatureextractor
  lasermon
  logger
  orcaguilocal
  peoplecounter
  position2dmon
  segwayrmp
  sicklaser 
  statusmon
  teleop
)

IF ( GENERATE_DOCUMENTATION AND DOXYGEN )
  ADD_CUSTOM_TARGET( docs ALL doxygen )
ENDIF ( GENERATE_DOCUMENTATION AND DOXYGEN )
