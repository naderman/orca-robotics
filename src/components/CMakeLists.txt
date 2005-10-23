MESSAGE ( "== COMPONENTS ==" )

# when adding new directories, please maintain alphabetical order
SUBDIRS(
  drunkrobot
  lasermon
  position2dmon
  powermon
  segwayrmp
  sicklaser 
  teleop
)

IF ( GENERATE_DOCUMENTATION AND DOXYGEN )
  ADD_CUSTOM_TARGET( docs ALL doxygen )
ENDIF ( GENERATE_DOCUMENTATION AND DOXYGEN )
