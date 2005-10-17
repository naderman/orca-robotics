SUBDIRS(
  lasermon
#  playerlaser
  lasermon
  position2dmon
  powermon
  segwayrmp
  teleop
)

IF ( GENERATE_DOCUMENTATION AND DOXYGEN )
  ADD_CUSTOM_TARGET( docs ALL doxygen )
ENDIF ( GENERATE_DOCUMENTATION AND DOXYGEN )