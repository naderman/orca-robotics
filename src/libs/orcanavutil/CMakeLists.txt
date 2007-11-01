INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

FILE( GLOB SRCS *.cpp )


GLOBAL_ADD_LIBRARY( OrcaNavUtil SHARED ${SRCS} )

TARGET_LINK_LIBRARIES( OrcaNavUtil HydroNavUtil )
INSTALL_FILES( /include/orcanavutil .*\\.h$ ) 
