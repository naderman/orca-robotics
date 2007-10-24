INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseHydroUtil.cmake )

FILE( GLOB SRCS *.cpp )

IF(NOT OS_WIN)
    GLOBAL_ADD_LIBRARY( OrcaDef SHARED ${SRCS} )
ELSE (NOT OS_WIN)
    # for some reason cannot get shared libs working in windows
    GLOBAL_ADD_LIBRARY( OrcaDef STATIC ${SRCS} )
ENDIF (NOT OS_WIN)

TARGET_LINK_LIBRARIES( OrcaDef OrcaObjects )

# special dependency on the generated files
ADD_DEPENDENCIES( OrcaDef OrcaObjects )

INSTALL_FILES( /include/orcadef .*\\.h$ )

IF ( BUILD_TESTS )
    ADD_SUBDIRECTORY ( test )
ENDIF ( BUILD_TESTS )
