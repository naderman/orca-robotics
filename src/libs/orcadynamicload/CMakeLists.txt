IF ( NOT WIN32 )

#  INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
#  INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )

  FILE( GLOB SRCS *.cpp )
  
  INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs )
  
  GLOBAL_ADD_LIBRARY( OrcaDynamicLoad SHARED ${SRCS} )
  
  INSTALL_FILES( /include/orcadynamicload .*\\.h$ ) 

ENDIF ( NOT WIN32 )
