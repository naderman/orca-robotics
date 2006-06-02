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
  orcalogplayer
  orcamisc
  orcanavutil
  orcaobj
  orcaogmap
  orcapathplan
  libqorcauml
  orcaplayer
  qorcacm
#  qorcagui
# UTILITY APPLICATIONS
  cleanregistry
  gorca
  iceping
  orcasmc
)
