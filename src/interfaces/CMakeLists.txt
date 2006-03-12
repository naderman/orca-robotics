#
# Possibly build the documentation
#
IF ( GENERATE_DOCUMENTATION AND DOXYGEN )
  ADD_CUSTOM_TARGET( docs ALL doxygen )
ENDIF ( GENERATE_DOCUMENTATION AND DOXYGEN )

SUBDIRS( 
  cpp
  slice
)