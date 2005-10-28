MESSAGE ( "== UTILITIES ==" )

# when adding new directories, please maintain alphabetical order
SUBDIRS(  
  liborcaguitools
  liborcaiceutil
  orcasmc
)

IF ( GENERATE_DOCUMENTATION AND DOXYGEN )
  ADD_CUSTOM_TARGET( docs ALL doxygen )
ENDIF ( GENERATE_DOCUMENTATION AND DOXYGEN )
