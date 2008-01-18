SET ( LIB_NAME OrcaObjects )
GLOBAL_ADD_LICENSE( LGPL )

# This is one of fundamental libs. It is not optional (but maybe it should change)

# Do not include UseComponentRules here because it will link to OrcaIce
# creating circular dependency.
INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseHydroUtil.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )

FILE( GLOB SRCS *.cpp )

# for some reason cannot get shared libs working in windows
IF(NOT OS_WIN)
  GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${SRCS} )
ELSE (NOT OS_WIN)
  GLOBAL_ADD_LIBRARY( ${LIB_NAME} STATIC ${SRCS} )
ENDIF (NOT OS_WIN)

TARGET_LINK_LIBRARIES( ${LIB_NAME} OrcaInterfaces )

# special dependency on the generated files
ADD_DEPENDENCIES( ${LIB_NAME} OrcaInterfaces )

INSTALL_FILES( /include/orcaobj .*\\.h$ )

IF ( BUILD_TESTS )
    ADD_SUBDIRECTORY ( test )
ENDIF ( BUILD_TESTS )
