INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

LINK_DIRECTORIES( ${ICE_HOME}/lib
                  ${PROJECT_BINARY_DIR}/src/utils/orcamisc
                  ${PROJECT_BINARY_DIR}/src/utils/orcaice
                  ${PROJECT_BINARY_DIR}/src/interfaces/cpp/orca )

FILE( GLOB SRCS *.cpp )

IF ( NOT WIN32 )
  GLOBAL_ADD_LIBRARY( OrcaCompModel SHARED ${SRCS} )
  TARGET_LINK_LIBRARIES( OrcaCompModel OrcaMisc )
ELSE ( NOT WIN32 )
  GLOBAL_ADD_LIBRARY( OrcaCompModel STATIC ${SRCS} )
  TARGET_LINK_LIBRARIES( OrcaCompModel 
              optimized IceGrid debug IceGridd
              optimized Glacier2 debug Glacier2d
              OrcaMisc )
ENDIF ( NOT WIN32 )

# special dependency on the generated files
ADD_DEPENDENCIES( OrcaCompModel OrcaInterfaces )

INSTALL_FILES( /include/orcacm .*\\.h$ )
