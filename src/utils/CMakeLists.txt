# A place for well-tested and well-documented libraries and utility applications.

MESSAGE ( "== UTILITIES ==" )

# when adding new directories, please maintain alphabetical order 
# unless there are inter-dependencies.
SUBDIRS(  
# LIBRARIES
  orcaice
  orcacm
  orcadef
  orcagui
  libqorcauml
  orcaplayer
  qorcacm
# UTILITY APPLICATIONS
  iceping
  gorca
  orcasmc
)
