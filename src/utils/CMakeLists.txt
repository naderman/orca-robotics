MESSAGE ( "== UTILITIES ==" )

# when adding new directories, please maintain alphabetical order
SUBDIRS(  
# LIBRARIES
#  liborcaguitools
  liborcaiceutil
  libqorcauml
# UTILITY APPLICATIONS
  iceping
#  nsview
  orcasmc
)

IF ( GENERATE_DOCUMENTATION AND DOXYGEN )
  ADD_CUSTOM_TARGET( docs ALL doxygen )
ENDIF ( GENERATE_DOCUMENTATION AND DOXYGEN )
